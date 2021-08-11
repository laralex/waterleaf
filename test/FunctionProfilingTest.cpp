#include "wlf_core/Prelude.hpp"

#include "gtest/gtest.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <thread>
#include <vector>

using namespace wlf;

namespace {

static usize DummyWork(usize loopSize) {
   usize dummyCount = 0;
   for(usize i = 0; i < loopSize; ++i) {
      ++dummyCount;
   }
   return dummyCount;
};

struct Statistics {
   f32 mean, std, median, min, max;
};

void RemoveSampleOutliers(std::vector<f32>& sortedSample,
                          f32 magnitudeThreshold) {
   auto newEndIdx = sortedSample.size();
   while(sortedSample[newEndIdx - 1] >= magnitudeThreshold) {
      --newEndIdx;
   }
   sortedSample.resize(newEndIdx);
}

f32 CalculateMedian(const std::vector<f32>& sortedSample) {
   usize size = sortedSample.size();
   if(size % 2 == 0) {
      return (sortedSample[size / 2] + sortedSample[size / 2 - 1]) * 0.5f;
   } else {
      return sortedSample[size / 2];
   }
}

f32 CalculateMean(const std::vector<f32>& sample) {
   return std::transform_reduce(
      sample.begin(), sample.end(), 0.0f, std::plus{},
      [&](auto elem) { return elem / static_cast<f32>(sample.size()); });
}

f32 CalculateStd(const std::vector<f32>& sample, f32 mean) {
   f32 sumMeanDiffs = std::transform_reduce(
      sample.begin(), sample.end(), 0.0f, std::plus{}, [mean](auto elem) {
         auto diff = static_cast<f32>(elem) - mean;
         return diff * diff;
      });
   return std::sqrtf(sumMeanDiffs / (static_cast<f32>(sample.size()) - 1.0f));
}

Statistics CalculateStatistics(std::vector<f32>& sample) {
   // remove outliers
   std::sort(sample.begin(), sample.end());
   f32 magnitudeThreshold = sample[0] * 2.0f;
   RemoveSampleOutliers(sample, magnitudeThreshold);

   // calculate pure statistics
   Statistics resultStats;
   resultStats.min    = sample[0];
   resultStats.max    = sample[sample.size() - 1];
   resultStats.median = CalculateMedian(sample);
   resultStats.mean   = CalculateMean(sample);
   resultStats.std    = CalculateStd(sample, resultStats.mean);
   // spdlog::info("mean {} median {} std {} min {} max {}", resultStats.mean,
   //              resultStats.median, resultStats.std, resultStats.min,
   //              resultStats.max);
   return resultStats;
}

class FunctionProfilingTest : public ::testing::Test {
protected:
   std::vector<f32> mRunsTimings;

   void WarmUp(usize nWarmUps, usize workLoopSize) {
      for(auto run = 0; run < nWarmUps; ++run) {
         SCOPED_TRACE("WarmUp");
         auto result = DummyWork(workLoopSize);
         EXPECT_EQ(result, workLoopSize);
      }
   }
};

} // namespace

TEST_F(FunctionProfilingTest, OutputPassedThrough) {
   struct Result {
      f32 a;
      u8 b;
      Result(f32 a, u8 b) : a(a), b(b) {}
   };

   {
      const auto DummyFunc = []() { return Result(42.0f, 123); };
      auto [result, timeMs] =
         util::ProfileInvoke<std::chrono::milliseconds>(DummyFunc);
      EXPECT_NEAR(result.a, 42.0f, 1e-5);
      EXPECT_EQ(result.b, 123);
   }

   {
      const auto DummyFunc = []() { return Result(36.5f, 42); };
      auto [result, timeMs] =
         util::ProfileInvoke<std::chrono::milliseconds>(DummyFunc);
      EXPECT_NEAR(result.a, 36.5f, 1e-5);
      EXPECT_EQ(result.b, 42);
   }

   {
      auto DummyFunc = []() {};
      auto timeMs = util::ProfileInvoke<std::chrono::microseconds>(DummyFunc);
      bool returnedOnlyTiming = std::is_same_v<long long, decltype(timeMs)>;
      EXPECT_TRUE(returnedOnlyTiming);
   }
}

TEST_F(FunctionProfilingTest, ResultDiscarding) {
   struct Result {
      f32 a;
      u8 b;
      Result(f32 a, u8 b) : a(a), b(b) {}
   };

   {
      const auto DummyFunc = []() { return Result(42.0f, 123); };
      auto timeMs =
         util::ProfileInvokeDiscardResult<std::chrono::milliseconds>(DummyFunc);
      bool isTimestampType = std::is_same_v<long long, decltype(timeMs)>;
      EXPECT_TRUE(isTimestampType);
   }

   {
      const auto DummyFunc = []() { return Result(36.5f, 42); };
      auto timeMs =
         util::ProfileInvokeDiscardResult<std::chrono::milliseconds>(DummyFunc);
      bool isTimestampType = std::is_same_v<long long, decltype(timeMs)>;
      EXPECT_TRUE(isTimestampType);
   }

   {
      auto DummyFunc = []() {};
      auto timeMs =
         util::ProfileInvokeDiscardResult<std::chrono::microseconds>(DummyFunc);
      bool isTimestampType = std::is_same_v<long long, decltype(timeMs)>;
      EXPECT_TRUE(isTimestampType);
   }
}

TEST_F(FunctionProfilingTest, ArbitraryInputsAccepted) {
   {
      auto DummyFunc = [](f64 a, f64 b, f64 c, f64 d, f64 e, f32 f, f64 g) {
         return (a + b + c + d + e + static_cast<f64>(f) * g);
      };
      auto [result, timeMs] = util::ProfileInvoke<std::chrono::milliseconds>(
         DummyFunc, 1., 2., 3., 4., 5., 6.f, 2.);
      EXPECT_FLOAT_EQ(result, DummyFunc(1., 2., 3., 4., 5., 6.f, 2.));
   }

   {
      auto DummyFunc = [](i32 a, const std::vector<f32>& v, f32 b, f64 c) {
         return (static_cast<usize>(a) * static_cast<usize>(b))
                + static_cast<usize>(c) + v.size();
      };
      auto [result, timeMs] = util::ProfileInvoke<std::chrono::milliseconds>(
         DummyFunc, 1, std::vector{2.0f, 42.0f}, 3.0f, 5.0);
      EXPECT_EQ(result, DummyFunc(1, {2.0f, 42.0f}, 3.0f, 5.0));
   }

   {
      auto DummyFunc = []() { return 5; };
      auto [result, timeMs] =
         util::ProfileInvoke<std::chrono::milliseconds>(DummyFunc);
      EXPECT_EQ(result, DummyFunc());
   }
}

TEST_F(FunctionProfilingTest, CorrectTimeReturned) {
   usize nRuns = 5, baseSleepTimeMs = 100;
   f32 acceptedSleepNoiseMs = 15.0f;
   auto SleepFunc           = [](usize sleepMillisecs) {
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepMillisecs));
   };

   {
      WarmUp(1, 100000);
      mRunsTimings.clear();
      for(usize runIdx = 1; runIdx <= nRuns; ++runIdx) {
         SCOPED_TRACE(runIdx);
         auto timeUs = util::ProfileInvoke<std::chrono::microseconds>(
            SleepFunc, baseSleepTimeMs * runIdx);
         EXPECT_GE(timeUs, 1000 * baseSleepTimeMs * runIdx);
         mRunsTimings.push_back(static_cast<f32>(timeUs)
                                / static_cast<f32>(runIdx));
      }
      auto statsUs = CalculateStatistics(mRunsTimings);
      EXPECT_LE(statsUs.max - statsUs.min, 1000.f * acceptedSleepNoiseMs);
   }
}