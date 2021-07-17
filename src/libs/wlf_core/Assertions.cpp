#include <assert.h>
#include <string_view>
#include <iostream>

#include "Assertions.hpp"

using namespace wlf;

namespace {
  constexpr bool operator&(AssertionLevel lhs, AssertionLevel rhs) {
    return static_cast<ptrdiff_t>(lhs) & static_cast<ptrdiff_t>(rhs);
  }
}

/* Assertion utilities */
void wlf::Assert(bool isOk, std::string_view message) {
  if constexpr (!wlf::NoAsserts && (EnabledAssertions & AssertionLevel::RunTime)) {
    if (!isOk) {
      std::cout << message << '\n';
      // TODO(laralex): fancier termination (error window, etc)
      exit(1);
    }
  }
}

void wlf::AssertDebug(bool isOk, std::string_view message) {
  constexpr bool chosen = (EnabledAssertions & AssertionLevel::DebugRunTime);
  if constexpr (!wlf::NoAsserts && wlf::IsDebugBuild && chosen) {
    if (!isOk) {
      std::cout << message << '\n';
      // TODO(laralex): fancier termination (error window, etc)
      exit(1);
    }
  }
}