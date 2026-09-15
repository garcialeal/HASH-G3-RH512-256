# SPDX-License-Identifier: MIT
#
# G3-RH512-256 Non-Cryptographic Spatial Hash Engine
#
# Copyright (C) 2026 Antonio García Leal (garcialeal)
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.15)
project(g3_rh512_256 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(g3_hash STATIC src/g3_hash.c)
target_include_directories(g3_hash PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

enable_testing()
add_executable(test_g3 tests/test_g3.c)
target_link_libraries(test_g3 PRIVATE g3_hash)
add_test(NAME g3_hash_test COMMAND test_g3)