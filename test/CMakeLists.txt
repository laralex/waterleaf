cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(waterleaf_tests)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==== building and configuring GTest ====

# NOTE: GoogleTest is not a git submodule, in order to prevent downloading of it
# when building without tests enabled
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

include(FetchContent)
set(FETCHCONTENT_QUIET off)
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/download")
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.11.0
)

# prevent overriding compiler/linker settings (on Windows)
include(GoogleTest)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory("${googletest_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/download/googletest-build")
endif()

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
macro(assemble_test TESTNAME FILES)
    add_executable(${TESTNAME} "${FILES}")
    set_target_properties(${TESTNAME} PROPERTIES FOLDER wlf_tests)
    target_link_libraries(${TESTNAME} ${GTEST_TARGETS} ${ARGN})
    gtest_discover_tests(${TESTNAME}
        SOURCES "${FILES}"
        WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
endmacro()

assemble_test(wlf_unit_tests "DummyTest.cpp" wlf_render)
