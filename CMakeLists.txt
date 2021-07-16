cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(waterleaf)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WLF_BUILD_TESTS "Build the unit tests" ON)
set(WLF_VERBOSITY_LEVEL "100" CACHE STRING
    "Messages with not lower level will be logged/printed \
    ('0' no log, '1' critical, '2' +interdictory errors, '3' +warnings, etc)" FORCE)

set(WLF_ASSERTION_LEVEL "100" CACHE STRING
    "Assertions with not lower level will be checked and required to pass \
    ('0' none, '1' static_assert, '2' +runtime, '3' +debug runtime" FORCE)

# ==== end of config ====

# build setup
add_definitions(
    -DWLF_VERBOSITY_LEVEL=${WLF_VERBOSITY_LEVEL}
    -DWLF_ASSERTION_LEVEL=${WLF_ASSERTION_LEVEL})

if(!CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Cached CMake build type")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DWLF_DEBUG)
    set(CMAKE_DEBUG_POSTFIX "_debug" CACHE STRING "")
endif()

set(WLF_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(WLF_ARTIFACTS_DIR "${CMAKE_CURRENT_BINARY_DIR}/artifacts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WLF_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WLF_ARTIFACTS_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WLF_ARTIFACTS_DIR}")
set(CMAKE_FILES_DIRECTORY "/.cmake")

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
