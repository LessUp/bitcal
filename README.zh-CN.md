# BitCal

<p align="center">
  <strong>现代化、跨平台、基于 SIMD 指令集加速的高性能 C++17 位运算库</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://lessup.github.io/bitcal/"><img src="https://img.shields.io/badge/docs-Doxygen-blue.svg" alt="文档"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="许可证"></a>
  <a href="https://en.cppreference.com/w/cpp/17"><img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17"></a>
  <a href="#安装"><img src="https://img.shields.io/badge/header--only-yes-green.svg" alt="纯头文件"></a>
  <a href="CHANGELOG.zh-CN.md"><img src="https://img.shields.io/badge/version-2.1.0-blue.svg" alt="版本"></a>
</p>

<p align="center">
  <a href="README.md">English</a> | <strong>简体中文</strong>
</p>

---

## 概述

**BitCal** 是一个现代化的 C++17 纯头文件位运算库，提供高性能位操作功能并支持自动 SIMD 加速。通过 `if constexpr` 实现编译期分派，BitCal 相比标量实现可提供高达 **6 倍的性能提升**，同时保持零运行时开销。

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

| 特性 | 描述 | 性能影响 |
|------|------|---------|
| 🚀 **SIMD 加速** | 自动选择 SSE2/AVX2 (x86) 或 NEON (ARM) | 最高快 6 倍 |
| ⚡ **零开销** | 通过 `if constexpr` 编译期分派 | 无运行时成本 |
| 📦 **纯头文件** | 单个 `#include <bitcal/bitcal.hpp>` | 零依赖 |
| 🔧 **丰富的 API** | 位运算、位移、popcount、CLZ/CTZ、位反转、ANDNOT | 生产就绪 |
| 🌍 **跨平台** | Linux、Windows、macOS，支持 x86-64 和 ARM | 通用支持 |
| 🏎️ **类型安全** | 编译期位宽验证 | 尽早捕获错误 |

<details>
<summary>📋 目录</summary>

- [安装](#安装)
- [快速开始](#快速开始)
- [API 概览](#api-概览)
- [使用场景](#使用场景)
- [性能](#性能)
- [文档](#文档)
- [平台支持](#平台支持)
- [构建说明](#构建说明)
- [项目结构](#项目结构)
</details>

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
    GIT_TAG v2.1.0
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
    uint64_t* data = a.data();   // 64 字节对齐的指针
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
| `bitcal::bit1024` | 1024 位 | 16 | 超大批次位运算 |

自定义位宽（64 的倍数）：
```cpp
bitcal::bitarray<2048> custom;
```

## 📖 API 概览

### 构造函数
```cpp
bitcal::bit256 a;                    // 初始化为零
bitcal::bit256 b(0xDEADBEEF);        // 设置低 64 位
bitcal::bit256 c = b;                // 拷贝
```

### 位运算操作符
```cpp
a & b    // 与运算        a &= b    // 与运算并赋值
a | b    // 或运算        a |= b    // 或运算并赋值
a ^ b    // 异或          a ^= b    // 异或并赋值
~a       // 非运算
a.andnot(b)  // 优化的 a & ~b
```

### 位移操作符
```cpp
a << n   // 左移          a <<= n   // 左移并赋值
a >> n   // 右移          a >>= n   // 右移并赋值
```

### 位计数
```cpp
a.popcount()              // 统计置位数量
a.count_leading_zeros()   // 前导零计数
a.count_trailing_zeros()  // 尾随零计数
```

### 单比特操作
```cpp
a.get_bit(index)          // 读取位（从 LSB 开始 0 索引）
a.set_bit(index, value)   // 设置/清零位
a.flip_bit(index)         // 翻转位
```

### 状态与比较
```cpp
a.is_zero()               // 检查是否全为零
a.clear()                 // 清零
a == b                    // 相等比较
a != b                    // 不等比较
```

### 数据访问
```cpp
a.data()                  // 64 位字数组指针
a[index]                  // 访问第 index 个 64 位字
a.bits                    // 编译期位宽常量
a.u64_count               // 64 位字数量（编译期常量）
```

### 位操作
```cpp
a.reverse()               // 位序反转
```

### 强制指定后端
```cpp
bitcal::bitarray<256, bitcal::simd_backend::avx2>  强制_avx2;
bitcal::bitarray<256, bitcal::simd_backend::sse2>  强制_sse2;
bitcal::bitarray<256, bitcal::simd_backend::neon>  强制_neon;
bitcal::bitarray<256, bitcal::simd_backend::scalar> 强制_标量;
```

## 💡 使用场景

BitCal 在需要高性能位操作的场景中表现出色：

| 使用场景 | 描述 | 优势 |
|----------|------|------|
| **位集合** | 压缩布尔存储 | 相比 `bool[]` 空间减少 64 倍 |
| **布隆过滤器** | 概率数据结构 | SIMD 加速哈希混合 |
| **网络掩码** | CIDR/子网计算 | 大掩码快速与或操作 |
| **密码学** | 分组密码位运算 | 硬件 popcount 用于汉明距离 |
| **数据压缩** | 位打包/解包 | 高效跨字位移 |
| **图形处理** | 掩码操作 | 像素掩码并行处理 |

查看 [`examples/`](examples/) 获取具体实现。

```cpp
// 示例：快速 CIDR 掩码生成
bitcal::bit256 make_mask(int prefix_len) {
    bitcal::bit256 mask;
    mask.clear();
    for (int i = 0; i < prefix_len; ++i) {
        mask.set_bit(255 - i, true);
    }
    return mask;
}
```

## 📊 性能

### 编译器优化选项

为获得最佳性能，使用以下编译器选项：

```bash
# GCC/Clang
g++ -std=c++17 -O3 -march=native -DNDEBUG

# MSVC
cl /std:c++17 /O2 /arch:AVX2 /DNDEBUG
```

| 选项 | 作用 |
|------|------|
| `-O3` | 最大优化级别 |
| `-march=native` | 启用所有 CPU 特性 |
| `-DNDEBUG` | 发布模式移除断言 |

### 性能测试结果

#### Intel Core i7-12700K @ 3.6GHz (AVX2)

| 运算 | 标量 | AVX2 | 加速比 |
|------|------|------|--------|
| 与运算-256 | 12.3 ns | 2.1 ns | **5.9×** |
| 异或运算-512 | 24.8 ns | 4.3 ns | **5.8×** |
| 左移-256 | 18.6 ns | 5.2 ns | **3.6×** |
| 人口计数-512 | 45.2 ns | 22.3 ns | **2.0×** |
| is_zero-256 | 4.5 ns | 1.8 ns | **2.5×** |

#### ARM Cortex-A72 @ 2.0GHz (NEON)

| 运算 | 标量 | NEON | 加速比 |
|------|------|------|--------|
| 与运算-128 | 8.4 ns | 3.2 ns | **2.6×** |
| 异或运算-256 | 16.9 ns | 6.8 ns | **2.5×** |
| 左移-128 | 12.3 ns | 6.8 ns | **1.8×** |

> 📈 在你的环境中运行 `./benchmarks/bench_bitcal` 查看实际数据。

### SIMD 后端选择

BitCal 根据编译器选项自动选择最佳后端：

| 平台 | 后端 | 位宽 | 触发条件 |
|------|------|------|----------|
| x86-64 | AVX2 | 256 位 | `-mavx2` 可用 |
| x86-64 | SSE2 | 128 位 | `-msse2` 可用 |
| ARM | NEON | 128 位 | ARMv7-A+ 或 ARM64 |
| 任意 | 标量 | 64 位 | 回退方案 |

## 📚 文档

### API 参考

| 主题 | 描述 |
|------|------|
| [类型](docs/zh/api/types.md) | `bitarray` 模板和类型别名 |
| [核心运算](docs/zh/api/core-operations.md) | 与、或、异或、非、ANDNOT |
| [位移操作](docs/zh/api/shift-operations.md) | 左移和右移 |
| [位计数](docs/zh/api/bit-counting.md) | popcount、CLZ、CTZ |
| [位操作](docs/zh/api/bit-manipulation.md) | 获取/设置/翻转位、反转 |
| [SIMD 后端](docs/zh/api/simd-backend.md) | 后端选择 |
| [Ops 命名空间](docs/zh/api/ops-namespace.md) | 底层 API |

### 架构设计

- [概述](docs/zh/architecture/overview.md) - 设计原则
- [SIMD 分派](docs/zh/architecture/simd-dispatch.md) - 编译期选择
- [平台支持](docs/zh/architecture/platform-support.md) - 兼容性矩阵

### 入门指南

- [安装指南](docs/zh/getting-started/installation.md)
- [快速开始](docs/zh/getting-started/quickstart.md)
- [构建选项](docs/zh/getting-started/build-options.md)

完整文档：[https://lessup.github.io/bitcal/](https://lessup.github.io/bitcal/)

## 🌍 平台支持

| 平台 | 架构 | 编译器 | SIMD | CI 状态 |
|------|------|--------|------|---------|
| Linux | x86-64 | GCC 7+, Clang 6+ | SSE2/AVX2 | ✅ 已验证 |
| Linux | ARM64 | GCC（交叉编译） | NEON | ✅ 已验证 |
| Linux | ARM32 | GCC（交叉编译） | NEON | ✅ 已验证 |
| Windows | x86-64 | MSVC 2017+ | SSE2/AVX2 | ✅ 已验证 |
| macOS | x86-64 | Apple Clang | SSE2/AVX2 | ✅ 已验证 |
| macOS | ARM64 (Apple Silicon) | Apple Clang | NEON | ✅ 已验证 |

### 系统要求

- **C++17** 或更高版本
- **CMake 3.16+**（仅用于构建测试/基准测试）
- **支持的编译器**：GCC 7+, Clang 6+, MSVC 2017+

## 🔨 构建说明

### 构建测试和基准测试

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 运行测试
./tests/test_bitcal

# 运行基准测试
./benchmarks/bench_bitcal
```

### 系统范围安装

```bash
cmake --install . --prefix /usr/local
```

## 🏗️ 项目结构

```
bitcal/
├── include/bitcal/       # 头文件（纯头文件库）
│   ├── bitcal.hpp        # 主头文件（包含这个）
│   ├── config.hpp        # 平台检测
│   ├── simd_traits.hpp   # SIMD 类型特征
│   ├── scalar_ops.hpp    # 标量回退实现
│   ├── sse_ops.hpp       # SSE2 实现
│   ├── avx_ops.hpp       # AVX2 实现
│   ├── avx512_ops.hpp    # AVX-512 支持
│   └── neon_ops.hpp      # NEON 实现
├── docs/                 # 文档
│   ├── en/               # 英文文档
│   └── zh/               # 中文文档
├── tests/                # 单元测试
├── benchmarks/           # 性能基准测试
└── examples/             # 示例程序
```

### 内存布局

```
bitarray<256> 内存布局：
┌──────────────────────────────────────────────────────────┐
│ 对齐 (64字节) │ 字 0 │ 字 1 │ 字 2 │ 字 3 │ 填充至 64B │
│ 64 字节       │ 0-63 │64-127│128-191│192-255│          │
└──────────────────────────────────────────────────────────┘
```

- **对齐方式：** 64 字节（缓存行对齐）
- **字节序：** 小端（LSB 在 `data()[0]`）
- **存储：** 连续的 `uint64_t` 数组

## 📝 更新日志

详见 [CHANGELOG.zh-CN.md](CHANGELOG.zh-CN.md) 了解版本历史。

### 最新版本：v2.1.0（2026-04-16）

- ✨ **新增：** 原生 SIMD 指令的 ANDNOT 运算
- ⚡ **性能：** `is_zero()` 提速最高 2.3×，`~` 提速 1.7×
- 🧪 **测试：** bit1024 完整测试覆盖
- 🔧 **基础设施：** ARM32 CI 支持

## 🤝 参与贡献

欢迎贡献！请参阅 [CONTRIBUTING.md](CONTRIBUTING.md) 了解贡献指南。

## 📄 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。

## 🙏 致谢

- 设计灵感来源于 [Boost.SIMD](https://github.com/boostorg/simd) 和 [xsimd](https://github.com/xtensor-stack/xsimd)
- 性能测试使用 [Google Benchmark](https://github.com/google/benchmark)

---

<p align="center">
  <a href="https://github.com/LessUp/bitcal">⭐ GitHub 星标</a> •
  <a href="https://github.com/LessUp/bitcal/issues">🐛 提交问题</a> •
  <a href="https://github.com/LessUp/bitcal/discussions">💬 讨论区</a>
</p>
