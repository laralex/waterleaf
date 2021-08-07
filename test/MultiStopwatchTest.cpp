#include "wlf_core/Profiling.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <vector>


using namespace wlf;
using namespace wlf::utils;

class MultiStopwatchTest : public ::testing::Test {
protected:
   static void SetUpTestSuite() {
      for(usize i = 0; i < Names.size(); ++i) {
         Names[i] = std::to_string(i*1000);
      }
      for(usize i = 0; i < NumStopwatchesVariants.size(); ++i) {
         StopwatchesVariants[i] =
            ConstructMultiStopwatch(NumStopwatchesVariants[i]);
      }
   }

   static std::optional<MultiStopwatch>
   ConstructMultiStopwatch(usize nStopwatches) {
      auto builder = MultiStopwatchBuilder{nStopwatches};
      for(usize i = 0; i < nStopwatches; ++i) {
         builder.WithStopwatchName(i, std::string{Names[i % nStopwatches]});
      }
      return MultiStopwatch::FromBuilder(std::move(builder));
   }

protected:
   static std::vector<usize> NumStopwatchesVariants;
   static std::vector<std::string> Names;
   static std::vector<std::optional<MultiStopwatch>> StopwatchesVariants;
};

std::vector<usize> MultiStopwatchTest::NumStopwatchesVariants = {1, 16, 256};
std::vector<std::string> MultiStopwatchTest::Names =
   std::vector<std::string>(1000);
std::vector<std::optional<MultiStopwatch>>
   MultiStopwatchTest::StopwatchesVariants =
      std::vector<std::optional<MultiStopwatch>>(NumStopwatchesVariants.size());

TEST_F(MultiStopwatchTest, BuilderCompleteness) {
   for(auto nStopwatches : NumStopwatchesVariants) {
      auto builder = MultiStopwatchBuilder{nStopwatches};
      for(usize i = 0; i < nStopwatches * 2; i += 2) {
         EXPECT_FALSE(builder.IsComplete())
            << "Builder shouldn't be complete while not all stopwatches named."
            << " nStopwatches=" << nStopwatches << " i=" << i;
         builder.WithStopwatchName(i, std::string{Names[i % nStopwatches]});
      }
      if(nStopwatches > 1) {
         usize oddIndex = nStopwatches * 2 + 1;
         for(usize i = oddIndex; i > 1; i -= 2) {
            EXPECT_FALSE(builder.IsComplete())
               << "Builder shouldn't be complete while not all stopwatches named."
               << " nStopwatches=" << nStopwatches << " i=" << i;
            builder.WithStopwatchName(i, std::string{Names[i % nStopwatches]});
         }
      }
      builder.WithStopwatchName(1, std::string{Names[1]});
      EXPECT_TRUE(builder.IsComplete()) << "Builder should be complete";

      for(usize i = nStopwatches + 100; i < nStopwatches + 150; ++i) {
         builder.WithStopwatchName(i, std::string{Names[i % nStopwatches]});
         EXPECT_TRUE(builder.IsComplete())
            << "Builder should be complete still";
      }

      auto multiStopwatchOpt = MultiStopwatch::FromBuilder(std::move(builder));
      EXPECT_TRUE(multiStopwatchOpt);
   }
}

TEST_F(MultiStopwatchTest, BuilderWithoutStopwatches) {
   auto builder = MultiStopwatchBuilder{0};
   EXPECT_TRUE(builder.IsComplete())
      << "No stopwatches to initialize, builder should be complete";

   for(usize i = 0; i < 100; ++i) {
      builder.WithStopwatchName(i, "ZZZ");
      EXPECT_TRUE(builder.IsComplete()) << "Builder should be complete still";
   }

   auto multistopwatch = MultiStopwatch::FromBuilder(std::move(builder));
   EXPECT_TRUE(multistopwatch)
      << "Making from a complete builder should succeed";
}

TEST_F(MultiStopwatchTest, IncompleteBuilder) {
   usize nStopwatches = 20;
   for(usize upTo = 0; upTo < nStopwatches - 1; ++upTo) {
      auto builder = MultiStopwatchBuilder{nStopwatches};
      for(usize i = 0; i < upTo; ++i) {
         builder.WithStopwatchName(i, std::string{Names[i % nStopwatches]});
      }
      EXPECT_FALSE(builder.IsComplete())
         << "Not all stopwatches are initialized, builder should be incomplete";
      EXPECT_FALSE(MultiStopwatch::FromBuilder(std::move(builder)))
         << "Making from an incomplete builder should fail";
   }
}

TEST_F(MultiStopwatchTest, StopwatchesNumber) {
   for(usize i = 0; i < StopwatchesVariants.size(); ++i) {
      EXPECT_EQ(StopwatchesVariants[i]->StopwatchesNumber(),
                NumStopwatchesVariants[i])
         << "Stopwatches number should match the builder's stopwatches number";
   }
}

TEST_F(MultiStopwatchTest, IsKeyValid) {
   for(usize i = 0; i < StopwatchesVariants.size(); ++i) {
      usize nStopwatches = NumStopwatchesVariants[i];
      for(usize key = 0; key < nStopwatches; ++key) {
         EXPECT_TRUE(StopwatchesVariants[i]->IsKeyValid(key))
            << "Key less than StopwatchesNumber should be valid";
      }
      for(usize key = nStopwatches; key < nStopwatches + 100; ++key) {
         EXPECT_FALSE(StopwatchesVariants[i]->IsKeyValid(key))
            << "Key greater-equal than StopwatchesNumber should be invalid";
      }
   }
}

TEST_F(MultiStopwatchTest, NameOf) {
   for(usize i = 0; i < StopwatchesVariants.size(); ++i) {
      usize nStopwatches = NumStopwatchesVariants[i];
      for(usize key = 0; key < nStopwatches; ++key) {
         EXPECT_TRUE(StopwatchesVariants[i]->NameOf(key));
         EXPECT_EQ(StopwatchesVariants[i]->NameOf(key), Names[i])
            << "Name of stopwatch should match what was specified by builder";
      }
   }
}

TEST_F(MultiStopwatchTest, StopwatchesIndependence) {}