# BitCal

<p align="center">
  <strong>纯头文件 C++23 位运算练习库，x86-64 + AVX2 优先</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="许可证"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23"></a>
  <a href="#状态"><img src="https://img.shields.io/badge/status-experimental-orange.svg" alt="状态"></a>
</p>

---

## 概述

**BitCal** 是一个个人业余练习库，探索 SIMD 位运算在定宽字块上的最小实现。设计优先级：**小巧、可读、可快速迭代**，不追通用性。

公开模型三层：

- `bit_block<Bits>`：拥有固定宽度位存储
- `bit_view` / `const_bit_view`：非拥有视图
- `bit_and<Bits>()` 与 `and_into()` 这类自由算法

`<bitcal/bitcal.hpp>` 是唯一稳定公开入口。

## 状态

实验性。业余练习仓库，不保证 API 跨大版本稳定。

- 语言基线：C++23
- 平台：Linux x86-64（GCC / Clang）
- 后端：`scalar` / `avx2`，编译期由 `BITCAL_HAS_AVX2` 固定，无运行时选择
- 分发模型：源码集成、单一目标平台；不提供预编译二进制

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

    const auto lhs = bitcal::bit_block<256>::from_words(lhs_words);
    const auto rhs = bitcal::bit_block<256>::from_words(rhs_words);

    // bit_block 重载：Bits 自动推导，省去 <256> 与 .view()
    const auto and_result = bitcal::bit_and(lhs, rhs);
    const auto andnot_result = bitcal::bit_andnot(lhs, rhs);

    std::cout << "popcount(lhs) = " << bitcal::popcount(lhs.view()) << '\n';
    std::cout << "is_zero(andnot)? " << (bitcal::is_zero(andnot_result.view()) ? "yes" : "no") << '\n';
}
```

> **调用形态**：返回型算法（`bit_and` / `bit_or` / `bit_xor` / `bit_andnot` / `shift_left` / `shift_right`）有两套重载：
> - 静态宽度形态：`bit_and(lhs_words, rhs_words)` -- 接 `std::span<const uint64_t, N>` / `std::array` / C 数组，宽度从 extent 推导，宽度错配是编译错误
> - 拥有型形态：`bit_and(lhs, rhs)` -- 接 `bit_block<Bits>`，CTAD 推导
>
> 原地算法（`*_into`）与查询算法（`equals` / `is_zero` / `popcount`）只接视图，`bit_view` 隐式转 `const_bit_view`，无需重载。

## API 参考

### 核心类型

| 类型 | 说明 |
|------|------|
| `bit_block<Bits>` | 拥有固定宽度位存储。`Bits >= 64` 且为 64 的倍数（如 64、128、192、256、512） |
| `bit_view` | 非拥有可变视图 |
| `const_bit_view` | 非拥有只读视图 |

> **宽度约束说明**：`Bits % 64 == 0` 是硬约束，刻意只服务 64 倍数宽度场景（SHA 指纹、AES 块、SIMD 字块等）。不覆盖任意位宽需求（如 Curve25519 的 255 位、Bloom filter 的 `k*m` 变宽、位图索引的尾字非对齐）。原因：字宽对齐是 SIMD 字打包 + 强对齐 + 零跨字位偏移分支的前提，破坏它会动摇当前性能模型。

### 算法

| 类别 | 函数 |
|------|------|
| 位运算 | `bit_and()`, `bit_or()`, `bit_xor()`, `bit_andnot()` |
| 原地位运算 | `and_into()`, `or_into()`, `xor_into()`, `andnot_into()` |
| 查询 | `equals()`, `is_zero()`, `popcount()` |
| 移位 | `shift_left()`, `shift_right()` |

> **契约说明**：返回型算法取静态宽度 word range，宽度错配是编译错误；`*_into()` 要求视图宽度匹配，宽度不一致在 Release 下为未定义行为（Debug 下 `assert` 触发）；`equals()` 在宽度不一致时返回 `false`。
>
> **别名契约**：`*_into()` 内核逐字独立（`out[i]` 只依赖 `lhs[i]` / `rhs[i]`），`out` 可与 `lhs` 或 `rhs` 别名（含传入同一视图）；`copy_words_to()` 支持自拷贝。

### 后端

x86-64: `scalar` / `avx2`（编译期由 `BITCAL_HAS_AVX2` 固定，无运行时选择）

- AVX2 build：`-mavx2`（或 `-march=native`），4 字及以上（`>= 256` 位）的运算走 `__m256i` 路径；`>= 256` 位 `bit_block` 的存储按 32 字节对齐（存储保证，非正确性前提——内核使用 unaligned load/store，见 `NOTES.md`）。
- Scalar build：`-mno-avx2` 或不开启 `BITCAL_NATIVE_ARCH`，所有宽度走标量循环，自然对齐。

> **编译选项一致性**：后端在编译期固定且库为 header-only，同一程序的所有翻译单元必须以一致的 SIMD 编译选项构建。混用（如部分 TU 带 `-mavx2`、部分不带）会使 inline 内核在不同 TU 中定义不一致，构成 ODR 违规——链接器可能任意保留其中一版内核，导致无声的性能劣化。

## 构建

```bash
# AVX2 路径（默认开发配置）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build --config Release -j"$(nproc)"
ctest --test-dir build --output-on-failure -C Release
```

Scalar 路径（验证 `BITCAL_HAS_AVX2 == 0` 分支，已进 CI；以下本地命令供调试复用）：

```bash
cmake -S . -B build-scalar -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_NATIVE_ARCH=OFF -DCMAKE_CXX_FLAGS="-mno-avx2"
cmake --build build-scalar -j"$(nproc)"
ctest --test-dir build-scalar --output-on-failure
```

## 基准

`benchmarks/` 含两组可执行文件（需 `BITCAL_BUILD_BENCHMARKS=ON`）：

- `bitcal_benchmark`：BitCal `bit_and` 多宽度基线计时。
- `benchmark_compare`：BitCal 与 `std::bitset` 对比计时。

结果在本地生成，不入库；计时 harness 自研 `std::chrono`，无 google-benchmark 依赖。

