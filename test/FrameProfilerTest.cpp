#include "wlf_core/Profiling.hpp"

#include "gtest/gtest.h"
#include <chrono>
#include <optional>
#include <thread>


using namespace wlf;
using namespace wlf::util;

class FrameProfilerTest : public ::testing::Test {
protected:
   void SetUp() override {
      m_FrameProfiler = std::make_optional(FrameProfiler{
         N_BUFFERED_FRAMES, *MultiStopwatch::FromBuilder(
                               MultiStopwatchBuilder{N_STOPWATCHES}
                                  .WithStopwatchName(0, "Calculating AAA")
                                  .WithStopwatchName(1, "Rendering BBB")
                                  .WithStopwatchName(2, "Loading CCC"))});
   }

   void BriefSleep() {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }

   void RunFrame() {
      m_FrameProfiler->StartNewFrame();

      m_FrameProfiler->BeginMeasureOf(FramePart::CALCULATING_AAA);
      m_FrameProfiler->BeginMeasureOf(FramePart::RENDERING_BBB);
      BriefSleep();
      m_FrameProfiler->EndMeasureOf(FramePart::CALCULATING_AAA);
      m_FrameProfiler->EndMeasureOf(FramePart::RENDERING_BBB);
   }

   enum FramePart : usize {
      CALCULATING_AAA,
      RENDERING_BBB,
      LOADING_CCC,
   };

   constexpr static usize N_BUFFERED_FRAMES = 3;
   constexpr static usize N_STOPWATCHES     = 3;
   std::optional<FrameProfiler> m_FrameProfiler;
};

TEST_F(FrameProfilerTest, OneSectionManyMeasurementsPerFrame) {
   m_FrameProfiler->StartNewFrame();
   m_FrameProfiler->BeginMeasureOf(FramePart::CALCULATING_AAA);
   BriefSleep();
   m_FrameProfiler->EndMeasureOf(FramePart::CALCULATING_AAA);
   auto aaaOld =
      *m_FrameProfiler->CurrentCumulativeTimingOf(FramePart::CALCULATING_AAA);

   m_FrameProfiler->BeginMeasureOf(FramePart::RENDERING_BBB);
   BriefSleep();
   m_FrameProfiler->EndMeasureOf(FramePart::RENDERING_BBB);
   auto bbb =
      *m_FrameProfiler->CurrentCumulativeTimingOf(FramePart::RENDERING_BBB);

   m_FrameProfiler->BeginMeasureOf(FramePart::CALCULATING_AAA);
   m_FrameProfiler->EndMeasureOf(FramePart::CALCULATING_AAA);
   auto aaaNew =
      *m_FrameProfiler->CurrentCumulativeTimingOf(FramePart::CALCULATING_AAA);

   EXPECT_LE(aaaOld, aaaNew)
      << "Should add timing after measuring same key in one frame";
   EXPECT_EQ(
      *m_FrameProfiler->CurrentCumulativeTimingOf(FramePart::RENDERING_BBB),
      bbb)
      << "Should keep other timings untouched";
   EXPECT_EQ(
      *m_FrameProfiler->CurrentCumulativeTimingOf(FramePart::LOADING_CCC), 0)
      << "Unmeasured keys should remain with 0 timing";
}

TEST_F(FrameProfilerTest, BufferingFramesMeasurements) {
   usize nTestFrames = 10;
   std::vector<std::vector<u64>> measurementsHistory(nTestFrames);
   for(usize i = 0; i < nTestFrames; ++i) {
      measurementsHistory[i] = std::vector<u64>(N_STOPWATCHES);
   }
   for(usize i = 0; i < nTestFrames; ++i) {
      for(usize s = 0; s < N_STOPWATCHES; ++s) {
         EXPECT_FALSE(m_FrameProfiler->HistoricalTimingOf(s, i))
            << "No records should should be returned before any measurements";
      }
   }

   for(usize currentFrame = 1; currentFrame < nTestFrames; ++currentFrame) {
      RunFrame();
      usize maxFramesBack = std::min(currentFrame, N_BUFFERED_FRAMES);
      for(usize s = 0; s < N_STOPWATCHES; ++s) {
         EXPECT_TRUE(m_FrameProfiler->CurrentCumulativeTimingOf(s))
            << "Measurement of this frame should be accessible";
         measurementsHistory[currentFrame][s] =
            *m_FrameProfiler->CurrentCumulativeTimingOf(s);
      }
      for(usize i = 1; i <= maxFramesBack; ++i) {
         for(usize s = 0; s < N_STOPWATCHES; ++s) {
            EXPECT_TRUE(m_FrameProfiler->HistoricalTimingOf(s, i))
               << "Measurements of old buffered frame should be accessible";
            EXPECT_EQ(measurementsHistory[currentFrame - i][s],
                      *m_FrameProfiler->HistoricalTimingOf(s, i))
               << "Measurements of old buffered frame should be untouched";
         }
      }
      for(usize i = maxFramesBack + 1; i < nTestFrames; ++i) {
         for(usize s = 0; s < N_STOPWATCHES; ++s) {
            EXPECT_FALSE(m_FrameProfiler->HistoricalTimingOf(s, i))
               << "Very old measurements should be gone for:"
               << " currentFrame=" << currentFrame << " nFramesBack=" << i
               << " stopwatchKey=" << s;
         }
      }
   }
}

TEST_F(FrameProfilerTest, GetTimingInterchangeability) {
   for(usize i = 0; i < 4; ++i) {
      RunFrame();
      for(usize s = 0; s < N_STOPWATCHES; ++s) {
         auto old     = m_FrameProfiler->HistoricalTimingOf(s, 0);
         auto current = m_FrameProfiler->CurrentCumulativeTimingOf(s);
         EXPECT_EQ(old, current)
            << "GetTimingOf(s) and GetOldTimingOf(s, 0) should be the same";
      }
   }
}

TEST_F(FrameProfilerTest, DetermenisticInterface) {
   for(usize i = 0; i < 4; ++i) {
      RunFrame();
      for(usize s = 0; s < N_STOPWATCHES; ++s) {
         for(usize t = 0; t < 3; ++t) {
            EXPECT_EQ(m_FrameProfiler->CurrentCumulativeTimingOf(s),
                      m_FrameProfiler->CurrentCumulativeTimingOf(s))
               << "GetTiming should be deterministic";
            EXPECT_EQ(m_FrameProfiler->IsKeyValid(s),
                      m_FrameProfiler->IsKeyValid(s))
               << "IsKeyValid should be deterministic";
         }
      }
   }
}

TEST_F(FrameProfilerTest, FrameTimeCorrectness) {}