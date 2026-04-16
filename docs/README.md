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

### English Documentation

| Directory | Contents |
|-----------|----------|
| [Getting Started](en/getting-started/) | Installation, quickstart, build options |
| [API Reference](en/api/) | Types, operations, backend selection |
| [Architecture](en/architecture/) | Design principles, SIMD dispatch, platform support |
| [Changelog](en/changelog/) | Version history and release notes |

### 中文文档

| 目录 | 内容 |
|------|------|
| [入门指南](zh/getting-started/) | 安装、快速开始、构建选项 |
| [API 参考](zh/api/) | 类型、操作、后端选择 |
| [架构设计](zh/architecture/) | 设计原则、SIMD 分派、平台支持 |
| [更新日志](zh/changelog/) | 版本历史和发布说明 |

## Specifications

The `/specs` directory contains all specification documents following Spec-Driven Development methodology:

| Directory | Purpose |
|-----------|---------|
| `/specs/product/` | Product Requirements Documents (PRD) |
| `/specs/rfc/` | Technical design documents (RFC) |
| `/specs/api/` | Public API specifications |
| `/specs/testing/` | Test specifications and coverage requirements |

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

## Project Structure

```
bitcal/
├── include/bitcal/       # Core headers (header-only library)
├── specs/                # Specification documents (PRDs, RFCs, APIs)
├── docs/                 # Developer and user documentation
│   ├── en/               # English documentation
│   └── zh/               # Chinese documentation (中文文档)
├── tests/                # Unit tests
├── benchmarks/           # Performance benchmarks
└── examples/             # Example programs
```

## Links

- [GitHub Repository](https://github.com/LessUp/bitcal)
- [README (English)](../README.md)
- [README (中文)](../README.zh-CN.md)
- [Changelog](../CHANGELOG.md)
- [Contributing Guide](../CONTRIBUTING.md)
