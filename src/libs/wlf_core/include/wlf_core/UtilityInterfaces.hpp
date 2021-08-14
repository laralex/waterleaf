#pragma once

#include "Define.hpp"

namespace wlf::util {

class ENGINE_API INonCopyable {
public:
   INonCopyable(const INonCopyable&) = delete;
   auto operator=(const INonCopyable&) -> INonCopyable& = delete;

   INonCopyable(INonCopyable&&) = default;
   auto operator=(INonCopyable&&) -> INonCopyable& = default;
   ~INonCopyable()                                 = default;

protected:
   INonCopyable() = default;
};

class ENGINE_API INonAssignable {
public:
   INonAssignable& operator=(const INonAssignable&) = delete;
   INonAssignable& operator=(INonAssignable&&) = delete;

   INonAssignable(const INonAssignable&) = default;
   INonAssignable(INonAssignable&&)      = default;
   ~INonAssignable()                     = default;

protected:
   INonAssignable() = default;
};

} // namespace wlf::util