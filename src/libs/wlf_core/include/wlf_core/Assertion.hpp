#pragma once

#include "Define.hpp"

#include <string_view>
#include <utility>

namespace wlf::detail {

enum class AssertionLevel : size_t {
   // only one enabled: define wlf::NoAsserts == true
   // else:             define wlf::NoAsserts == false
   None = 1 << 0,
   // enabled : since wlf::NoAsserts == false,
   //           static_assert(false||wlf::NoAsserts) aborts compilation
   CompileTime = 1 << 1,
   // enabled : wlf::Assert panics in any build type, fail is logged
   // disabled: wlf::Assert is noop
   RunTime = 1 << 2,
   // enabled : wlf::DebugAssert panics in Debug build type, fail is logged
   // disabled: wlf::DebugAssert is noop
   DebugRunTime = 1 << 3,
};

auto constexpr operator|(const AssertionLevel lhs, const AssertionLevel rhs)
   -> AssertionLevel {
   return static_cast<AssertionLevel>(static_cast<size_t>(lhs)
                                      | static_cast<size_t>(rhs));
}

inline constexpr AssertionLevel EnabledAssertions =
   AssertionLevel::None
#if WLF_ASSERTION_LEVEL >= 1
   | AssertionLevel::CompileTime
#endif
#if WLF_ASSERTION_LEVEL >= 2
   | AssertionLevel::RunTime
#endif
#if WLF_ASSERTION_LEVEL >= 3
   | AssertionLevel::DebugRunTime
#endif
   ; // EnabledAssertions
} // namespace wlf::detail

namespace wlf {

/* wlf::NoAsserts
 * Indicates if the library was configured to skip compilation of assertions
 * NOTE: use it to suppress `static_assert` like so
 * `static_assert(ok || wlf::NoAsserts)` */
ENGINE_API
inline constexpr bool NoAsserts =
   (detail::EnabledAssertions == detail::AssertionLevel::None);

/* wlf::Assert
 * If library is configured to check runtime assertions, this assertion
 * will be compiled in any build type (Debug/Release) */
ENGINE_API
void AssertRelease(const bool isOk, const std::string_view message) noexcept;

/* wlf::Assert
 * If library is configured to check debug runtime assertions, this assertion
 * will be compiled in Debug build type, and will be a noop in Release */
ENGINE_API
void Assert(const bool isOk, const std::string_view message) noexcept;

} // namespace wlf
