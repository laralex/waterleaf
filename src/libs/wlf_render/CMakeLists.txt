cmake_minimum_required(VERSION 3.15.1 FATAL_ERROR)
project(wlf_render)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SOURCE_DIR}/InternalUtils.cmake)
make_library(${PROJECT_NAME} ${WLF_LIBRARIES_TYPE}
    "${WLF_BASE_WARNING_FLAGS}"   # flags
    "wlf_core glm"                # link against
    Dummy.cpp)
