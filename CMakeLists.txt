# SPDX-License-Identifier: MIT
#
# RH512 non-cryptographic hash (G3-RH512-256 library)
#
# Copyright (C) 2026 Antonio García Leal (garcialeal)
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.15)
project(g3_rh512_256 VERSION 2.0.0 LANGUAGES C CXX)

# Tests are built by default only when this is the top-level project,
# not when it is included from another project (e.g. as a submodule).
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(G3_HASH_IS_TOP_LEVEL ON)
else()
    set(G3_HASH_IS_TOP_LEVEL OFF)
endif()
option(G3_HASH_BUILD_TESTS "Build the G3-RH512-256 test suite" ${G3_HASH_IS_TOP_LEVEL})

# Compiler warnings for this project's own targets only (not propagated).
function(g3_hash_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
    endif()
endfunction()

add_library(g3_hash STATIC src/g3_hash.c)
target_include_directories(g3_hash PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(g3_hash PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
g3_hash_enable_warnings(g3_hash)

if(G3_HASH_BUILD_TESTS)
    enable_testing()

    # C test: SMHasher3 verification value, known answer, alignment, consistency
    add_executable(test_g3 tests/test_g3.c)
    target_link_libraries(test_g3 PRIVATE g3_hash)
    set_target_properties(test_g3 PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
    g3_hash_enable_warnings(test_g3)
    add_test(NAME g3_hash_test COMMAND test_g3)

    # C++ test: the optional C++17 interface in g3_hash.h
    add_executable(test_g3_cpp tests/test_g3_cpp.cpp)
    target_link_libraries(test_g3_cpp PRIVATE g3_hash)
    set_target_properties(test_g3_cpp PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    g3_hash_enable_warnings(test_g3_cpp)
    add_test(NAME g3_hash_cpp_test COMMAND test_g3_cpp)
endif()
