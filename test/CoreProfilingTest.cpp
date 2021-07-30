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

static usize DummyWork(usize loopSize) {
   usize dummyCount = 0;
   for(usize i = 0; i < loopSize; ++i) {
      ++dummyCount;
   }
   return dummyCount;
};

constexpr auto ProfileInMillisecs =
   &utils::ProfileInMillisecs<decltype(DummyWork)&&, usize&&>;

constexpr auto ProfileInMicrosecs =
   &utils::ProfileInMicrosecs<decltype(DummyWork)&&, usize&&>;

struct Statistics {
   f32 mean, std, median;
};

Statistics CalculateStatistics(const std::vector<f32>& sortedSample) {
   usize size = sortedSample.size();
   Statistics resultStats;
   if(size % 2 == 0) {
      resultStats.median =
         (sortedSample[size / 2] + sortedSample[size / 2 - 1]) / 2;
   } else {
      resultStats.median = sortedSample[size / 2];
   }
   resultStats.mean = std::transform_reduce(
      sortedSample.begin(), sortedSample.end(), 0.0f, std::plus{},
      [size](auto elem) { return elem / static_cast<f32>(size); });
   f32 sumMeanDiffs =
      std::transform_reduce(sortedSample.begin(), sortedSample.end(), 0.0f,
                            std::plus{}, [&](auto elem) {
                               auto diff =
                                  static_cast<f32>(elem) - resultStats.mean;
                               return diff * diff;
                            });
   resultStats.std = std::sqrtf(sumMeanDiffs / (static_cast<f32>(size) - 1.0f));
   return resultStats;
}

class ProfilingFunctionTest : public ::testing::Test {
   std::vector<f32> mRunsTimings;

protected:
   void WarmUp(usize nWarmUps, usize workLoopSize) {
      for(auto run = 0; run < nWarmUps; ++run) {
         auto result = DummyWork(workLoopSize);
         EXPECT_EQ(result, workLoopSize);
      }
   }

   template<typename ProfilingF>
   Statistics ProfileSameJobSize(usize nRuns,
                                 usize workLoopSize,
                                 ProfilingF profilingFunc) {
      mRunsTimings.clear();
      for(auto run = 0; run < nRuns; ++run) {
         auto [result, timing] =
            profilingFunc(DummyWork, std::forward<usize>(workLoopSize));
         EXPECT_EQ(result, workLoopSize);
         mRunsTimings.push_back(static_cast<f32>(timing));
      }
      return CalculateStatistics(mRunsTimings);
   }

   template<typename ProfilingF>
   Statistics ProfileIncreasingJobSize(usize nRuns,
                                       usize baseLoopSize,
                                       ProfilingF profilingFunc) {
      mRunsTimings.clear();
      for(usize runIdx = 1; runIdx <= nRuns; ++runIdx) {
         auto [result, timing] =
            profilingFunc(DummyWork, runIdx * baseLoopSize);
         EXPECT_EQ(result, runIdx * baseLoopSize);
         mRunsTimings.push_back(static_cast<f32>(timing / runIdx));
      }
      return CalculateStatistics(mRunsTimings);
   }
};

} // namespace

TEST_F(ProfilingFunctionTest, WallTimeIndependentOverhead) {
   const usize nRuns = 1000, workLoopSize = 5000, nWarmUps = 250;
   {
      WarmUp(nWarmUps, workLoopSize);
      auto stats = ProfileSameJobSize(nRuns, workLoopSize, ProfileInMillisecs);
      EXPECT_LE(std::abs(stats.mean - stats.median),
                0.001f * ACCEPTED_PROFILING_INCONSISTENCY_US);
   }

   {
      WarmUp(nWarmUps, workLoopSize);
      auto stats = ProfileSameJobSize(nRuns, workLoopSize, ProfileInMicrosecs);
      EXPECT_LE(std::abs(stats.mean - stats.median),
                ACCEPTED_PROFILING_INCONSISTENCY_US);
   }
}

TEST_F(ProfilingFunctionTest, JobSizeIndependentOverhead) {
   const usize nRuns = 500, baseLoopSize = 250;

   {
      WarmUp(1, baseLoopSize * nRuns);
      auto stats =
         ProfileIncreasingJobSize(nRuns, baseLoopSize, ProfileInMillisecs);
      EXPECT_LE(stats.std, 0.001f * ACCEPTED_PROFILING_INCONSISTENCY_US);
   }

   {
      WarmUp(1, baseLoopSize * nRuns);
      auto stats =
         ProfileIncreasingJobSize(nRuns, baseLoopSize, ProfileInMicrosecs);
      EXPECT_LE(stats.std, ACCEPTED_PROFILING_INCONSISTENCY_US);
   }
}

TEST_F(ProfilingFunctionTest, OutputPassedThrough) {
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

TEST_F(ProfilingFunctionTest, ArbitraryInputsAccepted) {
   {
      auto DummyFunc = [](f64 a, f64 b, f64 c, f64 d, f64 e, f32 f, f64 g) {
         return (a + b + c + d + e + static_cast<f64>(f) * g);
      };
      auto [result, timeMs] =
         utils::ProfileInMillisecs(DummyFunc, 1., 2., 3., 4., 5., 6.f, 2.);
      EXPECT_EQ(result, DummyFunc(1., 2., 3., 4., 5., 6.f, 2.));
   }

   {
      auto DummyFunc = [](i32 a, const std::vector<f32>& v, f32 b, f64 c) {
         return (static_cast<usize>(a) * static_cast<usize>(b))
                 + static_cast<usize>(c) + v.size();
      };
      auto [result, timeMs] = utils::ProfileInMicrosecs(
         DummyFunc, 1, std::vector{2.0f, 42.0f}, 3.0f, 5.0);
      EXPECT_EQ(result, DummyFunc(1, {2.0f, 42.0f}, 3.0f, 5.0));
   }
}