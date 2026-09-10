/*
 * G3-RH512-256 Non-Cryptographic Spatial Hash Engine
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 * Licensed under the MIT License.
 */

#include "g3_hash.h"
#include <string.h>

static const uint64_t C[] = {
    0x9e3779b185ebca87ULL, 0xbb67ae8584caa73bULL,
    0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
    0x510e527fade682d1ULL, 0x9b05688c2b03e58aULL,
    0x1f12759bd39c024fULL, 0x5be0cd19137e2179ULL
};

static inline uint64_t rotl64(uint64_t x, int8_t r) {
    return (x << r) | (x >> (64 - r));
}

static inline uint64_t avalanche(uint64_t h) {
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= h >> 33;
    return h;
}

void g3_rh512_256(const void* key, const size_t len, const uint64_t seed, uint64_t out[4]) {
    const uint8_t* data = (const uint8_t*)key;
    const size_t nblocks = len / 32;

    uint64_t h[8];
    for (int i = 0; i < 4; ++i) h[i] = seed ^ C[i];
    for (int i = 4; i < 8; ++i) h[i] = (uint64_t)len ^ C[i];

    for (size_t i = 0; i < nblocks; ++i) {
        const uint64_t* block = (const uint64_t*)(data + i * 32);

        for (int j = 0; j < 4; ++j) {
            h[j] ^= block[j];
            h[j+4] += h[j];
        }

        uint64_t t[8];
        memcpy(t, h, sizeof(h));

        h[0] = rotl64(h[0] ^ t[7], 23) * C[0];
        h[1] = rotl64(h[1] ^ t[0], 19) * C[1];
        h[2] = rotl64(h[2] ^ t[1], 31) * C[2];
        h[3] = rotl64(h[3] ^ t[2], 13) * C[3];
        h[4] = rotl64(h[4] + t[3], 37) ^ C[4];
        h[5] = rotl64(h[5] + t[4], 41) ^ C[5];
        h[6] = rotl64(h[6] + t[5], 17) ^ C[6];
        h[7] = rotl64(h[7] + t[6], 29) ^ C[7];
    }

    const uint8_t* tail = data + nblocks * 32;
    size_t remaining = len & 31;
    if (remaining > 0) {
        uint64_t t_block[4] = {0, 0, 0, 0};
        memcpy(t_block, tail, remaining);
        ((uint8_t*)t_block)[remaining] = 0x80;

        for (int j = 0; j < 4; ++j) {
            uint64_t m = rotl64(t_block[j] * C[j], 15);
            h[j] ^= m;
            h[j+4] += m;
        }
    }

    for (int i = 0; i < 4; ++i) h[i] ^= rotl64(h[i+4], 27);
    h[0] ^= rotl64(h[2], 13); h[1] ^= rotl64(h[3], 13);
    h[4] ^= rotl64(h[6], 13); h[5] ^= rotl64(h[7], 13);
    for (int i = 0; i < 7; i += 2) h[i] += h[i+1];

    out[0] = avalanche(h[0] ^ h[4]);
    out[1] = avalanche(h[1] ^ h[5]);
    out[2] = avalanche(h[2] ^ h[6]);
    out[3] = avalanche(h[3] ^ h[7]);
}

void g3_hash_bytes(const void* data, size_t size, uint8_t out_256bit[32]) {
    uint64_t out[4] = {0};
    g3_rh512_256(data, size, 0, out);
    memcpy(out_256bit, out, sizeof(out));
}