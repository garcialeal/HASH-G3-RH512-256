# HASH-G3-RH512-256

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Standard](https://img.shields.io/badge/Language-C99%20%7C%20C%2B%2B17-blue.svg)]()
[![SMHasher3](https://img.shields.io/badge/SMHasher3-186%2F186%20Passed-brightgreen.svg)]()
[![Throughput](https://img.shields.io/badge/Throughput-13.6%20GiB%2Fs-success.svg)]()

A high-performance, non-cryptographic hash engine based on **Parallel Circular Feedback Architecture**. Engineered with a 512-bit state machine adaptable for both 256-bit and 512-bit output digests, `HASH-G3-RH512-256` delivers zero dynamic allocations, $O(1)$ spatial data identity verification, and production-grade collision resistance.

Originally designed for general-purpose high-speed hashing, this engine powers vertex deduplication, $O(1)$ geometric state tracking, and ultra-fast mesh deduplication within the `.v4s` 3D file format and V4S12 geometry ecosystem.

---

## Technical Highlights

* **Parallel Circular Feedback Architecture:** Utilizes an internal 512-bit circular shift register driving simultaneous feedback loops, natively generating either 256-bit or 512-bit output digests.
* **100% Quality Score in SMHasher3:** Verified across all **186/186** empirical hash quality tests in the SMHasher3 benchmark suite.
* **Extreme Throughput:** Achieves **13.6 GiB/s** processing speeds on standard x86_64 and ARM64 hardware.
* **Zero Allocations ($O(1)$ Memory):** Operates exclusively on stack frames and CPU registers. Zero heap usage (`malloc`/`new`), making it ideal for real-time rendering pipelines, WebAssembly (WASM), and embedded systems.
* **Dual Language Bindings:** Native ANSI C99 core implementation paired with modern header-only C++17 wrappers.

---

## Performance & Quality Metrics

| Metric | Specification |
| :--- | :--- |
| **Architecture** | Parallel Circular Feedback ($G_3$) |
| **Internal State** | 512-bit Circular Shift Register |
| **Digest Options** | Adaptable 256-bit / 512-bit |
| **SMHasher3 Score** | 186/186 Tests Passed (100%) |
| **Peak Throughput** | ~13.6 GiB/s |
| **Primary Application** | Geometric deduplication, $O(1)$ mesh hashing (`.v4s` format) |

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
#include <iostream>
#include <string_view>

int main() {
    constexpr std::string_view mesh_data = "MESH_VERTEX_ATTRIBUTES";

    /* Modern C++17 array output */
    v4s12::Hash256 digest = v4s12::compute_g3_hash(mesh_data.data(), mesh_data.size());

    std::cout << "256-bit Hash: ";
    for (uint8_t byte : digest) {
        std::cout << std::hex << static_cast<int>(byte);
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
# Clone and build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Run test suite
./test_g3
```

---

## License

This project is licensed under the permissive **MIT License**. See the [LICENSE](LICENSE) file for complete details. Free for open-source, commercial, and embedded engine usage.