/*
 * SPDX-License-Identifier: MIT
 *
 * RH512 non-cryptographic hash (G3-RH512-256 library) - test suite
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 * Licensed under the MIT License.
 */

#include "g3_hash.h"
#include <stdio.h>
#include <string.h>

static int failures = 0;

static void check(int ok, const char* what) {
    printf("[%s] %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) failures++;
}

/* SMHasher3 verification value: hash keys {0}, {0,1}, ... of length
 * 0..255 with seed 256-len, concatenate the 256 digests, hash that with
 * seed 0, and read the first 4 bytes as a little-endian integer. */
static uint32_t smhasher3_verification(void) {
    uint8_t key[256];
    uint8_t digests[256 * 32];
    uint8_t final_digest[32];

    for (int i = 0; i < 256; ++i) {
        key[i] = (uint8_t)i;
        g3_rh512_256_bytes(key, (size_t)i, (uint64_t)(256 - i), &digests[i * 32]);
    }
    g3_rh512_256_bytes(digests, sizeof(digests), 0, final_digest);

    return  (uint32_t)final_digest[0]        | ((uint32_t)final_digest[1] << 8) |
           ((uint32_t)final_digest[2] << 16) | ((uint32_t)final_digest[3] << 24);
}

int main(void) {
    /* 1. Identical to the function tested in SMHasher3 (RH512, 0xD5E60849). */
    uint32_t v = smhasher3_verification();
    printf("SMHasher3 verification value: 0x%08X (expected 0xD5E60849)\n", v);
    check(v == 0xD5E60849u, "matches the official SMHasher3 RH512 verification value");

    /* 2. Known-answer test for a fixed string. */
    static const uint8_t expected[32] = {
        0x79, 0xb6, 0xae, 0xf1, 0x25, 0x8e, 0xbb, 0xca,
        0x99, 0xfb, 0x49, 0xc1, 0xfa, 0x73, 0xe1, 0xf8,
        0x6d, 0x49, 0x44, 0x69, 0xd8, 0xd4, 0x74, 0xca,
        0x3c, 0x9f, 0xb3, 0x17, 0x04, 0xab, 0xc9, 0x56
    };
    const char* msg = "V4S12_DETERMINISTIC_VECTOR_GEOMETRY";
    uint8_t h[32];
    g3_hash_bytes(msg, strlen(msg), h);
    check(memcmp(h, expected, 32) == 0, "known-answer test (fixed string)");

    /* 3. Result does not depend on memory alignment of the input. */
    uint8_t buf[200 + 8];
    for (int i = 0; i < 200 + 8; ++i) buf[i] = (uint8_t)(i * 37 + 11);
    uint8_t ref[32], shifted[32];
    uint8_t moved[200 + 8];
    g3_hash_bytes(buf, 200, ref);
    int same = 1;
    for (int off = 1; off < 8; ++off) {
        memmove(moved + off, buf, 200);
        g3_hash_bytes(moved + off, 200, shifted);
        if (memcmp(ref, shifted, 32) != 0) same = 0;
    }
    check(same, "independent of input alignment");

    /* 4. Word API and byte API agree. */
    uint64_t w[4];
    g3_rh512_256(msg, strlen(msg), 0, w);
    int agree = 1;
    for (int i = 0; i < 4; ++i)
        for (int b = 0; b < 8; ++b)
            if ((uint8_t)(w[i] >> (8 * b)) != h[8 * i + b]) agree = 0;
    check(agree, "word output and byte output are consistent");

    printf(failures ? "\n%d test(s) FAILED\n" : "\nAll tests passed\n", failures);
    return failures ? 1 : 0;
}
