#include "wlf_core/Profiling.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <thread>


using namespace wlf;
using namespace wlf::utils;

TEST(ProfilingMultiStopwatch, BuilderCorrectness) {
   {
      usize nStopwatches = 3;
      auto builder       = MultiStopwatch::CreateBuilder(nStopwatches)
                        .WithStopwatchName(0, "AAA")
                        .WithStopwatchName(2, "CCC");
      EXPECT_FALSE(builder.IsComplete());

      builder.WithStopwatchName(10, "ZZZ");
      EXPECT_FALSE(builder.IsComplete());

      builder.WithStopwatchName(1, "BBB");
      EXPECT_TRUE(builder.IsComplete());

      builder.WithStopwatchName(0, "DDD");
      EXPECT_TRUE(builder.IsComplete());

      builder.WithStopwatchName(10, "ZZZ");
      EXPECT_TRUE(builder.IsComplete());

      auto multistopwatch = MultiStopwatch::FromBuilder(std::move(builder));
      EXPECT_TRUE(multistopwatch);

      EXPECT_EQ(nStopwatches, multistopwatch->StopwatchesNumber());
      EXPECT_EQ(multistopwatch->NameOf(0), "DDD");
      EXPECT_EQ(multistopwatch->NameOf(1), "BBB");
      EXPECT_EQ(multistopwatch->NameOf(2), "CCC");
      EXPECT_EQ(multistopwatch->NameOf(3), std::nullopt);
      EXPECT_EQ(multistopwatch->NameOf(10), std::nullopt);
   }

   {
      auto builder = MultiStopwatch::CreateBuilder(0);
      EXPECT_TRUE(builder.IsComplete());

      builder.WithStopwatchName(10, "ZZZ");
      EXPECT_TRUE(builder.IsComplete());

      auto multistopwatch = MultiStopwatch::FromBuilder(std::move(builder));
      EXPECT_TRUE(multistopwatch);
   }

   {
      auto builder =
         MultiStopwatch::CreateBuilder(2).WithStopwatchName(0, "AAA");
      EXPECT_FALSE(builder.IsComplete());

      auto multistopwatch = MultiStopwatch::FromBuilder(std::move(builder));
      EXPECT_FALSE(multistopwatch);
   }
}

TEST(ProfilingMultiStopwatch, StopwatchesIndependence) {
   usize nStopwatches = 3;
   auto builder       = MultiStopwatch::CreateBuilder(nStopwatches)
                     .WithStopwatchName(0, "AAA")
                     .WithStopwatchName(1, "BBB")
                     .WithStopwatchName(2, "CCC");

   auto multistopwatch = MultiStopwatch::FromBuilder(std::move(builder));
   EXPECT_TRUE(multistopwatch);

   EXPECT_TRUE(multistopwatch->IsKeyValid(0));
   EXPECT_TRUE(multistopwatch->IsKeyValid(1));
   EXPECT_TRUE(multistopwatch->IsKeyValid(2));
   EXPECT_FALSE(multistopwatch->IsKeyValid(3));

   multistopwatch->ResetBeginningOf(0);
   std::this_thread::sleep_for(std::chrono::milliseconds(3));
   multistopwatch->ResetBeginningOf(2);

   multistopwatch->RecordElapsedOf(0);
   multistopwatch->RecordElapsedOf(2);

   EXPECT_NE(multistopwatch->LastElapsedUsOf(0), 0);
   EXPECT_EQ(multistopwatch->LastElapsedUsOf(1), 0);
   EXPECT_NE(multistopwatch->LastElapsedUsOf(2), 0);
   
   EXPECT_GE(multistopwatch->LastElapsedUsOf(0),
             multistopwatch->LastElapsedUsOf(2));
   
}