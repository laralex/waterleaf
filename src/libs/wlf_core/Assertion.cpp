#include "Assertion.hpp"

#include <assert.h>
#include <iostream>
#include <string_view>

using namespace wlf;

namespace {

constexpr auto operator&(AssertionLevel lhs, AssertionLevel rhs) -> bool {
   return static_cast<ptrdiff_t>(lhs) & static_cast<ptrdiff_t>(rhs);
}

}

/* Assertion utilities */

void wlf::Assert(bool isOk, std::string_view message) {
   constexpr bool chosen = (EnabledAssertions & AssertionLevel::RunTime);
   if constexpr(!wlf::NoAsserts && chosen) {
      if(!isOk) {
         std::cout << message << '\n';
         // TODO(laralex): fancier termination (error window, etc)
         exit(1);
      }
   }
}

void wlf::AssertDebug(bool isOk, std::string_view message) {
   constexpr bool chosen = (EnabledAssertions & AssertionLevel::DebugRunTime);
   if constexpr(!wlf::NoAsserts && wlf::IsDebugBuild && chosen) {
      if(!isOk) {
         std::cout << message << '\n';
         // TODO(laralex): fancier termination (error window, etc)
         exit(1);
      }
   }
}