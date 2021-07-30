#include "wlf_core/Prelude.hpp"

#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>
#include <numeric>
#include <spdlog/spdlog.h>
#include <vector>



using namespace wlf;

namespace {

const f64 ACCEPTED_PROFILING_INCONSISTENCY_US = 5.0f;

usize DummyWork(usize loopSize) {
   usize dummyCount = 0;
   for(usize i = 0; i < loopSize; ++i) {
      ++dummyCount;
   }
   return dummyCount;
};

struct Statistics {
   f32 mean, std, median;
};

Statistics CalculateStatistics(std::vector<f32>&& sample) {
   std::sort(sample.begin(), sample.end());
   f32 sampleSize = static_cast<f32>(sample.size());

   Statistics resultStats;
   if(sample.size() % 2 == 0) {
      resultStats.median =
         (sample[sample.size() / 2] + sample[sample.size() / 2 - 1]) / 2;
   } else {
      resultStats.median = sample[sample.size() / 2];
   }
   resultStats.mean = std::transform_reduce(
      sample.begin(), sample.end(), 0.0f, std::plus{},
      [sampleSize](auto elem) { return elem / sampleSize; });
   f32 sumMeanDiffs = std::transform_reduce(
      sample.begin(), sample.end(), 0.0f, std::plus{}, [&](auto elem) {
         auto diff = static_cast<f32>(elem) - resultStats.mean;
         return diff * diff;
      });
   resultStats.std = std::sqrtf(sumMeanDiffs / (sampleSize - 1.0f));
   return resultStats;
}

} // namespace

GTEST_TEST(CoreProfiling, ProfileSameFunction_OverheadConsistency) {
   const usize nRuns = 1000, workLoopSize = 5000, nWarmUps = 250;

   {
      std::vector<f32> runsTimeMs;

      // warm up
      for(auto run = 0; run < nWarmUps; ++run) {
         auto [result, timeMs] =
            utils::ProfileInMillisecs(DummyWork, workLoopSize);
         EXPECT_EQ(result, workLoopSize);
      }

      for(auto run = 0; run < nRuns; ++run) {
         auto [result, timeMs] =
            utils::ProfileInMillisecs(DummyWork, workLoopSize);
         EXPECT_EQ(result, workLoopSize);
         runsTimeMs.push_back(static_cast<f32>(timeMs));
      }
      auto stats = CalculateStatistics(std::move(runsTimeMs));
      EXPECT_LE(std::abs(stats.mean - stats.median),
                0.001f * ACCEPTED_PROFILING_INCONSISTENCY_US);
   }

   {
      std::vector<f32> runsTimeUs;

      // warm up
      for(auto run = 0; run < nWarmUps; ++run) {
         auto [result, timeMs] =
            utils::ProfileInMicrosecs(DummyWork, workLoopSize);
         EXPECT_EQ(result, workLoopSize);
      }

      for(auto run = 0; run < nRuns; ++run) {
         auto [result, timeUs] =
            utils::ProfileInMicrosecs(DummyWork, workLoopSize);
         EXPECT_EQ(result, workLoopSize);
         runsTimeUs.push_back(static_cast<f32>(timeUs));
      }
      auto stats = CalculateStatistics(std::move(runsTimeUs));
      EXPECT_LE(std::abs(stats.mean - stats.median),
                ACCEPTED_PROFILING_INCONSISTENCY_US);
   }
}

GTEST_TEST(CoreProfiling, ProfileDifferentFunctions_OverheadConsistency) {
   const usize nRuns = 1000, baseLoopSize = 250, nWarmUps = 100;

   // warm up
   for(usize runIdx = 0; runIdx < nWarmUps; ++runIdx) {
      auto [result, timeMs] =
         utils::ProfileInMillisecs(DummyWork, runIdx * baseLoopSize);
      EXPECT_EQ(result, runIdx * baseLoopSize);
   }

   {
      std::vector<f32> overheadTimesMs;
      utils::ProfileInMillisecs(DummyWork, nRuns * baseLoopSize); // warm up
      for(usize runIdx = 1; runIdx <= nRuns; ++runIdx) {
         auto [result, profiledTimeMs] =
            utils::ProfileInMillisecs(DummyWork, runIdx * baseLoopSize);
         EXPECT_EQ(result, runIdx * baseLoopSize);
         overheadTimesMs.push_back(static_cast<f32>(profiledTimeMs / runIdx));
      }
      auto stats = CalculateStatistics(std::move(overheadTimesMs));
      EXPECT_LE(stats.mean, 0.001f * ACCEPTED_PROFILING_INCONSISTENCY_US);
   }

   {
      std::vector<f32> overheadTimesUs;
      utils::ProfileInMicrosecs(DummyWork, nRuns * baseLoopSize); // warm up
      for(usize runIdx = 1; runIdx <= nRuns; ++runIdx) {
         auto [result, profiledTimeUs] =
            utils::ProfileInMicrosecs(DummyWork, runIdx * baseLoopSize);
         EXPECT_EQ(result, runIdx * baseLoopSize);
         overheadTimesUs.push_back(static_cast<f32>(profiledTimeUs / runIdx));
      }
      auto stats = CalculateStatistics(std::move(overheadTimesUs));
      EXPECT_LE(stats.std, ACCEPTED_PROFILING_INCONSISTENCY_US);
   }
}

GTEST_TEST(CoreProfiling, ProfileFunction_OutputPassedThrough) {
   struct Result {
      f32 a;
      u8 b;
      Result(f32 a, u8 b) : a(a), b(b) {}
   };

   {
      const auto DummyFunc  = []() { return Result(42.0f, 123); };
      auto [result, timeMs] = utils::ProfileInMillisecs(DummyFunc);
      EXPECT_NEAR(result.a, 42.0f, 1e-5);
      EXPECT_EQ(result.b, 123);
   }

   {
      const auto DummyFunc  = []() { return Result(36.5f, 42); };
      auto [result, timeMs] = utils::ProfileInMicrosecs(DummyFunc);
      EXPECT_NEAR(result.a, 36.5f, 1e-5);
      EXPECT_EQ(result.b, 42);
   }
}

GTEST_TEST(CoreProfiling, ProfileFunction_ArbitraryInputsAccepted) {
   {
      auto DummyFunc        = [](i32 a, i32 b, i32 c) { return (a + b + c); };
      auto [result, timeMs] = utils::ProfileInMillisecs(DummyFunc, 1, 2, 3);
      EXPECT_EQ(result, DummyFunc(1, 2, 3));
   }

   {
      auto DummyFunc = [](f64 a, f64 b, f64 c, f64 d) {
         return (a + b + c + d);
      };
      auto [result, timeMs] =
         utils::ProfileInMicrosecs(DummyFunc, 1.0, 2.0, 3.0, 4.0);
      EXPECT_EQ(result, DummyFunc(1.0, 2.0, 3.0, 4.0));
   }

   {
      auto DummyFunc = [](f64 a, f64 b, f64 c, f64 d, f64 e) {
         return (a + b + c + d + e);
      };
      auto [result, timeMs] =
         utils::ProfileInMillisecs(DummyFunc, 1.0, 2.0, 3.0, 4.0, 5.0);
      EXPECT_EQ(result, DummyFunc(1.0, 2.0, 3.0, 4.0, 5.0));
   }

   {
      auto DummyFunc = [](i32 a, const std::vector<f32>& v) {
         return (static_cast<usize>(a) + v.size());
      };
      auto [result, timeMs] =
         utils::ProfileInMicrosecs(DummyFunc, 1, std::vector{2.0f, 42.0f});
      EXPECT_EQ(result, DummyFunc(1, {2.0f, 42.0f}));
   }
}