#include "wlf_core/Prelude.hpp"

#include "gtest/gtest.h"
#include <chrono>


using namespace wlf;
using namespace wlf::util;

class RecordingMultiStopwatchTest : public ::testing::Test {
protected:
   static void SetUpTestSuite() {
      usize baseOffsetMs = 100;
      for(usize i = 0; i < Names.size(); ++i) {
         Names[i] = std::to_string(i * 1000);
      }
      for(usize i = 0; i < NumStopwatchesVariants.size(); ++i) {
         auto innerBuilder = ConstructMultiStopwatch(NumStopwatchesVariants[i]);
         auto innerStopwatch =
            MultiStopwatch::FromBuilder(std::move(innerBuilder));
         StopwatchesVariants[i] = std::make_optional<RecordingMultiStopwatch>(
            N_RECORDS_CAPACITY, std::move(*innerStopwatch));
         auto builder = ConstructMultiStopwatch(NumStopwatchesVariants[i]);

         PrototypeStopwatchesVariants[i] =
            MultiStopwatch::FromBuilder(std::move(builder));

         auto now = std::chrono::high_resolution_clock::now();
         auto timePoint =
            now - std::chrono::milliseconds((i + 1) * baseOffsetMs);
         StopwatchesVariants[i]->SetBeginningOfAll(timePoint);
         PrototypeStopwatchesVariants[i]->SetBeginningOfAll(timePoint);
      }
   }

   static MultiStopwatchBuilder ConstructMultiStopwatch(usize nStopwatches) {
      auto builder = MultiStopwatchBuilder{nStopwatches};
      for(usize i = 0; i < nStopwatches; ++i) {
         builder.WithStopwatchName(i, std::string{Names[i]});
      }
      return builder;
   }

protected:
   static std::vector<usize> NumStopwatchesVariants;
   static std::vector<std::string> Names;
   static std::vector<std::optional<RecordingMultiStopwatch>>
      StopwatchesVariants;
   static std::vector<std::optional<MultiStopwatch>>
      PrototypeStopwatchesVariants;
   static constexpr usize N_RECORDS_CAPACITY = 100;
};

auto RecordingMultiStopwatchTest::NumStopwatchesVariants =
   std::vector<usize>{0, 1, 16, 256};
auto RecordingMultiStopwatchTest::Names = std::vector<std::string>(1000);
auto RecordingMultiStopwatchTest::StopwatchesVariants =
   std::vector<std::optional<RecordingMultiStopwatch>>(
      NumStopwatchesVariants.size());
auto RecordingMultiStopwatchTest::PrototypeStopwatchesVariants =
   std::vector<std::optional<MultiStopwatch>>(NumStopwatchesVariants.size());

TEST_F(RecordingMultiStopwatchTest, InnerStopwatch) {
   u64 nRepeats = 5;

   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      for(usize variant = 0; variant < StopwatchesVariants.size(); ++variant) {
         auto nStopwatches = StopwatchesVariants[variant]->StopwatchesNumber();
         EXPECT_EQ(PrototypeStopwatchesVariants[variant]->StopwatchesNumber(),
                   nStopwatches)
            << "Inner stopwatch should have same number of stopwatches";
         for(usize key = 0; key < nStopwatches; ++key) {
            EXPECT_EQ(PrototypeStopwatchesVariants[variant]->BeginningOf(key),
                      StopwatchesVariants[variant]->BeginningOf(key))
               << "Inner stopwatch should have same beginnings";

            EXPECT_EQ(PrototypeStopwatchesVariants[variant]->NameOf(key),
                      StopwatchesVariants[variant]->NameOf(key))
               << "Inner stopwatch should have same names";
         }
      }
   }
}

TEST_F(RecordingMultiStopwatchTest, ReadingRecords) {
   u64 nRepeats = 20, baseOffsetMs = 100;
   usize nRecordsCapacity = 50;

   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      RecordingStopwatch stopwatch{nRecordsCapacity, Stopwatch{}};

      std::vector<u64> manualRecords(nRecordsCapacity * 2);
      for(usize recording = 0; recording < nRecordsCapacity * 2; ++recording) {
         auto now = std::chrono::high_resolution_clock::now();
         auto offsetMs =
            std::chrono::milliseconds((repeat + recording + 1) * baseOffsetMs);
         auto timePoint = now - offsetMs;
         stopwatch.SetBeginning(timePoint);
         stopwatch.StoreElapsed();
         stopwatch.RecordState();
         manualRecords[recording] = stopwatch.GetElapsedUs();
         EXPECT_NE(manualRecords[recording], 0) << "Test is broken. No timing";
      }

      for(usize recordingOffset = 0; recordingOffset < nRecordsCapacity;
          ++recordingOffset) {
         EXPECT_TRUE(stopwatch.IsRecordAvailable(recordingOffset))
            << "Record should be accessible. recordingOffset="
            << recordingOffset;
         EXPECT_TRUE(stopwatch.RecordedElapsedUs(recordingOffset))
            << "Record should be accessible. recordingOffset="
            << recordingOffset;
         EXPECT_EQ(stopwatch.RecordedElapsedUs(recordingOffset),
                   manualRecords[manualRecords.size() - recordingOffset - 1])
            << "Record shoulbe be the same as of saving"
            << " recordingOffset=" << recordingOffset;
      }

      for(usize recordingOffset = nRecordsCapacity;
          recordingOffset < nRecordsCapacity * 2; ++recordingOffset) {
         EXPECT_FALSE(stopwatch.IsRecordAvailable(recordingOffset))
            << "Record should be inaccessible after buffer overflow. "
            << "recordingOffset=" << recordingOffset;
         EXPECT_FALSE(stopwatch.RecordedElapsedUs(recordingOffset))
            << "Record should be inaccessible after buffer overflow. "
            << "recordingOffset=" << recordingOffset;
      }
   }
}


TEST_F(RecordingMultiStopwatchTest, ClearRecords) {
   u64 nRepeats = 20, baseOffsetMs = 100;
   usize nRecordsCapacity = 100;
   RecordingStopwatch stopwatch{nRecordsCapacity, Stopwatch{}};
   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      auto capacity = stopwatch.RecordsCapacity();
      for(usize recording = 0; recording < capacity; ++recording) {
         auto now       = std::chrono::high_resolution_clock::now();
         auto offsetMs  = std::chrono::milliseconds(baseOffsetMs * (repeat + 1)
                                                   * (recording + 1));
         auto timePoint = now - offsetMs;
         stopwatch.SetBeginning(timePoint);
         stopwatch.StoreElapsed();
         EXPECT_NE(stopwatch.GetElapsedUs(), 0)
            << "Test is broken. No timing";
         stopwatch.RecordState();
      }
      for(usize recordingOffset = 0; recordingOffset < nRecordsCapacity;
          ++recordingOffset) {
         EXPECT_TRUE(stopwatch.RecordedElapsedUs(recordingOffset))
            << "Test is broken: record should be accessible. recordingOffset="
            << recordingOffset;
      }

      stopwatch.ClearRecords();

      for(usize recordingOffset = 0; recordingOffset < nRecordsCapacity * 2;
          ++recordingOffset) {
         EXPECT_FALSE(stopwatch.RecordedElapsedUs(recordingOffset))
            << "After clearing no records should be accesible"
            << ". recordingOffset=" << recordingOffset;
      }
   }
}

TEST_F(RecordingMultiStopwatchTest, RecordsCapacity) {
   std::vector<usize> capacities = {0, 1, 2, 16, 10000};
   u64 nRepeats                  = 5;
   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      for(usize capacity : capacities) {
         RecordingStopwatch stopwatch{capacity, Stopwatch{}};
         EXPECT_EQ(stopwatch.RecordsCapacity(), capacity);
      }
   }
}