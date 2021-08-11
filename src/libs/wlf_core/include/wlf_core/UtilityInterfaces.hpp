#pragma once

#include "Defines.hpp"

namespace wlf::util {

class ENGINE_API INonCopyable {
public:
   INonCopyable(const INonCopyable&) = delete;
   INonCopyable& operator=(const INonCopyable&) = delete;

   INonCopyable(INonCopyable&&) = default;
   INonCopyable& operator=(INonCopyable&&) = default;
   ~INonCopyable() = default;
protected:
   INonCopyable() = default;
};

class ENGINE_API INonAssignable {
public:
   INonAssignable& operator=(const INonAssignable&) = delete;
   INonAssignable& operator=(INonAssignable&&) = delete;
   
   INonAssignable(const INonAssignable&) = default;
   INonAssignable(INonAssignable&&) = default;
   ~INonAssignable() = default;
protected:
   INonAssignable() = default;
};

}