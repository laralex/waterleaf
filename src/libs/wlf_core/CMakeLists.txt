cmake_minimum_required(VERSION 3.15.1 FATAL_ERROR)
project(wlf_core)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SOURCE_DIR}/InternalUtils.cmake)
make_library(${PROJECT_NAME} ${WLF_LIBRARIES_TYPE}
    "${WLF_STRICT_WARNING_FLAGS}" # flags
    ""                            # link against
    Assertions.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DWLF_ASSERTION_LEVEL=${WLF_ASSERTION_LEVEL})