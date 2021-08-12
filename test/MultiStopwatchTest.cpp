#include "wlf_core/Profiling.hpp"

#include "gtest/gtest.h"
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace wlf;
using namespace wlf::util;

class MultiStopwatchBuilderTest : public ::testing::Test {
protected:
   static void SetUpTestSuite() {
      for(usize i = 0; i < Names.size(); ++i) {
         Names[i] = std::to_string(i * 1000);
      }
   }
   static std::vector<std::string> Names;
   static std::vector<usize> NumStopwatchesVariants;
};

template<typename T>
class MultiStopwatchTest : public MultiStopwatchBuilderTest {
protected:
   static void SetUpTestSuite() {
      for(usize i = 0; i < NumStopwatchesVariants.size(); ++i) {
         StopwatchesVariants[i] =
            ConstructMultiStopwatch(NumStopwatchesVariants[i]);
      }
   }

   static std::optional<T> ConstructMultiStopwatch(usize nStopwatches) {
      auto builder = MultiStopwatchBuilder{nStopwatches};
      for(usize i = 0; i < nStopwatches; ++i) {
         builder.WithStopwatchName(i, std::string{Names[i]});
      }
      if constexpr(std::is_same_v<T, MultiStopwatch>) {
         return T::FromBuilder(std::move(builder));
      } else if constexpr(std::is_same_v<T, RecordingMultiStopwatch>) {
         usize nRecordingsCapacity = 10;
         return T::FromBuilder(nRecordingsCapacity, std::move(builder));
      }
   }

protected:
   static std::vector<std::optional<T>> StopwatchesVariants;
};

std::vector<std::string> MultiStopwatchBuilderTest::Names =
   std::vector<std::string>(1000);

std::vector<usize> MultiStopwatchBuilderTest::NumStopwatchesVariants = {1, 16,
                                                                        256};

template<typename T>
std::vector<std::optional<T>> MultiStopwatchTest<T>::StopwatchesVariants =
   std::vector<std::optional<T>>(NumStopwatchesVariants.size());

using AllStopwatchTypes =
   ::testing::Types<MultiStopwatch, RecordingMultiStopwatch>;
TYPED_TEST_SUITE(MultiStopwatchTest, AllStopwatchTypes);

TEST_F(MultiStopwatchBuilderTest, BuilderCompleteness) {
   for(auto nStopwatches : NumStopwatchesVariants) {
      auto builder = MultiStopwatchBuilder{nStopwatches};
      for(usize i = 0; i < nStopwatches * 2; i += 2) {
         EXPECT_FALSE(builder.IsComplete())
            << "Builder shouldn't be complete while not all stopwatches named."
            << " nStopwatches=" << nStopwatches << " i=" << i;
         builder.WithStopwatchName(i,
                                   std::string{this->Names[i % nStopwatches]});
      }
      if(nStopwatches > 1) {
         usize oddIndex = nStopwatches * 2 + 1;
         for(usize i = oddIndex; i > 1; i -= 2) {
            EXPECT_FALSE(builder.IsComplete())
               << "Builder shouldn't be complete while not all stopwatches named."
               << " nStopwatches=" << nStopwatches << " i=" << i;
            builder.WithStopwatchName(
               i, std::string{this->Names[i % nStopwatches]});
         }
      }
      builder.WithStopwatchName(1, std::string{this->Names[1]});
      EXPECT_TRUE(builder.IsComplete()) << "Builder should be complete";

      for(usize i = nStopwatches + 100; i < nStopwatches + 150; ++i) {
         builder.WithStopwatchName(i,
                                   std::string{this->Names[i % nStopwatches]});
         EXPECT_TRUE(builder.IsComplete())
            << "Builder should be complete still";
      }

      auto multiStopwatchOpt = MultiStopwatch::FromBuilder(std::move(builder));
      EXPECT_TRUE(multiStopwatchOpt);
   }
}

TEST_F(MultiStopwatchBuilderTest, BuilderWithoutStopwatches) {
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

TEST_F(MultiStopwatchBuilderTest, IncompleteBuilder) {
   usize nStopwatches = 20;
   for(usize upTo = 0; upTo < nStopwatches - 1; ++upTo) {
      auto builder = MultiStopwatchBuilder{nStopwatches};
      for(usize i = 0; i < upTo; ++i) {
         builder.WithStopwatchName(i,
                                   std::string{this->Names[i % nStopwatches]});
      }
      EXPECT_FALSE(builder.IsComplete())
         << "Not all stopwatches are initialized, builder should be incomplete";
      EXPECT_FALSE(MultiStopwatch::FromBuilder(std::move(builder)))
         << "Making from an incomplete builder should fail";
   }
}

TYPED_TEST(MultiStopwatchTest, StopwatchesNumber) {
   for(usize i = 0; i < this->StopwatchesVariants.size(); ++i) {
      EXPECT_EQ(this->StopwatchesVariants[i]->StopwatchesNumber(),
                this->NumStopwatchesVariants[i])
         << "Stopwatches number should match the builder's stopwatches number";
   }
}

TYPED_TEST(MultiStopwatchTest, IsKeyValid) {
   for(usize i = 0; i < this->StopwatchesVariants.size(); ++i) {
      usize nStopwatches = this->NumStopwatchesVariants[i];
      for(usize key = 0; key < nStopwatches; ++key) {
         EXPECT_TRUE(this->StopwatchesVariants[i]->IsKeyValid(key))
            << "Key less than StopwatchesNumber should be valid";
      }
      for(usize key = nStopwatches; key < nStopwatches + 100; ++key) {
         EXPECT_FALSE(this->StopwatchesVariants[i]->IsKeyValid(key))
            << "Key greater-equal than StopwatchesNumber should be invalid";
      }
   }
}

TYPED_TEST(MultiStopwatchTest, NameOf) {
   for(usize i = 0; i < this->StopwatchesVariants.size(); ++i) {
      usize nStopwatches = this->NumStopwatchesVariants[i];
      for(usize key = 0; key < nStopwatches; ++key) {
         EXPECT_EQ(this->StopwatchesVariants[i]->NameOf(key), this->Names[key])
            << "Name of stopwatch should match what was specified by builder";
      }
   }
}

TYPED_TEST(MultiStopwatchTest, SetBeginningOfAll) {
   u64 nRepeats = 10, baseOffsetMs = 10;

   Stopwatch prototype;
   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      auto now       = std::chrono::high_resolution_clock::now();
      auto offsetMs  = std::chrono::milliseconds(repeat * baseOffsetMs);
      auto timePoint = now - offsetMs;

      for(usize i = 0; i < this->StopwatchesVariants.size(); ++i) {
         usize nStopwatches = this->NumStopwatchesVariants[i];

         prototype.SetBeginning(timePoint);
         this->StopwatchesVariants[i]->SetBeginningOfAll(timePoint);

         for(usize key = 0; key < nStopwatches; ++key) {
            EXPECT_EQ(this->StopwatchesVariants[i]->BeginningOf(key),
                      prototype.Beginning())
               << "Beginning of all stopwatches should change";
         }
      }
   }
}

TYPED_TEST(MultiStopwatchTest, StoreElapsedOfAll) {
   u64 nRepeats = 5, baseOffsetMs = 10, allowedNoiseUs = 5000;

   Stopwatch prototype;
   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      for(usize i = 0; i < this->StopwatchesVariants.size(); ++i) {
         usize nStopwatches = this->NumStopwatchesVariants[i];
         auto& stopwatch    = this->StopwatchesVariants[i];
         auto timePoint     = std::chrono::high_resolution_clock::now()
                          - std::chrono::milliseconds(repeat * baseOffsetMs);
         prototype.SetBeginning(timePoint);
         stopwatch->SetBeginningOfAll(timePoint);

         prototype.StoreElapsed();
         stopwatch->StoreElapsedOfAll();

         for(usize key = 0; key < nStopwatches; ++key) {
            EXPECT_GE(stopwatch->GetElapsedUsOf(key),
                      prototype.GetElapsedUs())
               << "Elapsed timing shouldn't be lower than work time";
            EXPECT_LE(stopwatch->GetElapsedUsOf(key),
                      prototype.GetElapsedUs() + allowedNoiseUs)
               << "Elapsed timing shouldn't be much bigger than work time";
         }
      }
   }
}

TYPED_TEST(MultiStopwatchTest, ClearElapsedOfAll) {
   u64 nRepeats = 5, baseOffsetMs = 10;

   Stopwatch prototype;
   for(u64 repeat = 0; repeat < nRepeats; ++repeat) {
      for(usize i = 0; i < this->StopwatchesVariants.size(); ++i) {
         usize nStopwatches = this->NumStopwatchesVariants[i];
         auto& stopwatch    = this->StopwatchesVariants[i];
         auto timePoint     = std::chrono::high_resolution_clock::now()
                          - std::chrono::milliseconds(repeat * baseOffsetMs);
         prototype.SetBeginning(timePoint);
         stopwatch->SetBeginningOfAll(timePoint);

         prototype.StoreElapsed();
         stopwatch->StoreElapsedOfAll();

         prototype.ClearElapsed();
         stopwatch->ClearElapsedOfAll();

         for(usize key = 0; key < nStopwatches; ++key) {
            EXPECT_EQ(stopwatch->GetElapsedOf(key), prototype.GetElapsed())
               << "After clearing the saved elapsed should reset";
         }
      }
   }
}

// TODO(laralex): complete tests
TYPED_TEST(MultiStopwatchTest, AddStoreElapsedOfAll) {}

TYPED_TEST(MultiStopwatchTest, SetBeginningOf) {}
TYPED_TEST(MultiStopwatchTest, StoreElapsedOf) {}
TYPED_TEST(MultiStopwatchTest, AddStoreElapsedOf) {}
TYPED_TEST(MultiStopwatchTest, ClearElapsedOf) {}
TYPED_TEST(MultiStopwatchTest, GetElapsedOf) {}

TYPED_TEST(MultiStopwatchTest, StopwatchesIndependence) {}