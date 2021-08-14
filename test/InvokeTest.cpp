#include "wlf_core/Invoke.hpp"

#include "wlf_core/Define.hpp"

#include "gtest/gtest.h"


using namespace wlf;

TEST(InvokeInDebugTest, OutputPassedThrough) {
   struct Result {
      f32 a;
      u8 b;
      Result(f32 a, u8 b) : a(a), b(b) {}
   };

   {
      const auto DummyFunc = []() { return Result(42.0f, 123); };
      auto result          = wlf::InvokeInDebug(DummyFunc);
      if constexpr(wlf::IsDebugBuild) {
         EXPECT_NEAR(result.a, 42.0f, 1e-5);
         EXPECT_EQ(result.b, 123);
      } else {
         EXPECT_EQ(result,
                   wlf::detail::InvokeResult::NotExecutedNothingToReturn);
      }
   }

   {
      const auto DummyFunc = []() { return Result(36.5f, 42); };
      auto result          = wlf::InvokeInDebug(DummyFunc);
      if constexpr(wlf::IsDebugBuild) {
         EXPECT_NEAR(result.a, 36.5f, 1e-5);
         EXPECT_EQ(result.b, 42);
      } else {
         EXPECT_EQ(result,
                   wlf::detail::InvokeResult::NotExecutedNothingToReturn);
      }
   }

   {
      auto DummyFunc = []() {};
      wlf::InvokeInDebug(DummyFunc);
      bool returnedVoid = std::is_void_v<
         std::invoke_result_t<decltype(wlf::InvokeInDebug<decltype(DummyFunc)>),
                              decltype(DummyFunc)>>;
      if constexpr(wlf::IsDebugBuild) { EXPECT_TRUE(returnedVoid); }
   }
}