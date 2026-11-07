/*
 * SPDX-License-Identifier: MIT
 *
 * RH512 non-cryptographic hash (G3-RH512-256 library)
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 * Licensed under the MIT License.
 */

#ifndef G3_HASH_H
#define G3_HASH_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Core C99 API -- RH512, low 256 bits (SMHasher3 "RH512").
 * Not a cryptographic hash: do not use it against adversarial input. */

/* Digest as four 64-bit words (numeric values are platform independent). */
void g3_rh512_256(const void* key, size_t len, uint64_t seed, uint64_t out[4]);

/* Digest as 32 bytes, little-endian, with an explicit seed. */
void g3_rh512_256_bytes(const void* key, size_t len, uint64_t seed, uint8_t out[32]);

/* Same as g3_rh512_256_bytes with seed 0. */
void g3_hash_bytes(const void* data, size_t size, uint8_t out_256bit[32]);

#ifdef __cplusplus
}

/* Optional C++ Interface for v4s12 ecosystem */
#include <array>

namespace v4s12 {
    using Hash256 = std::array<uint8_t, 32>;

    inline Hash256 compute_g3_hash(const void* data, size_t size) {
        Hash256 result{};
        g3_hash_bytes(data, size, result.data());
        return result;
    }
}
#endif

#endif /* G3_HASH_H */
