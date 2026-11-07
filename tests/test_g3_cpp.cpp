/*
 * SPDX-License-Identifier: MIT
 *
 * RH512 non-cryptographic hash (G3-RH512-256 library) - C++ interface test
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 * Licensed under the MIT License.
 */

#include "g3_hash.h"
#include <cstdio>
#include <cstring>
#include <string>

int main() {
    int failures = 0;

    // Same known-answer vector as tests/test_g3.c
    const v4s12::Hash256 expected = {
        0x79, 0xb6, 0xae, 0xf1, 0x25, 0x8e, 0xbb, 0xca,
        0x99, 0xfb, 0x49, 0xc1, 0xfa, 0x73, 0xe1, 0xf8,
        0x6d, 0x49, 0x44, 0x69, 0xd8, 0xd4, 0x74, 0xca,
        0x3c, 0x9f, 0xb3, 0x17, 0x04, 0xab, 0xc9, 0x56
    };
    const std::string msg = "V4S12_DETERMINISTIC_VECTOR_GEOMETRY";

    const v4s12::Hash256 digest = v4s12::compute_g3_hash(msg.data(), msg.size());
    const bool kat_ok = (digest == expected);
    std::printf("[%s] compute_g3_hash known-answer test\n", kat_ok ? "PASS" : "FAIL");
    if (!kat_ok) failures++;

    uint8_t c_digest[32];
    g3_hash_bytes(msg.data(), msg.size(), c_digest);
    const bool same_ok = (std::memcmp(digest.data(), c_digest, 32) == 0);
    std::printf("[%s] C++ interface matches the C interface\n", same_ok ? "PASS" : "FAIL");
    if (!same_ok) failures++;

    std::printf(failures ? "\n%d test(s) FAILED\n" : "\nAll tests passed\n", failures);
    return failures ? 1 : 0;
}
