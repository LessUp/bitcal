# BitCal

[![CI](https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/bitcal/actions/workflows/ci.yml)
[![Docs](https://github.com/LessUp/bitcal/actions/workflows/pages.yml/badge.svg)](https://lessup.github.io/bitcal/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)](#)

**A modern, cross-platform, high-performance bit manipulation library with SIMD acceleration.**

English | [简体中文](README.zh-CN.md)

---

## Features

- 🚀 **SIMD Acceleration** — Automatic selection of SSE2/AVX/AVX2/NEON based on CPU capabilities
- ⚡ **Zero-overhead Abstraction** — Compile-time dispatch via C++17 `if constexpr`
- 📦 **Header-only** — Just `#include <bitcal/bitcal.hpp>` and go
- 🔧 **Rich API** — Bitwise ops, shifts, popcount, CLZ/CTZ, bit reversal, ANDNOT
- 🌍 **Cross-platform** — Linux, Windows, macOS on x86 and ARM

## Quick Start

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    // Bitwise operations
    auto c = a & b;           // AND
    auto d = a | b;           // OR
    auto e = a ^ b;           // XOR
    auto f = ~a;              // NOT
    auto g = a.andnot(b);     // a & ~b (native SIMD)

    // Shifts
    a <<= 10;                 // Left shift
    b >>= 5;                  // Right shift

    // Bit counting
    uint64_t ones = a.popcount();
    int lz = a.count_leading_zeros();
    int tz = a.count_trailing_zeros();

    // Bit manipulation
    a.set_bit(42, true);      // Set bit 42
    bool bit = a.get_bit(42); // Read bit 42
    a.flip_bit(42);           // Flip bit 42
    a.reverse();              // Reverse all bits

    return 0;
}
```

## Installation

### Option 1: Header-only (Recommended)

```bash
cp -r include/bitcal /path/to/your/project/include/
```

```cpp
#include <bitcal/bitcal.hpp>
```

### Option 2: CMake Integration

```bash
git clone https://github.com/LessUp/bitcal.git
cd bitcal && mkdir build && cd build
cmake .. && make install
```

```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target bitcal::bitcal)
```

## API Reference

### Type Aliases

| Type | Width | Storage |
|------|-------|---------|
| `bitcal::bit64` | 64 bits | 1 × `uint64_t` |
| `bitcal::bit128` | 128 bits | 2 × `uint64_t` |
| `bitcal::bit256` | 256 bits | 4 × `uint64_t` |
| `bitcal::bit512` | 512 bits | 8 × `uint64_t` |
| `bitcal::bit1024` | 1024 bits | 16 × `uint64_t` |

### Core Operations

| Operation | Description |
|-----------|-------------|
| `a & b`, `a \| b`, `a ^ b`, `~a` | Bitwise AND, OR, XOR, NOT |
| `a.andnot(b)` | `a & ~b` using native SIMD instruction |
| `a << n`, `a >> n` | Left/right shift |
| `a.popcount()` | Count set bits |
| `a.count_leading_zeros()` | Count leading zeros (CLZ) |
| `a.count_trailing_zeros()` | Count trailing zeros (CTZ) |
| `a.reverse()` | Reverse bit order |
| `a.is_zero()` | Check if all bits are zero |
| `a.get_bit(i)`, `a.set_bit(i, v)`, `a.flip_bit(i)` | Single bit operations |

### SIMD Backends

```cpp
enum class simd_backend {
    scalar,   // Portable fallback
    sse2,     // x86 SSE2
    avx,      // x86 AVX
    avx2,     // x86 AVX2
    avx512,   // x86 AVX-512 (falls back to AVX2)
    neon      // ARM NEON
};
```

Force a specific backend:
```cpp
bitcal::bitarray<256, bitcal::simd_backend::avx2> avx2_array;
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable_array;
```

## Performance

Intel Core i7-12700K @ 3.6GHz (AVX2 backend):

| Operation | Scalar | AVX2 | Speedup |
|-----------|--------|------|---------|
| AND-256 | 12.3 ns | 2.1 ns | **5.9×** |
| XOR-512 | 24.8 ns | 4.3 ns | **5.8×** |
| ShiftLeft-256 | 18.6 ns | 5.2 ns | **3.6×** |
| Popcount-512 | 45.2 ns | 22.3 ns | **2.0×** |

ARM Cortex-A72 @ 2.0GHz (NEON backend):

| Operation | Scalar | NEON | Speedup |
|-----------|--------|------|---------|
| AND-128 | 8.4 ns | 3.2 ns | **2.6×** |
| XOR-256 | 16.9 ns | 6.8 ns | **2.5×** |

## Platform Support

| Platform | Architecture | Compilers | Status |
|----------|-------------|-----------|--------|
| Linux | x86-64 | GCC 7+, Clang 6+ | ✅ CI verified |
| Linux | ARM64 | GCC (cross-compile) | ✅ CI verified |
| Linux | ARM32 | GCC (cross-compile) | ✅ CI verified |
| Windows | x86-64 | MSVC 2017+ | ✅ CI verified |
| macOS | x86-64 | Apple Clang | ✅ CI verified |
| macOS | ARM64 | Apple Clang | ✅ CI verified |

## Project Structure

```
bitcal/
├── include/bitcal/           # Header files
│   ├── config.hpp            # Platform detection & macros
│   ├── simd_traits.hpp       # SIMD type traits
│   ├── scalar_ops.hpp        # Scalar implementation
│   ├── sse_ops.hpp           # SSE2 implementation
│   ├── avx_ops.hpp           # AVX2 implementation
│   ├── neon_ops.hpp          # NEON implementation
│   └── bitcal.hpp            # Main header
├── tests/                    # Unit tests
├── benchmarks/               # Performance benchmarks
├── examples/                 # Usage examples
├── gitbook/                  # Documentation
└── changelog/                # Change logs
```

## Requirements

- C++17 or later
- CMake 3.16+ (for building tests/examples only)
- Supported compilers: GCC 7+, Clang 6+, MSVC 2017+

## Documentation

- [Getting Started](gitbook/getting-started/quickstart.md)
- [API Reference](gitbook/api/types.md)
- [Architecture](gitbook/architecture/overview.md)
- [Migration Guide](MIGRATION_GUIDE.md)

## License

MIT License — see [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Acknowledgments

- Inspired by design patterns from Boost.SIMD and xsimd
- Uses Google Benchmark for performance testing (optional)
