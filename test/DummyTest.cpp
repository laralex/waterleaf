#include <gtest/gtest.h>
#include "wlf_render/Dummy.hpp"
#include "wlf_core/Prelude.hpp"

GTEST_TEST(Example, Example)
{
    auto res = wlf::render::ExampleSin(3.14159f * 0.5f);
    ASSERT_NEAR(res, 1.0f, 0.001f);

    static_assert(true || wlf::NoAsserts, "not_ok_static");
    wlf::Assert(true, "not_ok_release");
    wlf::AssertDebug(true, "not_ok_debug");
}
