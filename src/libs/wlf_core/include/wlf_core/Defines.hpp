#pragma once

#define ENGINE_API __declspec(dllexport)

namespace wlf {
    /*    wlf::IsDebugBuild
        Indicates if the library was compiled with WLF_DEBUG define,
        (which most likely comes from Debug build type in CMake) */

    #ifdef WLF_DEBUG
    inline constexpr bool IsDebugBuild = WLF_DEBUG;
    #else
    inline constexpr bool IsDebugBuild = false;
    #endif
}
