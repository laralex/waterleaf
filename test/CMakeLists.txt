cmake_minimum_required(VERSION 3.15.1 FATAL_ERROR)
project(wlf_unit_tests)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==== building and configuring GTest ====

# NOTE: GoogleTest is not a git submodule, in order to prevent downloading of it
# when building without tests enabled

# prevent overriding compiler/linker settings (on Windows)
include(GoogleTest)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/download")
FetchContent_Declare(
   googletest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.11.0
)
FetchContent_MakeAvailable(googletest)

set(GTEST_TARGETS gtest gtest_main gmock gmock_main)

set_target_properties(${GTEST_TARGETS} PROPERTIES
   CMAKE_INSTALL_LIBDIR "${CMAKE_CURRENT_BINARY_DIR}/libs"
   LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libs"
   ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libs"
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
   PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

# keep cache cleaner
# https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
mark_as_advanced(
   BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
   gmock_build_tests gtest_build_samples gtest_build_tests
   gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

# ==== tests ====

include(${CMAKE_SOURCE_DIR}/InternalUtils.cmake)
function(make_test name link_against group_variable)
   make_executable(${name} "${WLF_BASE_WARNING_FLAGS}" "${link_against} ${GTEST_TARGETS}" ${ARGN})
   gtest_discover_tests(${name} SOURCES ${ARGN}
      WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
   set(${group_variable} ${${group_variable}} ${ARGN} PARENT_SCOPE)
endfunction()

# make_test(critical_tests "wlf_render" WLF_CRITICAL_TESTS "")
make_test(time_consuming_tests "wlf_render" WLF_TIME_CONSUMING_TESTS ProfilingFunctionTest.cpp)
make_test(aux_tests "wlf_render" WLF_AUX_TESTS ProfilingSectionTest.cpp)
make_test(all_tests "wlf_render" ""
   ${WLF_CRITICAL_TESTS}
   ${WLF_TIME_CONSUMING_TESTS}
   ${WLF_AUX_TESTS})
