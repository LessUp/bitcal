# BitCal

<p align="center">
  <strong>纯头文件 C++23 位运算库，正在进行 vNext 重设计</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="许可证"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23"></a>
  <a href="#状态"><img src="https://img.shields.io/badge/status-vNext-redesign-orange.svg" alt="状态"></a>
</p>

---

## 概述

**BitCal** 是一个纯头文件位运算库，当前正朝 **C++23** 与 **x86-64 优先** 的方向重设计。

新的公开模型围绕以下三层展开：
- `bit_block<Bits>`：拥有固定宽度位存储
- `bit_view` / `const_bit_view`：非拥有视图
- `bit_and<Bits>()` 与 `and_into()` 这类自由算法

`<bitcal/bitcal.hpp>` 仍然是唯一稳定的公开 include seam。

> English README: [README.en.md](README.en.md)（风格独立，不逐句对应）

## 状态

BitCal 当前处于 **vNext / 4.0.0** 的活动重设计阶段。

- **破坏性变更**：旧的 `bitarray` 中心公开模型正在被替换
- **破坏性变更**：最小语言基线提升到 C++23
- **破坏性变更**：不计划保留旧 API 的代码级兼容层
- **重点**：Linux / Windows x86-64 是主要优化与验证目标

## 快速开始

```cpp
#include <bitcal/bitcal.hpp>
#include <array>
#include <cstdint>
#include <iostream>
#include <span>

int main() {
    const std::array<std::uint64_t, 4> lhs_words{0xF0F0F0F0F0F0F0F0ULL, 0, 0, 0};
    const std::array<std::uint64_t, 4> rhs_words{0xFFFFFFFFFFFFFFFFULL, 0, 0, 0};

    const auto lhs = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(lhs_words.data(), lhs_words.size()));
    const auto rhs = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(rhs_words.data(), rhs_words.size()));

    const auto and_result = bitcal::bit_and<256>(lhs.view(), rhs.view());
    const auto andnot_result = bitcal::bit_andnot<256>(lhs.view(), rhs.view());

    std::cout << "popcount(lhs) = " << bitcal::popcount(lhs.view()) << '\n';
    std::cout << "is_zero(andnot)? " << (bitcal::is_zero(andnot_result.view()) ? "yes" : "no") << '\n';
}
```

## API 参考

### 核心类型

| 类型 | 说明 |
|------|------|
| `bit_block<Bits>` | 拥有固定宽度位存储。`Bits >= 64` 且为 64 的倍数（如 64、128、192、256） |
| `bit_view` | 非拥有可变视图 |
| `const_bit_view` | 非拥有只读视图 |

### 算法

| 类别 | 函数 |
|------|------|
| 位运算 | `bit_and<Bits>()`, `bit_or<Bits>()`, `bit_xor<Bits>()`, `bit_andnot<Bits>()` |
| 原地位运算 | `and_into()`, `or_into()`, `xor_into()`, `andnot_into()` |
| 查询 | `equals()`, `is_zero()`, `popcount()` |
| 移位 | `shift_left<Bits>()`, `shift_right<Bits>()` |

> **契约说明**：`equals()` 在视图宽度不一致时返回 `false`；其他算法要求视图宽度匹配，宽度不一致在 Release 下为未定义行为（Debug 下 `assert` 触发）。

### 后端

x86-64: `scalar` / `avx2`（编译期由 `BITCAL_HAS_AVX2` 固定，无运行时选择）

- AVX2 build：`-mavx2`（或 `-march=native`），`>= 256` 位 block 走 `__m256i` 路径并强制 32 字节对齐。
- Scalar build：`-mno-avx2` 或不开启 `BITCAL_NATIVE_ARCH`，所有宽度走标量循环，自然对齐。

## 构建

```bash
# AVX2 路径（默认开发配置）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build --config Release -j"$(nproc)"
ctest --test-dir build --output-on-failure -C Release

# Scalar 路径（验证 BITCAL_HAS_AVX2=0 分支）
cmake -S . -B build-scalar -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_NATIVE_ARCH=OFF -DCMAKE_CXX_FLAGS="-mno-avx2"
cmake --build build-scalar -j"$(nproc)"
ctest --test-dir build-scalar --output-on-failure
```

## 基准

`benchmarks/` 含两组可执行文件（需 `BITCAL_BUILD_BENCHMARKS=ON`）：

- `bitcal_benchmark`：BitCal 自身各操作计时。
- `benchmark_compare`：BitCal 与 `std::bitset` 对比计时。

结果在本地生成，不入库；如需保留基线，请本地保存 `benchmarks/results/` 下的输出。

## 迁移说明

旧的 `bitarray` API 已不再属于当前 shipped vNext 公共表面。BitCal 4.x 期望使用者迁移到 `bit_block`、`bit_view` / `const_bit_view` 与通过 `<bitcal/bitcal.hpp>` 提供的自由算法模型。

