cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(waterleaf_render)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_FILES
    Dummy.cpp)

add_library(wlf_render SHARED "${SRC_FILES}")
set_target_properties(wlf_render PROPERTIES FOLDER libs)

target_include_directories(wlf_render PUBLIC "include" PRIVATE "include/wlf_render")
target_link_libraries(wlf_render wlf_core glm)