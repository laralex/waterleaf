#include "wlf_core/Prelude.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <thread>


using namespace wlf;
using namespace wlf::utils;

TEST(ProfilingStopwatchTest, YieldsMeaningfulResults) {
   Stopwatch s;
   usize allowedOverheadMs = 150;

   {
      usize sleepMs = 200;
      s.ResetBeginning();
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
      s.RecordElapsed();
      EXPECT_GE(s.LastElapsedMs(), sleepMs);
      EXPECT_LE(static_cast<f32>(s.LastElapsedMs()),
                static_cast<f32>(sleepMs + allowedOverheadMs));
   }

   {
      usize hoursInPast       = 2;
      usize expectedElapsedMs = hoursInPast * 60 * 60 * 1000;
      auto timePointInPast    = std::chrono::high_resolution_clock::now()
                             - std::chrono::hours(hoursInPast);

      s.ResetBeginning(timePointInPast);
      s.RecordElapsed();
      EXPECT_EQ(s.GetBeginningTimepoint(), timePointInPast);
      EXPECT_GE(s.LastElapsedMs(), expectedElapsedMs);
      EXPECT_LE(s.LastElapsedMs(), expectedElapsedMs + allowedOverheadMs);
   }
}

TEST(ProfilingStopwatchTest, ElapsedTimingsIdentical) {
   Stopwatch s;
   usize sleepMs = 5;
   s.ResetBeginning();
   std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
   s.RecordElapsed();

   auto elapsed   = s.LastElapsed();
   auto elapsedMs = s.LastElapsedMs(), elapsedUs = s.LastElapsedUs();

   EXPECT_EQ(elapsedUs / 1000, elapsedMs);

   auto convertedElapsedMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
   EXPECT_EQ(static_cast<u64>(convertedElapsedMs.count()), elapsedMs);

   auto convertedElapsedUs =
      std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
   EXPECT_EQ(static_cast<u64>(convertedElapsedUs.count()), elapsedUs);

   EXPECT_EQ(elapsed, s.LastElapsed());
   EXPECT_EQ(elapsedMs, s.LastElapsedMs());
   EXPECT_EQ(elapsedUs, s.LastElapsedUs());
}

TEST(ProfilingStopwatchTest, BeginningFromFuture) {
   Stopwatch s;
   {
      usize futureOffsetMs   = 2;
      auto timePointInFuture  = std::chrono::high_resolution_clock::now()
                              + std::chrono::milliseconds(futureOffsetMs);
      s.ResetBeginning(timePointInFuture);
      s.RecordElapsed();
      EXPECT_EQ(s.LastElapsedUs(), static_cast<u64>(0));
   }
   
   {
      usize futureOffsetHours   = 24*365*2000;
      auto timePointInFuture  = std::chrono::high_resolution_clock::now()
                              + std::chrono::hours(futureOffsetHours);
      s.ResetBeginning(timePointInFuture);
      s.RecordElapsed();
      EXPECT_EQ(s.LastElapsedUs(), static_cast<u64>(0));
   }

}
// TODO(laralex): test very far past time points