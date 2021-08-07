#include "wlf_core/Prelude.hpp"

#include <chrono>
#include <gtest/gtest.h>

using namespace wlf;
using namespace wlf::utils;

TEST(RecordingStopwatchTest, InnerStopwatch) {
   u64 nRepeats = 5, baseOffsetMs = 100, nRecordsCapacity = 10;

   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      auto now       = std::chrono::high_resolution_clock::now();
      auto timePoint = now - std::chrono::milliseconds(repeat * baseOffsetMs);
      Stopwatch innerStopwatch;
      innerStopwatch.SetBeginning(timePoint);
      Stopwatch cloneStopwatch{innerStopwatch};
      RecordingStopwatch stopwatch{nRecordsCapacity, std::move(cloneStopwatch)};
      EXPECT_EQ(innerStopwatch.Beginning(),
                stopwatch.InnerStopwatch().Beginning())
         << "Inner stopwatch should be what was given upon construction";
   }
}

TEST(RecordingStopwatchTest, ReadingRecords) {
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
         stopwatch.InnerStopwatch().SetBeginning(timePoint);
         stopwatch.InnerStopwatch().SaveElapsed();
         stopwatch.RecordState();
         manualRecords[recording] = stopwatch.InnerStopwatch().SavedElapsedUs();
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
            << "Record should match elapsed microseconds at the moment of saving."
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


TEST(RecordingStopwatchTest, ClearRecords) {
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
         stopwatch.InnerStopwatch().SetBeginning(timePoint);
         stopwatch.InnerStopwatch().SaveElapsed();
         EXPECT_NE(stopwatch.InnerStopwatch().SavedElapsedUs(), 0)
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

TEST(RecordingStopwatchTest, RecordsCapacity) {
   std::vector<usize> capacities = {0, 1, 2, 16, 10000};
   u64 nRepeats                  = 5;
   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      for(usize capacity : capacities) {
         RecordingStopwatch stopwatch{capacity, Stopwatch{}};
         EXPECT_EQ(stopwatch.RecordsCapacity(), capacity);
      }
   }
}
/* 
TEST(RecordingStopwatchTest, UnassignableInternalStopwatch) {
   u64 nRepeats           = 20;
   usize nRecordsCapacity = 100;
   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      RecordingStopwatch stopwatch{nRecordsCapacity, Stopwatch{}};

      auto now         = std::chrono::high_resolution_clock::now();
      auto offsetHours = std::chrono::hours(1000);
      stopwatch.InnerStopwatch().SetBeginning(now - offsetHours);

      stopwatch.InnerStopwatch() = Stopwatch{};
   }
} */