cmake_minimum_required(VERSION 3.15.1 FATAL_ERROR)
project(techdemo)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SOURCE_DIR}/InternalUtils.cmake)
make_executable(${PROJECT_NAME}
  "${WLF_STRICT_WARNING_FLAGS}" # flags
  "wlf_render"                  # link against
  Main.cpp)