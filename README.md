# HASH-G3-RH512-256

[![CI](https://github.com/garcialeal/HASH-G3-RH512-256/actions/workflows/ci.yml/badge.svg)](https://github.com/garcialeal/HASH-G3-RH512-256/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Standard](https://img.shields.io/badge/Language-C99%20%7C%20C%2B%2B17-blue.svg)]()
[![SMHasher3](https://img.shields.io/badge/SMHasher3-250%2F250%20passed-brightgreen.svg)](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/raw/RH512.txt)

A portable C99 implementation of **RH512**, a fast non-cryptographic hash
function with a 512-bit internal state (eight 64-bit lanes) and a 256-bit
digest. It uses no heap allocation and produces the same digest on every
platform, independent of CPU byte order and memory alignment.

It is used for vertex and mesh deduplication in the `.v4s` 3D file format.

> **Not cryptographic.** RH512 is designed for hash tables, deduplication and
> checksums of trusted data. It must not be used where an attacker could craft
> inputs on purpose (integrity against tampering, signatures, passwords).
> Use a cryptographic hash such as BLAKE3 or SHA-256 for that.

---

## SMHasher3 status

RH512 is part of the upstream [SMHasher3](https://gitlab.com/fwojcik/smhasher3)
test suite, integrated by its maintainer Frank J. T. Wojcik:

* Added in commit `c378d84` ("Add RH512 hash"), source file
  `hashes/g3-rh512-256.cpp`.
* Results added in commit `573909f`; summary table updated in commit `3b61937`.

| SMHasher3 variant | Output | Result | Verification value (LE) |
| :--- | :--- | :--- | :--- |
| [RH512](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/raw/RH512.txt) | low 256 bits | 250 / 250 passed | `0xD5E60849` |
| [RH512h](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/raw/RH512h.txt) | high 256 bits | 250 / 250 passed | `0xE42C2BF3` |
| [RH512-64](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/raw/RH512-64.txt) | low 64 bits | 250 / 250 passed | `0x3C481EA3` |

This library implements the **RH512** variant (low 256 bits). The test suite
(`tests/test_g3.c`) recomputes the SMHasher3 verification value and fails unless
it equals `0xD5E60849`, which guarantees that the code in this repository is
bit-for-bit the function that SMHasher3 tested.

Passing SMHasher3 measures statistical quality (distribution, avalanche,
collisions on structured keys). It does not measure resistance to deliberately
constructed collisions.

---

## Performance

Figures from the official SMHasher3 results (single thread, nominal 3.5 GHz):

| Metric | Value |
| :--- | :--- |
| Bulk speed (256 KiB keys) | ~2.5 bytes/cycle (~8.2 GiB/s) |
| Small keys (1-32 bytes) | ~80 cycles/hash on average |

Speed depends on the CPU and compiler; measure on your own hardware.

---

## Integration Guide

### C99 Usage

```c
#include "g3_hash.h"
#include <stdio.h>

int main(void) {
    const char vertex_buffer[] = "VERTEX_XYZ_3D_DATA_BLOCK";
    uint8_t hash_256[32];

    /* Compute 256-bit hash */
    g3_hash_bytes(vertex_buffer, sizeof(vertex_buffer) - 1, hash_256);

    printf("256-bit Hash Digest: ");
    for (int i = 0; i < 32; ++i) {
        printf("%02x", hash_256[i]);
    }
    printf("\n");

    return 0;
}
```

### C++17 Usage

```cpp
#include "g3_hash.h"
#include <iomanip>
#include <iostream>
#include <string_view>

int main() {
    constexpr std::string_view mesh_data = "MESH_VERTEX_ATTRIBUTES";

    /* Modern C++17 array output */
    v4s12::Hash256 digest = v4s12::compute_g3_hash(mesh_data.data(), mesh_data.size());

    std::cout << "256-bit Hash: ";
    for (uint8_t byte : digest) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(byte);
    }
    std::cout << std::endl;

    return 0;
}
```

---

## Building and Verification

### Prerequisites

* C99 compatible C compiler (GCC 9+, Clang 10+, MSVC 2019+)
* C++17 compatible C++ compiler
* CMake 3.15+

### Build Steps

```bash
# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Run test suite (checks the SMHasher3 verification value)
ctest --test-dir build -C Release --output-on-failure
```

When this library is added to another CMake project with `add_subdirectory`,
the tests are not built unless `-DG3_HASH_BUILD_TESTS=ON` is given.

### Continuous integration

Every push and pull request is built and tested on Linux (GCC and Clang),
macOS (ARM64), Windows (MSVC), a big-endian s390x system emulated with QEMU,
and with AddressSanitizer and UndefinedBehaviorSanitizer. All jobs must
reproduce the SMHasher3 verification value `0xD5E60849`.

---

## License

This project is licensed under the permissive **MIT License**. See the [LICENSE](LICENSE) file for complete details. Free for open-source, commercial, and embedded engine usage.
