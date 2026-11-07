/*
 * SPDX-License-Identifier: MIT
 *
 * RH512 non-cryptographic hash (G3-RH512-256 library)
 *
 * Copyright (C) 2026 Antonio García Leal (garcialeal)
 * Licensed under the MIT License.
 *
 * This is the RH512 algorithm exactly as tested in SMHasher3
 * (hashes/g3-rh512-256.cpp, upstream commit c378d84).
 * tests/test_g3.c checks the official SMHasher3 verification
 * value (0xD5E60849), so any change to the algorithm is detected.
 *
 * Input words are always read as little-endian and output words are
 * always written as little-endian, so the digest is identical on every
 * platform, independent of CPU byte order and memory alignment.
 */

#include "g3_hash.h"
#include <string.h>

static const uint64_t C0 = UINT64_C(0xff51afd7ed558ccd);
static const uint64_t C1 = UINT64_C(0xc4ceb9fe1a85ec53);
static const uint64_t C2 = UINT64_C(0x9e3779b97f4a7c15);
static const uint64_t C3 = UINT64_C(0x517cc1b727220a95);
static const uint64_t C4 = UINT64_C(0xa0761d6478bd642f);
static const uint64_t C5 = UINT64_C(0xe7037ed1a0b428db);
static const uint64_t C6 = UINT64_C(0x8ebc6af09c88c6e3);
static const uint64_t C7 = UINT64_C(0x589965cc75374cc3);

static inline uint64_t rotl64(uint64_t x, unsigned r) {
    return (x << r) | (x >> (64 - r));
}

/* Little-endian load, byte by byte: no alignment or byte-order assumptions. */
static inline uint64_t load64_le(const uint8_t* p) {
    return  (uint64_t)p[0]        | ((uint64_t)p[1] <<  8) |
           ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

static inline void store64_le(uint64_t v, uint8_t* p) {
    for (int i = 0; i < 8; ++i) p[i] = (uint8_t)(v >> (8 * i));
}

static inline uint64_t avalanche(uint64_t x) {
    x ^= x >> 33;
    x *= UINT64_C(0xff51afd7ed558ccd);
    x ^= x >> 33;
    x *= UINT64_C(0xc4ceb9fe1a85ec53);
    x ^= x >> 33;
    return x;
}

/* Computes the full 512-bit final state h[0..7]. */
static void rh512_state(const void* key, size_t len, uint64_t seed, uint64_t h[8]) {
    const uint8_t* data = (const uint8_t*)key;

    uint64_t h0 = seed ^ C0, h1 = seed ^ C1, h2 = seed ^ C2, h3 = seed ^ C3;
    uint64_t h4 = (uint64_t)len ^ C4, h5 = (uint64_t)len ^ C5;
    uint64_t h6 = (uint64_t)len ^ C6, h7 = (uint64_t)len ^ C7;

    size_t i = 0;

    /* Main loop: 32-byte blocks */
    if (len >= 32) {
        for (; i <= len - 32; i += 32) {
            uint64_t k0 = load64_le(data + i);
            uint64_t k1 = load64_le(data + i + 8);
            uint64_t k2 = load64_le(data + i + 16);
            uint64_t k3 = load64_le(data + i + 24);

            h0 ^= k0 * C0; h0 = rotl64(h0, 23); h4 += h0; h4 = rotl64(h4, 31);
            h1 ^= k1 * C1; h1 = rotl64(h1, 24); h5 += h1; h5 = rotl64(h5, 31);
            h2 ^= k2 * C2; h2 = rotl64(h2, 25); h6 += h2; h6 = rotl64(h6, 31);
            h3 ^= k3 * C3; h3 = rotl64(h3, 26); h7 += h3; h7 = rotl64(h7, 31);

            /* Ring feedback between lanes */
            uint64_t t0 = h0, t1 = h1, t2 = h2, t3 = h3;
            uint64_t t4 = h4, t5 = h5, t6 = h6, t7 = h7;

            h0 ^= rotl64(t7, 11); h4 += t3;
            h1 ^= rotl64(t4, 11); h5 += t0;
            h2 ^= rotl64(t5, 11); h6 += t1;
            h3 ^= rotl64(t6, 11); h7 += t2;
        }
    }

    /* Tail (0..31 bytes) with 0x80 stop byte */
    if (i < len) {
        uint8_t tail[32] = {0};
        memcpy(tail, data + i, len - i);
        tail[len - i] = 0x80;

        uint64_t tk0 = load64_le(tail);
        uint64_t tk1 = load64_le(tail + 8);
        uint64_t tk2 = load64_le(tail + 16);
        uint64_t tk3 = load64_le(tail + 24);

        tk0 *= C4; tk0 = rotl64(tk0, 31); tk0 *= C0;
        tk1 *= C5; tk1 = rotl64(tk1, 31); tk1 *= C1;
        tk2 *= C6; tk2 = rotl64(tk2, 31); tk2 *= C2;
        tk3 *= C7; tk3 = rotl64(tk3, 31); tk3 *= C3;

        h0 ^= tk0; h4 ^= tk0;
        h1 ^= tk1; h5 ^= tk1;
        h2 ^= tk2; h6 ^= tk2;
        h3 ^= tk3; h7 ^= tk3;
    }

    /* Length seal */
    uint64_t flen = (uint64_t)len * C6;
    flen = rotl64(flen, 27) * C7;
    h0 ^= flen; h4 ^= flen;
    h7 ^= flen; h3 ^= flen;

    /* Butterfly finalizer, stage 1: distance 4 */
    h0 ^= rotl64(h4, 29); h0 *= C0; h4 += h0; h4 = rotl64(h4, 13);
    h1 ^= rotl64(h5, 29); h1 *= C1; h5 += h1; h5 = rotl64(h5, 13);
    h2 ^= rotl64(h6, 29); h2 *= C2; h6 += h2; h6 = rotl64(h6, 13);
    h3 ^= rotl64(h7, 29); h3 *= C3; h7 += h3; h7 = rotl64(h7, 13);

    /* Stage 2: distance 2 */
    h0 ^= rotl64(h2, 29); h0 *= C4; h2 += h0; h2 = rotl64(h2, 13);
    h1 ^= rotl64(h3, 29); h1 *= C5; h3 += h1; h3 = rotl64(h3, 13);
    h4 ^= rotl64(h6, 29); h4 *= C6; h6 += h4; h6 = rotl64(h6, 13);
    h5 ^= rotl64(h7, 29); h5 *= C7; h7 += h5; h7 = rotl64(h7, 13);

    /* Stage 3: distance 1 */
    h0 ^= rotl64(h1, 29); h0 *= C0; h1 += h0; h1 = rotl64(h1, 13);
    h2 ^= rotl64(h3, 29); h2 *= C1; h3 += h2; h3 = rotl64(h3, 13);
    h4 ^= rotl64(h5, 29); h4 *= C2; h5 += h4; h5 = rotl64(h5, 13);
    h6 ^= rotl64(h7, 29); h6 *= C3; h7 += h6; h7 = rotl64(h7, 13);

    h[0] = avalanche(h0); h[1] = avalanche(h1);
    h[2] = avalanche(h2); h[3] = avalanche(h3);
    h[4] = avalanche(h4); h[5] = avalanche(h5);
    h[6] = avalanche(h6); h[7] = avalanche(h7);
}

void g3_rh512_256(const void* key, size_t len, uint64_t seed, uint64_t out[4]) {
    uint64_t h[8];
    rh512_state(key, len, seed, h);
    out[0] = h[0]; out[1] = h[1]; out[2] = h[2]; out[3] = h[3];
}

void g3_rh512_256_bytes(const void* key, size_t len, uint64_t seed, uint8_t out[32]) {
    uint64_t h[8];
    rh512_state(key, len, seed, h);
    for (int i = 0; i < 4; ++i) store64_le(h[i], out + 8 * i);
}

void g3_hash_bytes(const void* data, size_t size, uint8_t out_256bit[32]) {
    g3_rh512_256_bytes(data, size, 0, out_256bit);
}
