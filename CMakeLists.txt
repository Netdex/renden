cmake_minimum_required(VERSION 3.16)
project(renden)

set(CMAKE_CXX_STANDARD 17)

add_library(coverage_config INTERFACE)
option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
	target_link_options(coverage_config INTERFACE --coverage)
endif()

option(PACKAGE_TESTS "Build tests for this project" OFF)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

add_subdirectory(src)
