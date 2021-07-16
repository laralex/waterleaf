cmake_minimum_required(VERSION 3.15.1 FATAL_ERROR)
project(waterleaf_core)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_FILES
    Assertions.cpp)

add_library(wlf_core SHARED "${SRC_FILES}")
set_target_properties(wlf_core PROPERTIES FOLDER libs)

target_include_directories(wlf_core PUBLIC "include" PRIVATE "include/wlf_core")

target_compile_definitions(wlf_core PRIVATE -DWLF_ASSERTION_LEVEL=${WLF_ASSERTION_LEVEL})