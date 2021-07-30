#include "Assertion.hpp"

#include <assert.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string_view>




using namespace wlf;

namespace {

constexpr auto operator&(AssertionLevel lhs, AssertionLevel rhs) -> bool {
   return static_cast<ptrdiff_t>(lhs) & static_cast<ptrdiff_t>(rhs);
}

constexpr void AbortIfNotOk(bool isOk, std::string_view message) {
   if(!isOk) {
      spdlog::critical("Assertion failed: {}", message);
      // TODO(laralex): fancier termination (error window, etc)
      exit(1);
   }
}

} // namespace

/* Assertion utilities */

void wlf::Assert(bool isOk, std::string_view message) {
   constexpr bool chosen = (EnabledAssertions & AssertionLevel::RunTime);
   if constexpr(!wlf::NoAsserts && chosen) { AbortIfNotOk(isOk, message); }
}

void wlf::AssertDebug(bool isOk, std::string_view message) {
   constexpr bool chosen = (EnabledAssertions & AssertionLevel::DebugRunTime);
   if constexpr(!wlf::NoAsserts && wlf::IsDebugBuild && chosen) {
      AbortIfNotOk(isOk, message);
   }
}