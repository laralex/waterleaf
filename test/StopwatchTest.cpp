#include "wlf_core/Prelude.hpp"
#include "gtest/gtest.h"

#include <chrono>
#include <thread>


using namespace wlf;
using namespace wlf::utils;

template<typename T>
class StopwatchTest : public ::testing::Test {
public:
   void SetUp() override {
      if constexpr(std::is_same_v<T, Stopwatch>) {
         m_Stopwatch = std::make_optional<T>();
      } else if constexpr(std::is_same_v<T, RecordingStopwatch>) {
         usize nRecordsCapacity = 10;
         m_Stopwatch = std::make_optional<T>(nRecordsCapacity, Stopwatch{});
      }
   }

   void MeasureDummyWork(wlf::u64 sleepMs) {
      m_Stopwatch->SetBeginning();
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
      m_Stopwatch->SaveElapsed();
   }

   void MeasureFromTimepointTillNow(hires_timepoint timepoint) {
      m_Stopwatch->SetBeginning(timepoint);
      m_Stopwatch->SaveElapsed();
   }
   std::optional<T> m_Stopwatch;
};

using AllStopwatchTypes = ::testing::Types<Stopwatch, RecordingStopwatch>;
TYPED_TEST_SUITE(StopwatchTest, AllStopwatchTypes);


TYPED_TEST(StopwatchTest, RealisticResults) {
   u64 baseSleepMs = 3, sleepNoiseMs = 50, nRepeats = 3;
   for(u64 i = 1; i <= nRepeats; ++i) {
      u64 sleepMs = i * baseSleepMs;
      this->MeasureDummyWork(sleepMs);
      EXPECT_GE(this->m_Stopwatch->SavedElapsedMs(), sleepMs)
         << "Measurement can't be smaller than work time";
      EXPECT_LE(this->m_Stopwatch->SavedElapsedMs(), sleepMs + sleepNoiseMs)
         << "Measurement can't be much bigger than work time";
   }
}

TYPED_TEST(StopwatchTest, RealisticResultsBigTimespan) {
   u64 baseYears = 10, nRepeats = 5, sleepNoiseMs = 50;
   for(u64 i = 1; i < nRepeats; ++i) {
      u64 hoursInPast      = 365 * 24 * i * baseYears;
      auto timePointInPast = std::chrono::high_resolution_clock::now()
                             - std::chrono::hours(hoursInPast);
      u64 expectedElapsedMs = hoursInPast * 60 * 60 * 1000;
      this->MeasureFromTimepointTillNow(timePointInPast);
      EXPECT_GE(this->m_Stopwatch->SavedElapsedMs(), expectedElapsedMs)
         << "Measurement acn't be smaller than work time";
      EXPECT_LE(this->m_Stopwatch->SavedElapsedMs(),
                expectedElapsedMs + sleepNoiseMs)
         << "Measurement can't be much bigger than work time";
   }
}

TYPED_TEST(StopwatchTest, SetBeginningInFarPast) {
   u64 nRepeats = 5;
   for(u64 yearsInPast = 1; yearsInPast < nRepeats; ++yearsInPast) {
      u64 hoursInPast      = 365 * 24 * yearsInPast;
      auto timePointInPast = std::chrono::high_resolution_clock::now()
                             - std::chrono::hours(hoursInPast);
      this->m_Stopwatch->SetBeginning(timePointInPast);
      EXPECT_EQ(this->m_Stopwatch->Beginning(), timePointInPast)
         << "Beginning should change to what it's intended to";
   }
}


TYPED_TEST(StopwatchTest, SetBeginningInFuture) {
   u64 allowedNoiseUs = 100, nRepeats = 5, baseYears = 10;
   for(int i = 1; i <= nRepeats; ++i) {
      usize futureOffsetMs   = 2;
      auto timePointInFuture = std::chrono::high_resolution_clock::now()
                               + std::chrono::milliseconds(futureOffsetMs);
      this->m_Stopwatch->SetBeginning(timePointInFuture);
      EXPECT_LE(this->m_Stopwatch->Beginning(),
                std::chrono::high_resolution_clock::now())
         << "Near future: setting beginning should fallback to now()";
      this->m_Stopwatch->SaveElapsed();
      EXPECT_LE(this->m_Stopwatch->SavedElapsedUs(),
                static_cast<u64>(allowedNoiseUs))
         << "Near future: elapsed time should be about 0";
   }

   for(u64 i = 1; i <= nRepeats; ++i) {
      usize futureOffsetHours = 24 * 365 * i * baseYears;
      auto timePointInFuture  = std::chrono::high_resolution_clock::now()
                               + std::chrono::hours(futureOffsetHours);
      this->m_Stopwatch->SetBeginning(timePointInFuture);
      EXPECT_LE(this->m_Stopwatch->Beginning(),
                std::chrono::high_resolution_clock::now())
         << "Far future: setting beginning should fallback to now()"
         << " i = " << i;
      this->m_Stopwatch->SaveElapsed();
      EXPECT_LE(this->m_Stopwatch->SavedElapsedUs(),
                static_cast<u64>(allowedNoiseUs))
         << "Far future: elapsed time should be about 0."
         << " Thus elapsed should be about zero. i = " << i;
   }
}

TYPED_TEST(StopwatchTest, ElapsedTimingsIdentical) {
   u64 baseSleepMs = 3, nRepeats = 3;
   for(u64 i = 1; i <= nRepeats; ++i) {
      this->MeasureDummyWork(baseSleepMs * i);

      auto elapsed   = this->m_Stopwatch->SavedElapsed();
      auto elapsedMs = this->m_Stopwatch->SavedElapsedMs();
      auto elapsedUs = this->m_Stopwatch->SavedElapsedUs();

      auto convertedElapsedMs =
         std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
      EXPECT_EQ(static_cast<u64>(convertedElapsedMs.count()), elapsedMs)
         << "Converted elapsed duration to milliseconds should be identical";

      auto convertedElapsedUs =
         std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
      EXPECT_EQ(static_cast<u64>(convertedElapsedUs.count()), elapsedUs)
         << "Converted elapsed duration to microseconds should be identical";

      EXPECT_EQ(elapsedUs / 1000, elapsedMs)
         << "Converted microseconds duration to milliseconds should be identical";
   }
}

TYPED_TEST(StopwatchTest, ClearElapsed) {
   u64 nRepeats = 5, baseOffsetMs = 3;
   for(u64 i = 0; i < nRepeats; ++i) {
      auto timePoint = std::chrono::high_resolution_clock::now()
                       - std::chrono::milliseconds(i * baseOffsetMs);
      this->m_Stopwatch->SetBeginning(timePoint);
      this->m_Stopwatch->SaveElapsed();
      EXPECT_GE(this->m_Stopwatch->SavedElapsedMs(), i * baseOffsetMs)
         << "SaveElapsed is broken, unrealistic result";

      this->m_Stopwatch->ClearElapsed();
      EXPECT_EQ(this->m_Stopwatch->SavedElapsedUs(), 0)
         << "State should be cleared after call";
   }
}

TYPED_TEST(StopwatchTest, AddSaveElapsed) {
   u64 nRepeats = 5, baseOffsetMs = 1000, allowedNoiseMs = 3, nAdditions = 3;

   for(u64 i = 0; i < nRepeats; ++i) {
      auto timePoint = std::chrono::high_resolution_clock::now()
                       - std::chrono::milliseconds(i * baseOffsetMs);
      this->m_Stopwatch->SetBeginning(timePoint);
      u64 sumElapsedMs = 0;
      for(u64 addition = 0; addition < nAdditions; ++addition) {
         auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::high_resolution_clock::now()
                           - this->m_Stopwatch->Beginning())
                           .count();
         this->m_Stopwatch->AddSaveElapsed();
         sumElapsedMs += static_cast<u64>(elapsed);
         EXPECT_GE(this->m_Stopwatch->SavedElapsedMs(), sumElapsedMs)
            << "Elapsed time from beginning should accumulate."
            << " i=" << i << " addition=" << addition;
         EXPECT_LE(this->m_Stopwatch->SavedElapsedMs(),
                   sumElapsedMs + allowedNoiseMs)
            << "Cumulative elapsed time shouldn't be larger than work."
            << " i=" << i << " addition=" << addition;
      }
      this->m_Stopwatch->ClearElapsed();
   }
}

TYPED_TEST(StopwatchTest, SaveElapsedResetBeginning) {
   u64 nRepeats = 5, baseOffsetMs = 3;
   auto allowedNoiseDuration = std::chrono::milliseconds(3);
   for(u64 i = 0; i < nRepeats; ++i) {
      auto timePoint = std::chrono::high_resolution_clock::now()
                       - std::chrono::milliseconds(i * baseOffsetMs);
      this->m_Stopwatch->SetBeginning(timePoint);
      auto now = std::chrono::high_resolution_clock::now();
      this->m_Stopwatch->SaveElapsed(/*resetBeginning*/ true);

      EXPECT_GE(this->m_Stopwatch->SavedElapsedMs(), i * baseOffsetMs)
         << "Test is broken, SaveElapsed unrealistic result";

      EXPECT_GE(this->m_Stopwatch->Beginning(), now)
         << "Beginning time point should've been reset to now(). i=" << i;
      EXPECT_LE(this->m_Stopwatch->Beginning(), now + allowedNoiseDuration)
         << "Beginning time point should've been reset to now(). i=" << i;
   }
}
TYPED_TEST(StopwatchTest, AddSaveElapsedResetBeginning) {
   u64 nRepeats = 20, nAdditions = 5, baseOffsetMs = 3, allowedNoiseMs = 3;
   auto allowedNoiseDuration = std::chrono::milliseconds(allowedNoiseMs);
   for(u64 i = 0; i < nRepeats; ++i) {
      auto timePoint = std::chrono::high_resolution_clock::now()
                       - std::chrono::milliseconds(i * baseOffsetMs);
      this->m_Stopwatch->SetBeginning(timePoint);
      u64 sumElapsedMs = 0;
      for(u64 addition = 0; addition < nAdditions; ++addition) {
         auto now       = std::chrono::high_resolution_clock::now();
         auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                             now - this->m_Stopwatch->Beginning())
                             .count();
         this->m_Stopwatch->AddSaveElapsed(/*resetBeginning*/ true);
         sumElapsedMs += static_cast<u64>(elapsedMs);
         EXPECT_GE(this->m_Stopwatch->SavedElapsedMs(), sumElapsedMs)
            << "Test is broken, AddSaveElapsed unrealistic result. i=" << i;
         EXPECT_LE(this->m_Stopwatch->SavedElapsedMs(),
                   sumElapsedMs + allowedNoiseMs)
            << "Test is broken, AddSaveElapsed unrealistic result i=" << i;

         EXPECT_GE(this->m_Stopwatch->Beginning(), now)
            << "Beginning time point should've been reset to now(). i=" << i;
         EXPECT_LE(this->m_Stopwatch->Beginning(), now + allowedNoiseDuration)
            << "Beginning time point should've been reset to now(). i=" << i;
      }
      this->m_Stopwatch->ClearElapsed();
   }
}
