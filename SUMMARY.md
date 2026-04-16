# BitCal 2.1

**高性能位运算加速库** — 现代 C++17，Header-only，SIMD 加速

[![CI](https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/bitcal/actions/workflows/ci.yml)
[![Docs](https://github.com/LessUp/bitcal/actions/workflows/pages.yml/badge.svg)](https://lessup.github.io/bitcal/)

## 概况

| 项目 | 信息 |
|------|------|
| **版本** | 2.1.0 |
| **语言** | C++17 |
| **类型** | Header-only |
| **许可** | MIT |

## 核心特性

- ✅ **零开销抽象** — 编译期 SIMD 分派
- ✅ **跨平台** — x86 (SSE2/AVX/AVX2) + ARM (NEON)
- ✅ **丰富 API** — 位运算、位移、popcount、CLZ/CTZ
- ✅ **Header-only** — 无需编译，直接 include

## 性能

| 平台 | 后端 | 加速比 |
|------|------|--------|
| x86-64 | AVX2 | **5-6×** |
| ARM | NEON | **2.5×** |

## 架构

```
┌─────────────────────────────────────┐
│    用户 API (bitarray<N>)          │
├─────────────────────────────────────┤
│    编译期 SIMD 选择 (traits)       │
├──────────┬──────────┬───────────────┤
│  scalar  │   x86    │     ARM       │
│  (通用)  │SSE2/AVX2 │    NEON       │
└──────────┴──────────┴───────────────┘
```

## 文件结构

```
bitcal/
├── include/bitcal/     # 核心头文件
├── tests/              # 单元测试 (36 个)
├── benchmarks/         # 性能测试
├── examples/           # 示例代码
├── gitbook/            # 完整文档
├── CHANGELOG.md        # 变更日志
├── README.md           # 项目概述
└── CONTRIBUTING.md     # 贡献指南
```

## 快速开始

```cpp
#include <bitcal/bitcal.hpp>

bitcal::bit256 a(0xDEADBEEF);
bitcal::bit256 b(0xCAFEBABE);

auto c = a & b;           // AND
a <<= 10;                 // 左移
auto ones = a.popcount(); // 统计 1 的个数
```

## 平台支持

| 平台 | 架构 | 状态 |
|------|------|------|
| Linux | x86-64 | ✅ CI |
| Linux | ARM64/ARM32 | ✅ CI |
| Windows | x86-64 | ✅ CI |
| macOS | x86-64/ARM64 | ✅ CI |

## 文档

| 文档 | 说明 |
|------|------|
| [README.md](README.md) | 英文概述 |
| [README.zh-CN.md](README.zh-CN.md) | 中文概述 |
| [CHANGELOG.md](CHANGELOG.md) | 变更日志 |
| [CONTRIBUTING.md](CONTRIBUTING.md) | 贡献指南 |
| [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) | 迁移指南 |
| [gitbook/](gitbook/) | 完整文档 |
