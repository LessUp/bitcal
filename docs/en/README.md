# BitCal Documentation

<p align="center">
  <strong>Modern, Cross-Platform, SIMD-Accelerated Bit Manipulation Library for C++17</strong>
</p>

<p align="center">
  <a href="getting-started/installation.md">📦 Installation</a> •
  <a href="getting-started/quickstart.md">🚀 Quick Start</a> •
  <a href="api/types.md">📚 API Reference</a> •
  <a href="architecture/overview.md">🏗️ Architecture</a>
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

## Documentation Structure

### 🚀 Getting Started
| Document | Description | Difficulty |
|----------|-------------|------------|
| [Installation Guide](getting-started/installation.md) | System requirements, installation methods, and verification | ⭐ Beginner |
| [Quick Start](getting-started/quickstart.md) | First BitCal program, core concepts, common patterns | ⭐ Beginner |
| [Build Options](getting-started/build-options.md) | Compiler flags, CMake integration, optimization tips | ⭐⭐ Intermediate |

### 📚 API Reference
| Document | Description | Status |
|----------|-------------|--------|
| [Types](api/types.md) | `bitarray` template, type aliases, memory layout | Complete |
| [Core Operations](api/core-operations.md) | AND, OR, XOR, NOT, ANDNOT, comparison, state detection | Complete |
| [Shift Operations](api/shift-operations.md) | Left/right shifts with cross-word carry | Complete |
| [Bit Counting](api/bit-counting.md) | `popcount()`, `clz()`, `ctz()` with hardware acceleration | Complete |
| [Bit Manipulation](api/bit-manipulation.md) | get/set/flip bits, `reverse()`, `clear()` | Complete |
| [SIMD Backend](api/simd-backend.md) | Backend selection, platform detection, forcing backends | Complete |
| [Ops Namespace](api/ops-namespace.md) | Low-level functional API for raw pointer operations | Complete |

### 🏗️ Architecture
| Document | Description | Audience |
|----------|-------------|----------|
| [Overview](architecture/overview.md) | Design principles, file structure, performance characteristics | All users |
| [SIMD Dispatch](architecture/simd-dispatch.md) | Compile-time dispatch mechanism, backend selection logic | Advanced users |
| [Platform Support](architecture/platform-support.md) | Supported platforms, compilers, and architectures | All users |

### 📝 Release Notes
| Version | Release Date | Highlights |
|---------|--------------|------------|
| [v2.1.0](changelog/v2.1.0.md) | 2026-02-27 | ANDNOT operation, unified dispatch, performance improvements |
| [v2.0.0](changelog/v2.0.0.md) | 2026-01-08 | Complete rewrite: header-only, template-based, compile-time dispatch |

## Language Support

This documentation is available in multiple languages:

- 🇺🇸 **[English](README.md)** (current)
- 🇨🇳 **[中文](..//zh/README.md)** — 简体中文版本

## Version Information

| Property | Value |
|----------|-------|
| **Current Version** | v2.1.0 |
| **Release Date** | February 27, 2026 |
| **Minimum C++ Standard** | C++17 |
| **License** | MIT |
| **Dependencies** | None (header-only) |

## Getting Help

- 📖 **Full Documentation**: [GitHub Pages](https://lessup.github.io/bitcal/)
- 🐛 **Issue Tracker**: [GitHub Issues](https://github.com/LessUp/bitcal/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/LessUp/bitcal/discussions)
- 📋 **Changelog**: [View all releases](../../CHANGELOG.md)

## Contributing

Contributions are welcome! Please see our [Contributing Guidelines](../../CONTRIBUTING.md) for details on:
- Code style and conventions
- Pull request process
- Reporting issues
- Feature requests

---

<p align="center">
  <sub>Built with ❤️ by the BitCal team and contributors</sub>
</p>
