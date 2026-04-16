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

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/LessUp/bitcal.git

# Copy headers to your project (header-only)
cp -r bitcal/include/bitcal /path/to/your/project/include/
```

### Compile

```bash
g++ -std=c++17 -O3 -march=native your_program.cpp -o your_program
```

### Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // Create 256-bit bit arrays
    bitcal::bit256 a(0xFF00FF00FF00FF00);
    bitcal::bit256 b(0x0FF00FF00FF00FF0);
    
    // Bitwise operations (SIMD-accelerated)
    auto c = a & b;          // AND
    auto d = a | b;          // OR  
    auto e = a ^ b;          // XOR
    auto f = ~a;             // NOT
    auto g = a.andnot(b);    // a & ~b (~2× faster than a & ~b)
    
    // Shifts
    a <<= 10;                // Left shift
    b >>= 5;                 // Right shift
    
    // Bit counting
    uint64_t ones = a.popcount();
    int lz = a.count_leading_zeros();
    int tz = a.count_trailing_zeros();
    
    // Single bit operations
    a.set_bit(42, true);
    bool bit = a.get_bit(42);
    a.flip_bit(42);
    a.reverse();
    
    std::cout << "Popcount: " << ones << std::endl;
    return 0;
}
```

## 📊 Performance

### Intel Core i7-12700K @ 3.6GHz (AVX2)

| Operation | Scalar | AVX2 | Speedup |
|-----------|--------|------|---------|
| AND-256 | 12.3 ns | 2.1 ns | **5.9×** |
| XOR-512 | 24.8 ns | 4.3 ns | **5.8×** |
| Shift Left-256 | 18.6 ns | 5.2 ns | **3.6×** |
| Popcount-512 | 45.2 ns | 22.3 ns | **2.0×** |
| is_zero-256 | 4.5 ns | 1.8 ns | **2.5×** |

### ARM Cortex-A72 @ 2.0GHz (NEON)

| Operation | Scalar | NEON | Speedup |
|-----------|--------|------|---------|
| AND-128 | 8.4 ns | 3.2 ns | **2.6×** |
| XOR-256 | 16.9 ns | 6.8 ns | **2.5×** |
| Shift Left-128 | 12.3 ns | 6.8 ns | **1.8×** |

## 📚 Documentation

### Getting Started
- [📖 Installation Guide](docs/en/getting-started/installation.md) — Setup and requirements
- [🚀 Quick Start](docs/en/getting-started/quickstart.md) — Your first BitCal program
- [⚙️ Build Options](docs/en/getting-started/build-options.md) — Compiler flags and optimization

### API Reference
- [📘 Types](docs/en/api/types.md) — `bitarray` template and type aliases
- [🔧 Core Operations](docs/en/api/core-operations.md) — AND, OR, XOR, NOT, ANDNOT
- [↔️ Shift Operations](docs/en/api/shift-operations.md) — Left and right shifts
- [️⃣ Bit Counting](docs/en/api/bit-counting.md) — popcount, CLZ, CTZ
- [🎛️ Bit Manipulation](docs/en/api/bit-manipulation.md) — get/set/flip bits, reverse
- [💻 SIMD Backend](docs/en/api/simd-backend.md) — Backend selection
- [🔩 Ops Namespace](docs/en/api/ops-namespace.md) — Low-level API

### Architecture
- [🏗️ Overview](docs/en/architecture/overview.md) — Design principles
- [⚡ SIMD Dispatch](docs/en/architecture/simd-dispatch.md) — Compile-time selection
- [🖥️ Platform Support](docs/en/architecture/platform-support.md) — Compatibility matrix

### Available Types

| Type | Width | Words | Best For |
|------|-------|-------|----------|
| `bitcal::bit64` | 64-bit | 1 | Machine word |
| `bitcal::bit128` | 128-bit | 2 | SSE2/NEON native |
| `bitcal::bit256` | 256-bit | 4 | AVX2 native |
| `bitcal::bit512` | 512-bit | 8 | Large parallel ops |
| `bitcal::bit1024` | 1024-bit | 16 | Very large ops |

Custom widths (multiple of 64):
```cpp
bitcal::bitarray<2048> custom;
```

## 🌍 Platform Support

| Platform | Architecture | Compilers | SIMD | CI Status |
|----------|--------------|-----------|------|-----------|
| Linux | x86-64 | GCC 7+, Clang 6+ | SSE2/AVX2 | ✅ Verified |
| Linux | ARM64 | GCC (cross) | NEON | ✅ Verified |
| Linux | ARM32 | GCC (cross) | NEON | ✅ Verified |
| Windows | x86-64 | MSVC 2017+ | SSE2/AVX2 | ✅ Verified |
| macOS | x86-64 | Apple Clang | SSE2/AVX2 | ✅ Verified |
| macOS | ARM64 (Apple Silicon) | Apple Clang | NEON | ✅ Verified |

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
│   └── neon_ops.hpp      # NEON implementation
├── docs/                 # Documentation
│   ├── en/               # English docs
│   └── zh/               # 中文文档
├── tests/                # Unit tests
├── benchmarks/           # Performance benchmarks
└── examples/             # Example programs
```

## 📦 Requirements

- **C++17** or later
- **CMake 3.16+** (for tests/benchmarks)
- **Supported Compilers**: GCC 7+, Clang 6+, MSVC 2017+

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
