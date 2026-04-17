# BitCal

<p align="center">
  <strong>Modern, Cross-Platform, SIMD-Accelerated Bit Manipulation Library for C++17</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://lessup.github.io/bitcal/"><img src="https://img.shields.io/badge/docs-Doxygen-blue.svg" alt="Docs"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License"></a>
  <a href="https://en.cppreference.com/w/cpp/17"><img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17"></a>
  <a href="#installation"><img src="https://img.shields.io/badge/header--only-yes-green.svg" alt="Header-only"></a>
  <a href="CHANGELOG.md"><img src="https://img.shields.io/badge/version-2.1.0-blue.svg" alt="Version"></a>
</p>

<p align="center">
  <strong>English</strong> | <a href="README.zh-CN.md">简体中文</a>
</p>

---

## Overview

**BitCal** is a modern, high-performance C++17 header-only library for bit manipulation operations with automatic SIMD acceleration. Leveraging compile-time dispatch via `if constexpr`, BitCal delivers up to **6× performance improvement** over scalar implementations while maintaining zero runtime overhead.

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

| Feature | Description | Performance Impact |
|---------|-------------|-------------------|
| 🚀 **SIMD Acceleration** | Automatic SSE2/AVX2 (x86) or NEON (ARM) selection | Up to 6× faster |
| ⚡ **Zero Overhead** | Compile-time dispatch with `if constexpr` | No runtime cost |
| 📦 **Header-Only** | Single `#include <bitcal/bitcal.hpp>` | Zero dependencies |
| 🔧 **Rich API** | Bitwise ops, shifts, popcount, CLZ/CTZ, bit reversal, ANDNOT | Production-ready |
| 🌍 **Cross-Platform** | Linux, Windows, macOS on x86-64 and ARM | Universal support |
| 🏎️ **Type Safety** | Compile-time bit-width validation | Catch errors early |

<details>
<summary>📋 Table of Contents</summary>

- [Installation](#-installation)
- [Quick Start](#-quick-start)
- [API Overview](#-api-overview)
- [Use Cases](#-use-cases)
- [Performance](#-performance)
- [Documentation](#-documentation)
- [Platform Support](#-platform-support)
- [Build Instructions](#-build-instructions)
- [Project Structure](#-project-structure)
</details>

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
    GIT_TAG v2.1.0
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
    uint64_t* data = a.data();   // 64-byte aligned pointer
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
| `bitcal::bit1024` | 1024-bit | 16 | Very large bitsets |

Custom widths (multiple of 64):
```cpp
bitcal::bitarray<2048> custom;
```

## 📖 API Overview

### Constructors
```cpp
bitcal::bit256 a;                    // Zero-initialized
bitcal::bit256 b(0xDEADBEEF);        // Set lower 64 bits
bitcal::bit256 c = b;                // Copy
```

### Bitwise Operators
```cpp
a & b    // AND        a &= b    // AND-assign
a | b    // OR         a |= b    // OR-assign
a ^ b    // XOR        a ^= b    // XOR-assign
~a       // NOT
a.andnot(b)  // Optimized a & ~b
```

### Shift Operators
```cpp
a << n   // Left shift      a <<= n   // Left shift-assign
a >> n   // Right shift     a >>= n   // Right shift-assign
```

### Bit Counting
```cpp
a.popcount()              // Count set bits
a.count_leading_zeros()   // Count leading zeros
a.count_trailing_zeros()  // Count trailing zeros
```

### Single Bit Operations
```cpp
a.get_bit(index)          // Read bit (0-indexed from LSB)
a.set_bit(index, value)   // Set/clear bit
a.flip_bit(index)         // Toggle bit
```

### State & Comparison
```cpp
a.is_zero()               // Check if all bits are zero
a.clear()                 // Set all bits to zero
a == b                    // Equality comparison
a != b                    // Inequality comparison
```

### Data Access
```cpp
a.data()                  // Direct pointer to 64-bit words
a[index]                  // Access 64-bit word at index
a.bits                    // Compile-time bit count (static)
a.u64_count               // Number of 64-bit words (static)
```

### Bit Manipulation
```cpp
a.reverse()               // Reverse bit order
```

### Force Specific Backend
```cpp
bitcal::bitarray<256, bitcal::simd_backend::avx2>  force_avx2;
bitcal::bitarray<256, bitcal::simd_backend::sse2>  force_sse2;
bitcal::bitarray<256, bitcal::simd_backend::neon>  force_neon;
bitcal::bitarray<256, bitcal::simd_backend::scalar> force_scalar;
```

## 💡 Use Cases

BitCal excels in scenarios requiring high-performance bit manipulation:

| Use Case | Description | Benefit |
|----------|-------------|---------|
| **Bitsets** | Compact boolean storage | 64× space reduction vs `bool[]` |
| **Bloom Filters** | Probabilistic data structures | SIMD-accelerated hash mixing |
| **Network Masks** | CIDR/subnet calculations | Fast AND/OR on large masks |
| **Cryptography** | Block cipher bit ops | Hardware popcount for Hamming distance |
| **Data Compression** | Bit packing/unpacking | Efficient cross-word shifts |
| **Graphics** | Mask operations | Parallel processing of pixel masks |

See [`examples/`](examples/) for concrete implementations.

```cpp
// Example: Fast CIDR mask generation
bitcal::bit256 make_mask(int prefix_len) {
    bitcal::bit256 mask;
    mask.clear();
    for (int i = 0; i < prefix_len; ++i) {
        mask.set_bit(255 - i, true);
    }
    return mask;
}
```

## 📊 Performance

### Compiler Flags

For optimal performance, use these compiler flags:

```bash
# GCC/Clang
g++ -std=c++17 -O3 -march=native -DNDEBUG

# MSVC
cl /std:c++17 /O2 /arch:AVX2 /DNDEBUG
```

| Flag | Purpose |
|------|---------|
| `-O3` | Maximum optimization |
| `-march=native` | Enable all CPU features |
| `-DNDEBUG` | Remove assertions in release |

### Benchmark Results

#### Intel Core i7-12700K @ 3.6GHz (AVX2)

| Operation | Scalar | AVX2 | Speedup |
|-----------|--------|------|---------|
| AND-256 | 12.3 ns | 2.1 ns | **5.9×** |
| XOR-512 | 24.8 ns | 4.3 ns | **5.8×** |
| Shift Left-256 | 18.6 ns | 5.2 ns | **3.6×** |
| Popcount-512 | 45.2 ns | 22.3 ns | **2.0×** |
| is_zero-256 | 4.5 ns | 1.8 ns | **2.5×** |

#### ARM Cortex-A72 @ 2.0GHz (NEON)

| Operation | Scalar | NEON | Speedup |
|-----------|--------|------|---------|
| AND-128 | 8.4 ns | 3.2 ns | **2.6×** |
| XOR-256 | 16.9 ns | 6.8 ns | **2.5×** |
| Shift Left-128 | 12.3 ns | 6.8 ns | **1.8×** |

> 📈 Run `./benchmarks/bench_bitcal` in your environment to see actual numbers.

### SIMD Backend Selection

BitCal automatically selects the best backend based on compiler flags:

| Platform | Backend | Width | Condition |
|----------|---------|-------|-----------|
| x86-64 | AVX2 | 256-bit | `-mavx2` available |
| x86-64 | SSE2 | 128-bit | `-msse2` available |
| ARM | NEON | 128-bit | ARMv7-A+ or ARM64 |
| Any | Scalar | 64-bit | Fallback |

## 📚 Documentation

### API Reference

| Topic | Description |
|-------|-------------|
| [Types](docs/en/api/types.md) | `bitarray` template and type aliases |
| [Core Operations](docs/en/api/core-operations.md) | AND, OR, XOR, NOT, ANDNOT |
| [Shift Operations](docs/en/api/shift-operations.md) | Left and right shifts |
| [Bit Counting](docs/en/api/bit-counting.md) | popcount, CLZ, CTZ |
| [Bit Manipulation](docs/en/api/bit-manipulation.md) | get/set/flip bits, reverse |
| [SIMD Backend](docs/en/api/simd-backend.md) | Backend selection |
| [Ops Namespace](docs/en/api/ops-namespace.md) | Low-level API |

### Architecture

- [Overview](docs/en/architecture/overview.md) - Design principles
- [SIMD Dispatch](docs/en/architecture/simd-dispatch.md) - Compile-time selection
- [Platform Support](docs/en/architecture/platform-support.md) - Compatibility matrix

### Getting Started

- [Installation Guide](docs/en/getting-started/installation.md)
- [Quick Start](docs/en/getting-started/quickstart.md)
- [Build Options](docs/en/getting-started/build-options.md)

Full documentation: [https://lessup.github.io/bitcal/](https://lessup.github.io/bitcal/)

## 🌍 Platform Support

| Platform | Architecture | Compilers | SIMD | CI Status |
|----------|--------------|-----------|------|-----------|
| Linux | x86-64 | GCC 7+, Clang 6+ | SSE2/AVX2 | ✅ Verified |
| Linux | ARM64 | GCC (cross) | NEON | ✅ Verified |
| Linux | ARM32 | GCC (cross) | NEON | ✅ Verified |
| Windows | x86-64 | MSVC 2017+ | SSE2/AVX2 | ✅ Verified |
| macOS | x86-64 | Apple Clang | SSE2/AVX2 | ✅ Verified |
| macOS | ARM64 (Apple Silicon) | Apple Clang | NEON | ✅ Verified |

### Requirements

- **C++17** or later
- **CMake 3.16+** (for building tests/benchmarks)
- **Supported Compilers**: GCC 7+, Clang 6+, MSVC 2017+

## 🔨 Build Instructions

### Build Tests and Benchmarks

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests
./tests/test_bitcal

# Run benchmarks
./benchmarks/bench_bitcal
```

### Install System-Wide

```bash
cmake --install . --prefix /usr/local
```

## 🏗️ Project Structure

```
bitcal/
├── include/bitcal/       # Header files (header-only library)
│   ├── bitcal.hpp        # Main header (include this)
│   ├── config.hpp        # Platform detection
│   ├── simd_traits.hpp   # SIMD type traits
│   ├── scalar_ops.hpp    # Scalar fallback
│   ├── sse_ops.hpp       # SSE2 implementation
│   ├── avx_ops.hpp       # AVX2 implementation
│   ├── avx512_ops.hpp    # AVX-512 support
│   └── neon_ops.hpp      # NEON implementation
├── docs/                 # Documentation
│   ├── en/               # English docs
│   └── zh/               # 中文文档
├── tests/                # Unit tests
├── benchmarks/           # Performance benchmarks
└── examples/             # Example programs
```

### Memory Layout

```
bitarray<256> memory layout:
┌──────────────────────────────────────────────────────────┐
│ Alignment │ Word 0 │ Word 1 │ Word 2 │ Word 3 │ Padding │
│ 64 bytes  │ 0-63   │ 64-127 │128-191 │192-255 │ to 64B  │
└──────────────────────────────────────────────────────────┘
```

- **Alignment:** 64 bytes (cache-line aligned)
- **Endianness:** Little-endian (LSB in `data()[0]`)
- **Storage:** Contiguous `uint64_t` array

## 📝 Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history.

### Latest Release: v2.1.0 (2026-04-16)

- ✨ **New:** ANDNOT operation with native SIMD instructions
- ⚡ **Performance:** Up to 2.3× faster `is_zero()`, 1.7× faster `~`
- 🧪 **Testing:** Full bit1024 test coverage
- 🔧 **Infrastructure:** ARM32 CI support

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.

## 🙏 Acknowledgments

- Design inspired by [Boost.SIMD](https://github.com/boostorg/simd) and [xsimd](https://github.com/xtensor-stack/xsimd)
- Performance testing with [Google Benchmark](https://github.com/google/benchmark)

---

<p align="center">
  <a href="https://github.com/LessUp/bitcal">⭐ Star on GitHub</a> •
  <a href="https://github.com/LessUp/bitcal/issues">🐛 Report Issue</a> •
  <a href="https://github.com/LessUp/bitcal/discussions">💬 Discussions</a>
</p>
