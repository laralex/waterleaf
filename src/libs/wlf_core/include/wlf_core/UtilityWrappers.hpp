#pragma once

#include "UtilityInterfaces.hpp"

namespace wlf::utils {

template<typename T>
struct ENGINE_API NonAssignableWrap
      : public T
      , public INonAssignable {
   template<typename... Args>
   NonAssignableWrap(Args&&... args)
         : T(std::forward<Args>(args)...), INonAssignable() {}
};

} // namespace wlf::utils