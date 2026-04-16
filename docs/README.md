# BitCal Documentation

BitCal is a high-performance, cross-platform C++17 bit manipulation library with SIMD acceleration.

## Quick Start

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;           // AND
    a <<= 10;                 // Left shift
    auto ones = a.popcount(); // Count set bits

    return 0;
}
```

## Documentation Structure

### Getting Started
- [Installation Guide](setup/installation.md) — Setup and installation
- [Quick Start](setup/quickstart.md) — Your first BitCal program
- [Build Options](setup/build-options.md) — Compiler flags and CMake options
- [Migration Guide](setup/migration-guide.md) — Migrating from v1.x to v2.x

### API Reference
- [Types](api/types.md) — `bitarray` template and type aliases
- [Core Operations](api/core-operations.md) — AND, OR, XOR, NOT, ANDNOT
- [Shift Operations](api/shift-operations.md) — Left and right shifts
- [Bit Counting](api/bit-counting.md) — popcount, CLZ, CTZ
- [Bit Manipulation](api/bit-manipulation.md) — get/set/flip bits, reverse
- [SIMD Backend](api/simd-backend.md) — Backend selection
- [Ops Namespace](api/ops-namespace.md) — Low-level functional API

### Architecture
- [Overview](architecture/overview.md) — Design principles
- [SIMD Dispatch](architecture/simd-dispatch.md) — Compile-time dispatch mechanism
- [Platform Support](architecture/platform-support.md) — Compatibility matrix

### Specifications
- [Product Spec](../specs/product/bit-manipulation-library.md) — Feature requirements (PRD)
- [Architecture RFCs](../specs/rfc/) — Technical design documents
- [API Spec](../specs/api/bitcal-public-api.md) — Public interface specification
- [Testing Spec](../specs/testing/bitcal-testing-spec.md) — Test coverage requirements

### Internal Documents
- [Refactoring Notes](internal/refactor-notes.md) — Design rationale for v2.x
- [Release Notes](internal/release-notes.md) — Detailed release information

## Core Types

| Type | Width | Description |
|------|-------|-------------|
| `bitcal::bit64` | 64-bit | Single machine word |
| `bitcal::bit128` | 128-bit | SSE2/NEON native width |
| `bitcal::bit256` | 256-bit | AVX2 native width |
| `bitcal::bit512` | 512-bit | Large operations |
| `bitcal::bit1024` | 1024-bit | Very large operations |

## SIMD Performance

| Platform | Instruction Set | Speedup |
|----------|-----------------|---------|
| x86-64 | AVX2 | 5-6× |
| ARM | NEON | 2.5× |

## Project Structure

```
bitcal/
├── include/bitcal/       # Core headers (header-only library)
├── specs/                # Specification documents (PRDs, RFCs, APIs)
├── docs/                 # Developer and user documentation
│   ├── setup/            # Installation and build guides
│   ├── api/              # API reference documentation
│   ├── architecture/     # Architecture explanations
│   ├── internal/         # Internal development notes
│   └── assets/           # Images, diagrams
├── tests/                # Unit tests
├── benchmarks/           # Performance benchmarks
└── examples/             # Example programs
```

## Links

- [GitHub Repository](https://github.com/LessUp/bitcal)
- [中文 README](../README.zh-CN.md)
- [Changelog](../CHANGELOG.md)
- [Contributing Guide](../CONTRIBUTING.md)
