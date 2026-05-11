# BitCal

<p align="center">
  <strong>Header-Only C++17 Bit Manipulation Library with Compile-Time SIMD Dispatch</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://lessup.github.io/bitcal/"><img src="https://img.shields.io/badge/docs-GitHub_Pages-blue.svg" alt="Docs"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License"></a>
  <a href="https://en.cppreference.com/w/cpp/17"><img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17"></a>
  <a href="#-installation"><img src="https://img.shields.io/badge/header--only-yes-green.svg" alt="Header-only"></a>
  <a href="CHANGELOG.md"><img src="https://img.shields.io/badge/version-3.0.0-blue.svg" alt="Version"></a>
</p>

<p align="center">
  <strong>English</strong> | <a href="README.zh-CN.md">简体中文</a>
</p>

---

## Overview

**BitCal** is a C++17 header-only library for bit manipulation with compile-time SIMD dispatch. It provides the retained `<bitcal/bitcal.hpp>` public surface with `bitarray` template, automatic backend selection (SSE2/AVX2/NEON/scalar), and portable fallback. BitCal 3.0 is in stable maintenance with an archive-ready posture.

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;           // SIMD-accelerated AND (~2.1ns)
    auto pop = c.popcount();  // Hardware popcount

    return 0;
}
```

## ✨ Features

- **Header-Only**: Single `#include <bitcal/bitcal.hpp>` with zero external dependencies
- **Compile-Time SIMD Dispatch**: `if constexpr` selects SSE2/AVX2 (x86), NEON (ARM), or portable scalar fallback at compile time
- **Retained Public Surface**: `bitarray<Bits>` template with member functions for all operations
- **Rich API**: Bitwise ops, shifts, popcount, CLZ/CTZ, bit reversal, ANDNOT, single-bit manipulation
- **Cross-Platform**: Linux, Windows, macOS on x86-64 and ARM
- **Type-Safe**: Compile-time bit-width validation (`Bits % 64 == 0`)
- **Stable Maintenance**: 3.0 contract is frozen; archive-ready posture

## ⚠️ Breaking Changes in 3.0.0

BitCal 3.0.0 contracts the public API to the retained `bitarray` surface:

- Removed public `bitcal::ops` raw-pointer helpers
- Removed public traits: `is_bitarray`, `is_bitarray_v`, `bitarray_traits`
- Removed explicit `bit64` conversion convenience from the public contract
- Migration: Use `bitarray` member functions; adapt existing `uint64_t` buffers via `word()` / `set_word()`

See [CHANGELOG.md](CHANGELOG.md) and the [API reference](https://lessup.github.io/bitcal/) for full migration details.

## 🚀 Installation

### Option 1: Copy Headers (Simplest)

```bash
# Clone the repository
git clone https://github.com/LessUp/bitcal.git

# Copy headers to your project (header-only)
cp -r bitcal/include/bitcal /path/to/your/project/include/
```

### Option 2: CMake FetchContent (Recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
    bitcal
    GIT_REPOSITORY https://github.com/LessUp/bitcal.git
    GIT_TAG v3.0.0
)
FetchContent_MakeAvailable(bitcal)

target_link_libraries(your_target PRIVATE bitcal::bitcal)
```

### Option 3: CMake Install

```bash
# Clone and install
git clone https://github.com/LessUp/bitcal.git
 cd bitcal
 mkdir build && cd build
 cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
 cmake --build .
 cmake --install .
```

Then in your `CMakeLists.txt`:

```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target PRIVATE bitcal::bitcal)
```

## 🚦 Quick Start

### Basic Compilation

```bash
g++ -std=c++17 -O3 -march=native your_program.cpp -o your_program
```

> 💡 **Tip:** Use `-march=native` to enable all SIMD instructions available on your CPU.

### Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // Create bit arrays with different widths
    bitcal::bit256 a(0xFF00FF00FF00FF00);
    bitcal::bit256 b(0x0FF00FF00FF00FF0);

    // Bitwise operations (automatically SIMD-accelerated)
    auto c = a & b;              // AND
    auto d = a | b;              // OR
    auto e = a ^ b;              // XOR
    auto f = ~a;                 // NOT (accelerated via SIMD)
    auto g = a.andnot(b);        // a & ~b (~2× faster than separate ops)

    // Compound assignments
    a &= b;                      // AND in-place
    b |= a;                      // OR in-place

    // Shifts
    a <<= 10;                    // Left shift
    b >>= 5;                     // Right shift

    // Bit counting operations
    uint64_t ones = a.popcount();
    int lz = a.count_leading_zeros();   // CLZ
    int tz = a.count_trailing_zeros();  // CTZ

    // Single bit operations
    a.set_bit(42, true);         // Set bit 42
    bool bit = a.get_bit(42);    // Read bit 42
    a.flip_bit(42);              // Toggle bit 42

    // Bit manipulation
    a.reverse();                 // Reverse all bits
    a.clear();                   // Set all bits to zero

    // Direct word access (for interop with C APIs)
    const uint64_t* data = a.data();   // Read-only aligned pointer
    uint64_t word = a[0];        // Access first 64-bit word

    // Comparisons
    if (a == b) { /* ... */ }
    if (a != b) { /* ... */ }
    if (a.is_zero()) { /* all bits are zero */ }

    std::cout << "Popcount: " << ones << std::endl;
    return 0;
}
```

### Supported Types

| Type | Width | Words | Best For |
|------|-------|-------|----------|
| `bitcal::bit64` | 64-bit | 1 | Machine word ops |
| `bitcal::bit128` | 128-bit | 2 | SSE2/NEON native |
| `bitcal::bit256` | 256-bit | 4 | AVX2 native |
| `bitcal::bit512` | 512-bit | 8 | Large parallel ops |
| `bitcal::bit1024` | 1024-bit | 16 | Very large parallel ops |

Custom widths (multiple of 64):
```cpp
bitcal::bitarray<2048> custom;
```

## 📚 Documentation & API

The `bitarray<Bits>` template provides bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`), bit counting (`popcount`, CLZ/CTZ), single-bit manipulation (`get_bit`, `set_bit`, `flip_bit`), and direct word access. See the [API reference](https://lessup.github.io/bitcal/en/api/) for full details.

Complete documentation at **[https://lessup.github.io/bitcal/](https://lessup.github.io/bitcal/)** includes architecture, performance benchmarking, and platform-specific notes.

## 🌍 Platform Support

**Linux**, **Windows**, **macOS** on x86-64 and ARM with C++17 or later. See [CHANGELOG.md](CHANGELOG.md) for version history and [platform docs](https://lessup.github.io/bitcal/en/architecture/platform-support.html) for CI validation details.

## 🤝 Contributing

Contributions are welcome within the 3.0 contract boundary. For significant changes, open an issue first. See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.
