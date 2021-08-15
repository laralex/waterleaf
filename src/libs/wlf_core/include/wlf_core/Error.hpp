#pragma once

#include "Define.hpp"
#include "spdlog/spdlog.h"

#include "spdlog/sinks/stdout_sinks.h"
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>



namespace wlf::error {

template<typename T>
struct HumanizedError final {
   HumanizedError() = delete;
   static const std::vector<std::string_view> Descriptions;
};

template<typename T>
const auto HumanizedError<T>::Descriptions = std::vector<std::string_view>();

template<
   typename T, typename = std::enable_if_t<std::is_convertible_v<T, usize>>>
auto DescriptionOf [[nodiscard]] (const T errorCode)
-> std::optional<std::string_view> {
   if(static_cast<usize>(errorCode) >= HumanizedError<T>::Descriptions.size()) {
      return std::nullopt;
   }
   return HumanizedError<T>::Descriptions[static_cast<usize>(errorCode)];
}

template<
   typename T, typename = std::enable_if_t<std::is_convertible_v<T, usize>>>
void LogError(const T errorCode) {
   auto errorDescription = DescriptionOf(errorCode);
   if(!errorDescription) { return; }
   auto console = spdlog::stdout_logger_mt("console");
   spdlog::set_default_logger(console);
   spdlog::set_pattern("[in %s, at %!:%#] %v");
   SPDLOG_TRACE(errorDescription.value());
}

} // namespace wlf::error
