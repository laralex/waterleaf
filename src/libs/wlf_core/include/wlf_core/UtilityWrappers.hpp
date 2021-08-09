#pragma once

#include "UtilityInterfaces.hpp"

namespace wlf::utils {

template<typename T>
class ENGINE_API NonAssignableWrap
      : public INonAssignable
      , public INonCopyable {
public:
   NonAssignableWrap(T& wrapee)
         : INonAssignable(), INonCopyable(), m_WrappedRef(wrapee) {}

   T& operator->() noexcept {
      return m_WrappedRef;
   }

   const T& operator->() const noexcept {
      return m_WrappedRed;
   }

private:
   T& m_WrappedRef;
};

} // namespace wlf::utils