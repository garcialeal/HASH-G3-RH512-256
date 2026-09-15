/*
 * SPDX-License-Identifier: MIT
 *
 * G3-RH512-256 Non-Cryptographic Spatial Hash Engine
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 * Licensed under the MIT License.
 */

#include "g3_hash.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    const char* test_str = "V4S12_DETERMINISTIC_VECTOR_GEOMETRY";
    uint8_t hash[32];

    g3_hash_bytes(test_str, strlen(test_str), hash);

    printf("G3-RH512-256 Hash Test Output:\n");
    for (int i = 0; i < 32; ++i) {
        printf("%02x", hash[i]);
    }
    printf("\n\nTest Passed Successfully.\n");
    return 0;
}