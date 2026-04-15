# BitCal

**高性能位运算加速库**

BitCal 是一个现代化的 C++17 header-only 库，通过 SIMD 指令集加速位操作。

[GitHub](https://github.com/LessUp/bitcal) · [Releases](https://github.com/LessUp/bitcal/releases)

---

## 为什么选择 BitCal？

| 特性 | 说明 |
|------|------|
| 🚀 **高性能** | AVX2 可达 5-6 倍加速，NEON 可达 2.5 倍加速 |
| ⚡ **零开销** | 编译期分派，无运行时开销 |
| 📦 **易集成** | Header-only，只需一行 include |
| 🌍 **跨平台** | 支持 x86 和 ARM，所有主流操作系统 |

## 快速示例

```cpp
#include <bitcal/bitcal.hpp>

bitcal::bit256 a(0xDEADBEEF);
bitcal::bit256 b(0xCAFEBABE);

auto c = a & b;           // 位运算
a <<= 10;                 // 位移
auto ones = a.popcount(); // 位计数
```

## 核心功能

- **位运算**: AND, OR, XOR, NOT, ANDNOT
- **位移**: 左移、右移，支持跨 word 进位
- **位计数**: popcount, CLZ, CTZ
- **位操作**: get/set/flip 单个位
- **其他**: 位反转、零检测

## 支持的位宽

64 位 · 128 位 · 256 位 · 512 位 · 1024 位 · 自定义

## SIMD 支持

| 平台 | 指令集 |
|------|--------|
| x86-64 | SSE2, AVX, AVX2 |
| ARM | NEON |
| 其他 | 标量回退 |

---

➡️ 从 [安装指南](getting-started/installation.md) 开始
