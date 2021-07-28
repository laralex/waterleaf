#include "wlf_render/Dummy.hpp"

#include "wlf_core/Prelude.hpp"

#include <gtest/gtest.h>
#include <iostream>

using namespace wlf;

GTEST_TEST(Example, Example) {
   {
      auto [res, timeUs] =
         MeasureRunMicroseconds(render::ExampleSin, 3.14159f * 0.5f);
      std::cout << "Execution time (microsec): " << timeUs << std::endl;
   }

   {
      auto [res, timeNs] =
         MeasureRunNanoseconds(render::ExampleSin, 3.14159f * 0.5f);
      std::cout << "Execution time (nanosecs): " << timeNs << std::endl;

      ASSERT_NEAR(res, 1.0f, 0.001f);

      std::cout << "Enabled assertions" << static_cast<uint64_t>(EnabledAssertions) << " " << NoAsserts << std::endl;
      static_assert(true || NoAsserts, "not_ok_static");
      Assert(true, "not_ok_release");
      AssertDebug(false, "not_ok_debug");
   }
}
