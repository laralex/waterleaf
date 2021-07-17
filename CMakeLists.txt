cmake_minimum_required(VERSION 3.15.1 FATAL_ERROR)
project(waterleaf)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WLF_BUILD_TESTS "Build the unit tests" ON)
option(WLF_FAST_COMPILE "Build without slow flags (like warnings)" OFF)
set(WLF_LIBRARIES_TYPE "SHARED" CACHE STRING
   "Build and link libraries as static (when off - shared)")
set(WLF_ASSERTION_LEVEL "100" CACHE STRING
   "Assertions with not lower level will be checked and required to pass \
   ('0' none, '1' static_assert, '2' +runtime, '3' +debug runtime" FORCE)

# ==== end of config ====

# build setup
set(WLF_BASE_WARNING_FLAGS "-Wall -Wshadow -Werror -Wconversion" CACHE STRING "")
set(WLF_DEBUG_FLAGS "-DWLF_DEBUG")
if(NOT WLF_FAST_COMPILE)
   set(WLF_STRICT_WARNING_FLAGS "-W -Wswitch -Winline -Wreturn-type \
      -Wwrite-strings   -Wpointer-arith -Wunused-parameter -Wcast-qual \
      -Wchar-subscripts -Wcast-align    -Wredundant-decls" CACHE STRING "")
   set(WLF_OPTIMIZING_FLAGS "-O2")
endif()

if(!CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release CACHE STRING "")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   add_compile_options(${WLF_DEBUG_FLAGS})
elseif(CMAKE_BUILD_TYPE MATCHES "^(Release|RelWithDebInfo|MinSizeRel)$")
   add_compile_options(${WLF_OPTIMIZING_FLAGS})
endif()

set(WLF_BINARY_DIR    "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(WLF_ARTIFACTS_DIR "${CMAKE_CURRENT_BINARY_DIR}/artifacts")

# external libraries
set(EXTERN_SUBDIRS glm)
foreach(subdir ${EXTERN_SUBDIRS})
   add_subdirectory(
   "${CMAKE_CURRENT_SOURCE_DIR}/extern/${subdir}"
   "${CMAKE_CURRENT_BINARY_DIR}/artifacts_extern/${subdir}")
   set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/extern/${subdir}"
   PROPERTY FOLDER extern)
endforeach()

# project source code
add_subdirectory(src/libs/wlf_core   "${WLF_ARTIFACTS_DIR}/wlf_core")
add_subdirectory(src/libs/wlf_render "${WLF_ARTIFACTS_DIR}/wlf_render")
add_subdirectory(src/techdemo        "${WLF_ARTIFACTS_DIR}/techdemo")

message(STATUS "> Building tests: ${WLF_BUILD_TESTS}")
if(WLF_BUILD_TESTS)
   add_subdirectory(test "${CMAKE_CURRENT_BINARY_DIR}/artifacts_extern/googletest")
endif()
