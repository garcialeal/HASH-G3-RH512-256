/*
 * G3-RH512-256 Non-Cryptographic Spatial Hash Engine
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

/* Core C99 API */
void g3_rh512_256(const void* key, size_t len, uint64_t seed, uint64_t out[4]);
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