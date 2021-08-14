#pragma once

#include "Define.hpp"

#include <functional>
#include <type_traits>


namespace wlf {

namespace detail {
enum class InvokeResult { NotExecutedNothingToReturn };
}

template<typename F, typename... Args>
auto InvokeInDebug(F&& function, Args&&... args) noexcept(noexcept(
   std::invoke(std::forward<F>(function), std::forward<Args>(args)...))) {
   if constexpr(IsDebugBuild) {
      return std::invoke(std::forward<F>(function),
                         std::forward<Args>(args)...);
   } else {
      return detail::InvokeResult::NotExecutedNothingToReturn;
   }
}

} // namespace wlf