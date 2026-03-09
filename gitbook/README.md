# BitCal — 高性能位运算加速库

[![CI](https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/bitcal/actions/workflows/ci.yml)
[![Docs](https://github.com/LessUp/bitcal/actions/workflows/docs.yml/badge.svg)](https://lessup.github.io/bitcal/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/LessUp/bitcal/blob/main/LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)](#)

**BitCal** 是一个现代化的、跨平台的 C++17 高性能位运算加速库。通过编译期模板特化自动选择最优 SIMD 指令集，实现零运行时开销的位操作加速。

> 当前版本：**v2.1.0** · [GitHub 仓库](https://github.com/LessUp/bitcal) · [更新日志](changelog/v2.1.0.md)

---

## 核心特性

| 特性 | 说明 |
|------|------|
| **Header-only** | 零编译依赖，`#include <bitcal/bitcal.hpp>` 即可使用 |
| **多指令集加速** | SSE2 · AVX · AVX2 · AVX-512 · ARM NEON · 标量回退 |
| **编译期优化** | C++17 `if constexpr` + 模板特化，零运行时分派开销 |
| **跨平台** | Linux / Windows / macOS，x86 / ARM，GCC / Clang / MSVC |
| **丰富 API** | 逻辑运算 · 位移 · popcount · CLZ/CTZ · 位反转 · ANDNOT |

## 架构总览

```
┌─────────────────────────────────────────┐
│  用户 API                               │
│  bitcal::bit64 / bit128 / bit256 / ...  │
│  运算符: & | ^ ~ << >> andnot()         │
├─────────────────────────────────────────┤
│  编译期 SIMD 选择 (simd_traits)         │
│  if constexpr → 零开销分派              │
├──────────┬──────────┬──────────┬────────┤
│ scalar   │ SSE/SSE2 │ AVX/AVX2 │  NEON  │
│ (通用)   │  (x86)   │  (x86)   │ (ARM)  │
├──────────┴──────────┴──────────┴────────┤
│  硬件指令集                              │
│  alignas(64) · SIMD intrinsics          │
└─────────────────────────────────────────┘
```

## 快速开始

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;           // AND
    auto d = a | b;           // OR
    auto e = a ^ b;           // XOR
    auto f = a.andnot(b);     // a & ~b（原生 SIMD）

    a <<= 10;                 // 左移
    uint64_t ones = a.popcount();        // 统计 1 的个数
    int clz = a.count_leading_zeros();   // 前导零
    int ctz = a.count_trailing_zeros();  // 尾部零

    a.set_bit(42, true);      // 设置位
    a.flip_bit(42);           // 翻转位
    a.reverse();              // 位反转

    return 0;
}
```

➡️ 更多示例请查看 [快速上手](getting-started/quickstart.md)

## 性能数据

### x86-64（Intel Core i7-12700K @ 3.6GHz）

| 操作 | 标量 | SSE2 | AVX2 | 加速比 |
|------|------|------|------|--------|
| AND-256 | 12.3 ns | 4.5 ns | 2.1 ns | **5.9×** |
| XOR-512 | 24.8 ns | 9.2 ns | 4.3 ns | **5.8×** |
| ShiftLeft-256 | 18.6 ns | 8.4 ns | 5.2 ns | **3.6×** |
| Popcount-512 | 45.2 ns | 28.1 ns | 22.3 ns | **2.0×** |

### ARM64（Raspberry Pi 4 Cortex-A72 @ 2.0GHz）

| 操作 | 标量 | NEON | 加速比 |
|------|------|------|--------|
| AND-128 | 8.4 ns | 3.2 ns | **2.6×** |
| XOR-256 | 16.9 ns | 6.8 ns | **2.5×** |
| ShiftLeft-128 | 12.5 ns | 5.1 ns | **2.5×** |

## 支持的位宽与后端

| 类型别名 | 位宽 | SSE2 | AVX2 | NEON | 标量 |
|----------|------|------|------|------|------|
| `bit64` | 64 | ✅ | ✅ | ✅ | ✅ |
| `bit128` | 128 | ✅ 原生 | ✅ | ✅ 原生 | ✅ |
| `bit256` | 256 | ✅ 2×128 | ✅ 原生 | ✅ 2×128 | ✅ |
| `bit512` | 512 | ✅ 4×128 | ✅ 2×256 | ✅ 4×128 | ✅ |
| `bit1024` | 1024 | ✅ | ✅ | ✅ | ✅ |

## 平台兼容性

| 平台 | 架构 | 编译器 | 状态 |
|------|------|--------|------|
| Linux | x86-64 | GCC 7+, Clang 6+ | ✅ CI 验证 |
| Linux | ARM64 | GCC (交叉编译) | ✅ CI 验证 |
| Windows | x86-64 | MSVC 2017+ | ✅ CI 验证 |
| macOS | x86-64 | Apple Clang | ✅ CI 验证 |
| macOS | ARM64 | Apple Clang | ✅ CI 验证 |

## 适用场景

- **高性能计算** — 大位宽位向量操作
- **密码学** — 位级运算加速
- **数据压缩** — 位流处理
- **图像处理** — 二值图像操作
- **嵌入式系统** — ARM NEON 加速

---

📖 通过左侧导航栏浏览完整 [API 参考](api/types.md)、[架构设计](architecture/overview.md) 和 [安装指南](getting-started/installation.md)。
