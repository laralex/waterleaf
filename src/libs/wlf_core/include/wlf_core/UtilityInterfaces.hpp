#pragma once

namespace wlf::utils {

class INonCopyable {
public:
   INonCopyable(const INonCopyable&) = delete;
   INonCopyable& operator=(const INonCopyable&) = delete;
   INonCopyable(INonCopyable&&) = default;
   INonCopyable& operator=(INonCopyable&&) = default;
protected:
   INonCopyable() = default;
   ~INonCopyable() = default;
};

}