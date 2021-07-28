#include "wlf_render/Dummy.hpp"

#include "wlf_core/Prelude.hpp"

#include <spdlog/spdlog.h>
#include <gtest/gtest.h>

using namespace wlf;

GTEST_TEST(Example, Example) {
   {
      auto [res, timeUs] =
         MeasureRunMicroseconds(render::ExampleSin, 3.14159f * 0.5f);
      spdlog::info("Execution time (microsec): {:08d}", timeUs);
   }

   {
      auto [res, timeNs] =
         MeasureRunNanoseconds(render::ExampleSin, 3.14159f * 0.5f);
      spdlog::info("Execution time (nanosecs): {:08d}", timeNs);

      ASSERT_NEAR(res, 1.0f, 0.001f);

      static_assert(true || NoAsserts, "not_ok_static");
      Assert(true, "not_ok_release");
      AssertDebug(false, "not_ok_debug");
   }
}
