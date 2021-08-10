#include "Assertion.hpp"

#include <assert.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string_view>

using namespace wlf;

namespace {

constexpr auto operator&(const AssertionLevel lhs, const AssertionLevel rhs)
   -> bool {
   return static_cast<size_t>(lhs) & static_cast<size_t>(rhs);
}

constexpr void AbortIfNotOk(const bool isOk, const std::string_view message) {
   if(!isOk) {
      spdlog::critical("Assertion failed: {}", message);
      // TODO(laralex): fancier termination (error window, etc)
      exit(1);
   }
}

} // namespace

/* Assertion utilities */

void wlf::Assert(const bool isOk, const std::string_view message) noexcept {
   constexpr bool chosen = (EnabledAssertions & AssertionLevel::RunTime);
   if constexpr(!wlf::NoAsserts && chosen) { AbortIfNotOk(isOk, message); }
}

void wlf::AssertDebug(const bool isOk, const std::string_view message) noexcept {
   constexpr bool chosen = (EnabledAssertions & AssertionLevel::DebugRunTime);
   if constexpr(!wlf::NoAsserts && wlf::IsDebugBuild && chosen) {
      AbortIfNotOk(isOk, message);
   }
}