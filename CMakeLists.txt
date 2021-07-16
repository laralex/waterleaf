cmake_minimum_required(VERSION 3.15.1 FATAL_ERROR)
project(waterleaf)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WLF_BUILD_TESTS "Build the unit tests" ON)
option(WLF_FAST_COMPILE "Build without slow flags (like warnings)" ON)

set(WLF_ASSERTION_LEVEL "100" CACHE STRING
    "Assertions with not lower level will be checked and required to pass \
    ('0' none, '1' static_assert, '2' +runtime, '3' +debug runtime" FORCE)

# ==== end of config ====

# build setup
if(NOT WLF_FAST_COMPILE)
    set(WLF_BASE_WARNING_FLAGS   -Wall -Wshadow -Werror -Wconversion)
    set(WLF_STRICT_WARNING_FLAGS -W    -Wpointer-arith  -Wreturn-type
        -Wcast-qual  -Wwrite-strings   -Wswitch -Wunused-parameter
        -Wcast-align -Wchar-subscripts -Winline -Wredundant-decls)
    set(WLF_DEBUG_FLAGS -DWLF_DEBUG)
    set(WLF_OPTIMIZING_FLAGS -O2)
endif()

add_compile_options(${WLF_BASE_WARNING_FLAGS} ${WLF_STRICT_WARNING_FLAGS})

if(!CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(${WLF_DEBUG_FLAGS})
    set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "")
elseif(CMAKE_BUILD_TYPE MATCHES "^(Release|RelWithDebInfo|MinSizeRel)$")
    add_compile_options(${WLF_OPTIMIZING_FLAGS})
endif()

set(WLF_BINARY_DIR    "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(WLF_ARTIFACTS_DIR "${CMAKE_CURRENT_BINARY_DIR}/artifacts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WLF_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WLF_ARTIFACTS_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WLF_ARTIFACTS_DIR}")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${WLF_ARTIFACTS_DIR}")

# external libraries
set(EXTERN_ARTIFACTS_DIR "${CMAKE_CURRENT_BINARY_DIR}/artifacts_extern")
set(EXTERN_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")

add_subdirectory("${EXTERN_SOURCE_DIR}/glm" "${EXTERN_ARTIFACTS_DIR}/glm")
set_target_properties(glm PROPERTIES FOLDER extern)

# project source code
add_subdirectory(src/libs/wlf_core   "${WLF_ARTIFACTS_DIR}/wlf_core")
add_subdirectory(src/libs/wlf_render "${WLF_ARTIFACTS_DIR}/wlf_render")
add_subdirectory(src/techdemo        "${WLF_ARTIFACTS_DIR}/techdemo")

message(STATUS "> Building tests: ${WLF_BUILD_TESTS}")
if(WLF_BUILD_TESTS)
    add_subdirectory(test "${EXTERN_ARTIFACTS_DIR}/googletest")
endif()
