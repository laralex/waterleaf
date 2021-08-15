#pragma once

#pragma clang diagnostic ignored "-Wmacro-redefined"
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "glm/glm.hpp"

#include <cstdint>



#define ENGINE_API __declspec(dllexport)
namespace wlf {

/* wlf::IsDebugBuild
 * Indicates if the library was compiled with WLF_DEBUG define,
 * (which most likely comes from Debug build type in CMake) */

#ifdef WLF_DEBUG
inline constexpr bool IsDebugBuild = WLF_DEBUG;
#else
inline constexpr bool IsDebugBuild = false;
#endif

using f32 = float;
using f64 = double;

using i8    = std::int8_t;
using i16   = std::int16_t;
using i32   = std::int32_t;
using i64   = std::int64_t;
using isize = std::ptrdiff_t;

using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using u64   = std::uint64_t;
using usize = std::size_t;

using v2_f32 = glm::vec<2, wlf::f32>;
using v3_f32 = glm::vec<3, wlf::f32>;
using v4_f32 = glm::vec<4, wlf::f32>;

using v2_f64 = glm::vec<2, wlf::f64>;
using v3_f64 = glm::vec<3, wlf::f64>;
using v4_f64 = glm::vec<4, wlf::f64>;

using v2_i32 = glm::vec<2, wlf::i32>;
using v3_i32 = glm::vec<3, wlf::i32>;
using v4_i32 = glm::vec<4, wlf::i32>;

using v2_u32 = glm::vec<2, wlf::u32>;
using v3_u32 = glm::vec<3, wlf::u32>;
using v4_u32 = glm::vec<4, wlf::u32>;

using v2_bool = glm::vec<2, bool>;
using v3_bool = glm::vec<3, bool>;
using v4_bool = glm::vec<4, bool>;

using v2 = v2_f32;
using v3 = v3_f32;
using v4 = v4_f32;

using mat2_f32 = glm::mat<2, 2, wlf::f32>;
using mat3_f32 = glm::mat<3, 3, wlf::f32>;
using mat4_f32 = glm::mat<4, 4, wlf::f32>;

using mat2_f64 = glm::mat<2, 2, wlf::f64>;
using mat3_f64 = glm::mat<3, 3, wlf::f64>;
using mat4_f64 = glm::mat<4, 4, wlf::f64>;

using mat2 = mat2_f32;
using mat3 = mat3_f32;
using mat4 = mat4_f32;

} // namespace wlf
