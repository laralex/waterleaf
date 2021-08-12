#include "Assertion.hpp"

#include <assert.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string_view>

using namespace wlf;

namespace {

auto constexpr IsFlagSet(const detail::AssertionLevel lhs,
                         const detail::AssertionLevel rhs) noexcept -> bool {
   return static_cast<size_t>(lhs) & static_cast<size_t>(rhs);
}

void constexpr AbortIfNotOk(const bool isOk,
                            const std::string_view message) noexcept {
   if(!isOk) {
      spdlog::critical("Assertion failed: {}", message);
      // TODO(laralex): fancier termination (error window, etc)
      std::terminate();
   }
}

} // namespace

/* Assertion utilities */

void wlf::AssertRelease(const bool isOk,
                        const std::string_view message) noexcept {
   constexpr bool chosen =
      IsFlagSet(detail::EnabledAssertions, detail::AssertionLevel::RunTime);
   if constexpr(!wlf::NoAsserts && chosen) { AbortIfNotOk(isOk, message); }
}

void wlf::Assert(const bool isOk, const std::string_view message) noexcept {
   constexpr bool chosen = IsFlagSet(detail::EnabledAssertions,
                                     detail::AssertionLevel::DebugRunTime);
   if constexpr(!wlf::NoAsserts && wlf::IsDebugBuild && chosen) {
      AbortIfNotOk(isOk, message);
   }
}