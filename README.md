# BitCal - High-Performance Bit Operation Acceleration Library

[![CI](https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/bitcal/actions/workflows/ci.yml)
[![Docs](https://github.com/LessUp/bitcal/actions/workflows/docs.yml/badge.svg)](https://lessup.github.io/bitcal/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

English | [简体中文](README.zh-CN.md)

A modern, cross-platform, high-performance bit operation acceleration library optimized for Linux and ARM platforms.

> **Current Version**: v2.1.0

## Features

### Performance
- **Multi-ISA Support**: x86/x64 (SSE2, AVX, AVX2, AVX-512), ARM (NEON), generic scalar fallback
- **Compile-Time Optimization**: Zero-overhead abstractions via C++17 templates and constexpr
- **SIMD Acceleration**: Automatic optimal instruction set selection

### Functionality
- Shift operations (left/right): 64/128/256/512/1024-bit
- Logical operations (AND/OR/XOR/NOT)
- Bit counting: `popcount`, `clz` (leading zeros), `ctz` (trailing zeros)
- Bit reversal
- Byte order conversion

### Architecture
- **Header-Only**: Include and use directly, no compilation needed
- **Template Design**: Compile-time type safety
- **Cross-Platform**: Linux/Windows/macOS, x86/ARM
- **Zero Dependencies**: Standard library and compiler builtins only

## Quick Start

### Requirements
- C++17 or later
- CMake 3.16+ (for building tests only)
- Supported compilers: GCC 7+, Clang 6+, MSVC 2017+

### Installation

#### Option 1: Header-Only (Recommended)
```bash
cp -r include/bitcal /path/to/your/project/include/
```

```cpp
#include <bitcal/bitcal.hpp>
```

#### Option 2: CMake Integration
```bash
git clone https://github.com/LessUp/bitcal.git
cd bitcal && mkdir build && cd build
cmake .. && make && make install
```

```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target bitcal::bitcal)
```

### Basic Usage

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;   // AND
    auto d = a | b;   // OR
    auto e = a ^ b;   // XOR

    a <<= 10;         // Left shift
    b >>= 5;          // Right shift

    uint64_t ones = a.popcount();
    int leading = a.count_leading_zeros();
    int trailing = a.count_trailing_zeros();

    a.set_bit(42, true);
    bool bit = a.get_bit(42);
    a.flip_bit(42);
    a.reverse();

    return 0;
}
```

## Performance

Intel Core i7-12700K @ 3.6GHz (ns/op):

| Operation | Scalar | SSE2 | AVX2 | Speedup |
|-----------|--------|------|------|---------|
| AND-256 | 12.3 | 4.5 | 2.1 | 5.9x |
| XOR-512 | 24.8 | 9.2 | 4.3 | 5.8x |
| ShiftLeft-256 | 18.6 | 8.4 | 5.2 | 3.6x |
| Popcount-512 | 45.2 | 28.1 | 22.3 | 2.0x |

ARM Cortex-A72 @ 2.0GHz (Raspberry Pi 4):

| Operation | Scalar | NEON | Speedup |
|-----------|--------|------|---------|
| AND-128 | 8.4 | 3.2 | 2.6x |
| XOR-256 | 16.9 | 6.8 | 2.5x |
| ShiftLeft-128 | 12.5 | 5.1 | 2.5x |

## API Reference

### Type Aliases
```cpp
bitcal::bit64 / bit128 / bit256 / bit512 / bit1024
```

### Core Operations
- `shift_left(int)` / `operator<<`, `shift_right(int)` / `operator>>`
- `operator&`, `operator|`, `operator^`, `operator~`
- `andnot(mask)` — native SIMD ANDNOT
- `popcount()`, `count_leading_zeros()`, `count_trailing_zeros()`
- `reverse()`, `is_zero()` (SIMD-accelerated)
- `get_bit(size_t)`, `set_bit(size_t, bool)`, `flip_bit(size_t)`

### SIMD Backends
```cpp
enum class simd_backend { scalar, sse2, avx, avx2, avx512, neon };
```

## Project Structure

```
bitcal/
├── include/bitcal/
│   ├── config.hpp        # Platform detection
│   ├── simd_traits.hpp   # SIMD trait definitions
│   ├── scalar_ops.hpp    # Scalar implementation
│   ├── sse_ops.hpp       # SSE/SSE2 implementation
│   ├── avx_ops.hpp       # AVX/AVX2 implementation
│   ├── neon_ops.hpp      # ARM NEON implementation
│   └── bitcal.hpp        # Main header
├── tests/                # Unit tests
├── benchmarks/           # Performance benchmarks
└── examples/             # Examples
```

## License

MIT License
