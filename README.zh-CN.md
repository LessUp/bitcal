# BitCal

<p align="center">
  <strong>纯头文件 C++17 位操作库，编译期 SIMD 分派</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://lessup.github.io/bitcal/"><img src="https://img.shields.io/badge/docs-GitHub_Pages-blue.svg" alt="文档"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="许可证"></a>
  <a href="https://en.cppreference.com/w/cpp/17"><img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17"></a>
  <a href="#-安装"><img src="https://img.shields.io/badge/header--only-yes-green.svg" alt="纯头文件"></a>
  <a href="CHANGELOG.zh-CN.md"><img src="https://img.shields.io/badge/version-3.0.0-blue.svg" alt="版本"></a>
</p>

<p align="center">
  <a href="README.md">English</a> | <strong>简体中文</strong>
</p>

---

## 概述

**BitCal** 是 C++17 纯头文件位操作库，提供编译期 SIMD 分派。保留 `<bitcal/bitcal.hpp>` 公开接口与 `bitarray` 模板，自动选择后端（SSE2/AVX2/NEON/标量），并提供可移植回退方案。BitCal 3.0 处于稳定维护与归档就绪状态。

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;           // SIMD 加速的与运算（~2.1ns）
    auto pop = c.popcount();  // 硬件 popcount

    return 0;
}
```

## ✨ 特性

- **纯头文件**：单一 `#include <bitcal/bitcal.hpp>`，零外部依赖
- **编译期 SIMD 分派**：`if constexpr` 在编译期选择 SSE2/AVX2（x86）、NEON（ARM）或可移植标量回退
- **保留公开接口**：`bitarray<Bits>` 模板及成员函数覆盖所有操作
- **丰富 API**：位运算、位移、popcount、CLZ/CTZ、位反转、ANDNOT、单比特操作
- **跨平台**：Linux、Windows、macOS，支持 x86-64 和 ARM
- **类型安全**：编译期位宽验证（`Bits % 64 == 0`）
- **稳定维护**：3.0 契约冻结，归档就绪状态

## ⚠️ 3.0.0 破坏性变更

BitCal 3.0.0 将公开 API 收敛到保留的 `bitarray` 表面：

- 移除了公开的 `bitcal::ops` 原始指针辅助接口
- 移除了公开 traits：`is_bitarray`、`is_bitarray_v`、`bitarray_traits`
- 移除了公开契约中的显式 `bit64` 转换便捷入口
- 迁移方式：使用 `bitarray` 成员函数；通过 `word()` / `set_word()` 适配已有 `uint64_t` 缓冲区

迁移细节见 [CHANGELOG.zh-CN.md](CHANGELOG.zh-CN.md) 与 [API 参考](https://lessup.github.io/bitcal/)。

## 🚀 安装

### 方式一：复制头文件（最简单）

```bash
# 克隆仓库
git clone https://github.com/LessUp/bitcal.git

# 复制头文件到项目（纯头文件）
cp -r bitcal/include/bitcal /path/to/your/project/include/
```

### 方式二：CMake FetchContent（推荐）

```cmake
include(FetchContent)
FetchContent_Declare(
    bitcal
    GIT_REPOSITORY https://github.com/LessUp/bitcal.git
    GIT_TAG v3.0.0
)
FetchContent_MakeAvailable(bitcal)

target_link_libraries(your_target PRIVATE bitcal::bitcal)
```

### 方式三：CMake 安装

```bash
# 克隆并安装
git clone https://github.com/LessUp/bitcal.git
 cd bitcal
 mkdir build && cd build
 cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
 cmake --build .
 cmake --install .
```

然后在你的 `CMakeLists.txt` 中：

```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target PRIVATE bitcal::bitcal)
```

## 🚦 快速开始

### 基础编译

```bash
g++ -std=c++17 -O3 -march=native your_program.cpp -o your_program
```

> 💡 **提示：** 使用 `-march=native` 启用 CPU 支持的所有 SIMD 指令。

### 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // 创建不同位宽的位数组
    bitcal::bit256 a(0xFF00FF00FF00FF00);
    bitcal::bit256 b(0x0FF00FF00FF00FF0);

    // 位运算（自动 SIMD 加速）
    auto c = a & b;              // 与
    auto d = a | b;              // 或
    auto e = a ^ b;              // 异或
    auto f = ~a;                 // 非（SIMD 加速）
    auto g = a.andnot(b);        // a & ~b（比分步操作快约 2 倍）

    // 复合赋值
    a &= b;                      // 与并赋值
    b |= a;                      // 或并赋值

    // 位移
    a <<= 10;                    // 左移
    b >>= 5;                     // 右移

    // 位计数操作
    uint64_t ones = a.popcount();
    int lz = a.count_leading_zeros();   // 前导零计数
    int tz = a.count_trailing_zeros();  // 尾随零计数

    // 单比特操作
    a.set_bit(42, true);         // 设置第 42 位
    bool bit = a.get_bit(42);    // 读取第 42 位
    a.flip_bit(42);              // 翻转第 42 位

    // 位操作
    a.reverse();                 // 位反转
    a.clear();                   // 清零

    // 直接访问原始数据（用于与 C API 互操作）
    const uint64_t* data = a.data();   // 只读对齐指针
    uint64_t word = a[0];        // 访问第一个 64 位字

    // 比较
    if (a == b) { /* ... */ }
    if (a != b) { /* ... */ }
    if (a.is_zero()) { /* 所有位均为零 */ }

    std::cout << "Popcount: " << ones << std::endl;
    return 0;
}
```

### 支持的类型

| 类型 | 位宽 | 字数 | 最佳用途 |
|------|------|------|----------|
| `bitcal::bit64` | 64 位 | 1 | 机器字操作 |
| `bitcal::bit128` | 128 位 | 2 | SSE2/NEON 原生位宽 |
| `bitcal::bit256` | 256 位 | 4 | AVX2 原生位宽 |
| `bitcal::bit512` | 512 位 | 8 | 大位宽并行运算 |
| `bitcal::bit1024` | 1024 位 | 16 | 超大位宽并行运算 |

自定义位宽（64 的倍数）：
```cpp
bitcal::bitarray<2048> custom;
```

## 📚 文档与 API

`bitarray<Bits>` 模板提供位运算操作符（`&`、`|`、`^`、`~`、`<<`、`>>`），位计数（`popcount`、CLZ/CTZ），单比特操作（`get_bit`、`set_bit`、`flip_bit`）和直接字访问。完整细节见 [API 参考](https://lessup.github.io/bitcal/zh/api/)。

完整文档位于 **[https://lessup.github.io/bitcal/](https://lessup.github.io/bitcal/)**，包含架构设计、性能基准与平台特定说明。

## 🌍 平台支持

**Linux**、**Windows**、**macOS**，支持 x86-64 与 ARM，C++17 或更高版本。版本历史见 [CHANGELOG.zh-CN.md](CHANGELOG.zh-CN.md)，CI 验证细节见[平台文档](https://lessup.github.io/bitcal/zh/architecture/platform-support.html)

## 🤝 参与贡献

欢迎在 3.0 契约边界内贡献。对于重大变更，请先开 issue 讨论。贡献指南见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 📄 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。
