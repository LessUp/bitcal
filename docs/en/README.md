# BitCal Documentation

<p align="center">
  <strong>Modern, Cross-Platform, SIMD-Accelerated Bit Manipulation Library for C++17</strong>
</p>

<p align="center">
  <a href="getting-started/installation.md">📦 Installation</a> •
  <a href="getting-started/quickstart.md">🚀 Quick Start</a> •
  <a href="api/types.md">📚 API Reference</a> •
  <a href="architecture/overview.md">🏗️ Architecture</a> •
  <a href="../../specs/product/bit-manipulation-library.md">📋 Product Spec</a>
</p>

---

## Overview

**BitCal** is a high-performance C++17 header-only library that provides comprehensive bit manipulation operations with automatic SIMD acceleration. By leveraging compile-time dispatch and modern C++ features, BitCal delivers up to **6× performance improvement** over scalar implementations.

### Key Design Philosophy

- **Zero Runtime Overhead**: All SIMD backend selection happens at compile time via `if constexpr`
- **Transparent Acceleration**: Automatically utilizes SSE2/AVX2 (x86) or NEON (ARM) without code changes
- **Graceful Degradation**: Falls back to scalar implementation on unsupported platforms
- **Type Safety**: Compile-time bit-width guarantees with `constexpr` validation

## Quick Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // Create 256-bit bit arrays
    bitcal::bit256 a(0xDEADBEEFCAFEBABE);
    bitcal::bit256 b(0x123456789ABCDEF0);
    
    // SIMD-accelerated bitwise operations
    auto c = a & b;           // AND: ~2.1ns (AVX2)
    auto d = a | b;           // OR:  ~2.1ns (AVX2)
    auto e = a ^ b;           // XOR: ~2.1ns (AVX2)
    
    // Bit counting (hardware-accelerated where available)
    auto popcount = c.popcount();
    auto leading_zeros = c.count_leading_zeros();
    
    std::cout << "Popcount: " << popcount << std::endl;
    std::cout << "Leading zeros: " << leading_zeros << std::endl;
    
    return 0;
}
```

**Compile & Run:**
```bash
g++ -std=c++17 -O3 -march=native example.cpp -o example && ./example
```

---

## Documentation Index

### 🚀 Getting Started

| Document | Description |
|----------|-------------|
| [Installation Guide](getting-started/installation.md) | System requirements, installation methods, verification |
| [Quick Start](getting-started/quickstart.md) | First BitCal program, core concepts, common patterns |
| [Build Options](getting-started/build-options.md) | Compiler flags, CMake integration, optimization tips |

### 📚 API Reference

| Document | Description |
|----------|-------------|
| [Types](api/types.md) | `bitarray` template, type aliases, memory layout |
| [Core Operations](api/core-operations.md) | AND, OR, XOR, NOT, ANDNOT, comparison, state detection |
| [Shift Operations](api/shift-operations.md) | Left/right shifts with cross-word carry |
| [Bit Counting](api/bit-counting.md) | `popcount()`, `clz()`, `ctz()` with hardware acceleration |
| [Bit Manipulation](api/bit-manipulation.md) | get/set/flip bits, `reverse()`, `clear()` |
| [SIMD Backend](api/simd-backend.md) | Backend selection, platform detection, forcing backends |
| [Ops Namespace](api/ops-namespace.md) | Low-level functional API for raw pointer operations |

### 🏗️ Architecture

| Document | Description |
|----------|-------------|
| [Overview](architecture/overview.md) | Design principles, file structure, performance characteristics |
| [SIMD Dispatch](architecture/simd-dispatch.md) | Compile-time dispatch mechanism, backend selection logic |
| [Platform Support](architecture/platform-support.md) | Supported platforms, compilers, and architectures |

### 📝 Changelog

| Version | Release Date |
|---------|--------------|
| [v2.1.0](changelog/v2.1.0.md) | 2026-04-16 |
| [v2.0.0](changelog/v2.0.0.md) | 2026-01-08 |

---

## Specifications

This project follows **Spec-Driven Development (SDD)**. All specifications are in the `/specs` directory:

| Directory | Purpose |
|-----------|---------|
| [Product Spec](../../specs/product/bit-manipulation-library.md) | Feature requirements and acceptance criteria |
| [Architecture RFCs](../../specs/rfc/) | Technical design documents |
| [API Spec](../../specs/api/bitcal-public-api.md) | Public interface specification |
| [Testing Spec](../../specs/testing/bitcal-testing-spec.md) | Test coverage requirements |

---

## Core Types

| Type | Width | Words | Best For |
|------|-------|-------|----------|
| `bitcal::bit64` | 64-bit | 1 | Machine word |
| `bitcal::bit128` | 128-bit | 2 | SSE2/NEON native |
| `bitcal::bit256` | 256-bit | 4 | AVX2 native |
| `bitcal::bit512` | 512-bit | 8 | Large parallel ops |
| `bitcal::bit1024` | 1024-bit | 16 | Very large ops |

## SIMD Performance

| Platform | Instruction Set | Speedup |
|----------|-----------------|---------|
| x86-64 | AVX2 | 5-6× |
| ARM | NEON | 2.5× |

---

## Language Support

This documentation is available in multiple languages:

- 🇺🇸 **English** (current)
- 🇨🇳 **[中文](../zh/README.md)** — 简体中文版本

---

## Getting Help

- 📖 **Full Documentation**: [GitHub Pages](https://lessup.github.io/bitcal/)
- 🐛 **Issue Tracker**: [GitHub Issues](https://github.com/LessUp/bitcal/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/LessUp/bitcal/discussions)
- 📋 **Changelog**: [View all releases](../../CHANGELOG.md)

## Contributing

Contributions are welcome! Please see our [Contributing Guidelines](../../CONTRIBUTING.md) for details.

---

<p align="center">
  <sub>Built with ❤️ by the BitCal team and contributors</sub>
</p>
