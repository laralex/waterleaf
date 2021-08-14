#pragma once

#include "Define.hpp"

#include <vector>
#include <string_view>
#include <type_traits>

namespace wlf::error {

template<typename T>
class HumanizedError final {
   HumanizedError() = delete;

public:
   static const std::vector<std::string_view> Descriptions;
};

template<
   typename T, typename = std::enable_if_t<std::is_convertible_v<T, usize>>>
const std::string_view DescriptionOf [[nodiscard]] (const T errorCode) {
   return HumanizedError<T>::Descriptions[static_cast<usize>(errorCode)];
}
} // namespace wlf::util
