# BitCal

[![CI](https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/bitcal/actions/workflows/ci.yml)
[![Docs](https://github.com/LessUp/bitcal/actions/workflows/pages.yml/badge.svg)](https://lessup.github.io/bitcal/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)](#)

**现代化、跨平台的高性能位运算加速库，支持 SIMD 指令集自动选择。**

[English](README.md) | 简体中文

---

## 特性

- 🚀 **SIMD 加速** — 自动选择 SSE2/AVX/AVX2/NEON 指令集
- ⚡ **零开销抽象** — C++17 `if constexpr` 编译期分派
- 📦 **Header-only** — 只需 `#include <bitcal/bitcal.hpp>`
- 🔧 **丰富 API** — 位运算、位移、popcount、CLZ/CTZ、位反转、ANDNOT
- 🌍 **跨平台** — Linux、Windows、macOS，支持 x86 和 ARM

## 快速开始

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    // 位运算
    auto c = a & b;           // AND
    auto d = a | b;           // OR
    auto e = a ^ b;           // XOR
    auto f = ~a;              // NOT
    auto g = a.andnot(b);     // a & ~b（原生 SIMD）

    // 位移
    a <<= 10;                 // 左移
    b >>= 5;                  // 右移

    // 位计数
    uint64_t ones = a.popcount();
    int lz = a.count_leading_zeros();
    int tz = a.count_trailing_zeros();

    // 位操作
    a.set_bit(42, true);      // 设置第 42 位
    bool bit = a.get_bit(42); // 读取第 42 位
    a.flip_bit(42);           // 翻转第 42 位
    a.reverse();              // 位反转

    return 0;
}
```

## 安装

### 方法 1：Header-only（推荐）

```bash
cp -r include/bitcal /path/to/your/project/include/
```

```cpp
#include <bitcal/bitcal.hpp>
```

### 方法 2：CMake 集成

```bash
git clone https://github.com/LessUp/bitcal.git
cd bitcal && mkdir build && cd build
cmake .. && make install
```

```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target bitcal::bitcal)
```

## API 参考

### 类型别名

| 类型 | 位宽 | 存储 |
|------|------|------|
| `bitcal::bit64` | 64 位 | 1 × `uint64_t` |
| `bitcal::bit128` | 128 位 | 2 × `uint64_t` |
| `bitcal::bit256` | 256 位 | 4 × `uint64_t` |
| `bitcal::bit512` | 512 位 | 8 × `uint64_t` |
| `bitcal::bit1024` | 1024 位 | 16 × `uint64_t` |

### 核心操作

| 操作 | 说明 |
|------|------|
| `a & b`, `a \| b`, `a ^ b`, `~a` | 位运算 AND、OR、XOR、NOT |
| `a.andnot(b)` | `a & ~b`，使用原生 SIMD 指令 |
| `a << n`, `a >> n` | 左移/右移 |
| `a.popcount()` | 统计 1 的个数 |
| `a.count_leading_zeros()` | 前导零计数 (CLZ) |
| `a.count_trailing_zeros()` | 尾部零计数 (CTZ) |
| `a.reverse()` | 位反转 |
| `a.is_zero()` | 零检测 |
| `a.get_bit(i)`, `a.set_bit(i, v)`, `a.flip_bit(i)` | 单个位操作 |

### SIMD 后端

```cpp
enum class simd_backend {
    scalar,   // 通用标量实现
    sse2,     // x86 SSE2
    avx,      // x86 AVX
    avx2,     // x86 AVX2
    avx512,   // x86 AVX-512（回退到 AVX2）
    neon      // ARM NEON
};
```

强制指定后端：
```cpp
bitcal::bitarray<256, bitcal::simd_backend::avx2> avx2_array;
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable_array;
```

## 性能

Intel Core i7-12700K @ 3.6GHz（AVX2 后端）：

| 操作 | 标量 | AVX2 | 加速比 |
|------|------|------|--------|
| AND-256 | 12.3 ns | 2.1 ns | **5.9×** |
| XOR-512 | 24.8 ns | 4.3 ns | **5.8×** |
| ShiftLeft-256 | 18.6 ns | 5.2 ns | **3.6×** |
| Popcount-512 | 45.2 ns | 22.3 ns | **2.0×** |

ARM Cortex-A72 @ 2.0GHz（NEON 后端）：

| 操作 | 标量 | NEON | 加速比 |
|------|------|------|--------|
| AND-128 | 8.4 ns | 3.2 ns | **2.6×** |
| XOR-256 | 16.9 ns | 6.8 ns | **2.5×** |

## 平台支持

| 平台 | 架构 | 编译器 | 状态 |
|------|------|--------|------|
| Linux | x86-64 | GCC 7+, Clang 6+ | ✅ CI 验证 |
| Linux | ARM64 | GCC（交叉编译） | ✅ CI 验证 |
| Linux | ARM32 | GCC（交叉编译） | ✅ CI 验证 |
| Windows | x86-64 | MSVC 2017+ | ✅ CI 验证 |
| macOS | x86-64 | Apple Clang | ✅ CI 验证 |
| macOS | ARM64 | Apple Clang | ✅ CI 验证 |

## 项目结构

```
bitcal/
├── include/bitcal/           # 头文件
│   ├── config.hpp            # 平台检测和宏定义
│   ├── simd_traits.hpp       # SIMD 类型特征
│   ├── scalar_ops.hpp        # 标量实现
│   ├── sse_ops.hpp           # SSE2 实现
│   ├── avx_ops.hpp           # AVX2 实现
│   ├── neon_ops.hpp          # NEON 实现
│   └── bitcal.hpp            # 主头文件
├── tests/                    # 单元测试
├── benchmarks/               # 性能基准测试
├── examples/                 # 示例代码
├── gitbook/                  # 文档
└── changelog/                # 变更日志
```

## 系统要求

- C++17 或更高版本
- CMake 3.16+（仅用于构建测试/示例）
- 支持的编译器：GCC 7+、Clang 6+、MSVC 2017+

## 文档

- [快速上手](gitbook/getting-started/quickstart.md)
- [API 参考](gitbook/api/types.md)
- [架构设计](gitbook/architecture/overview.md)
- [迁移指南](MIGRATION_GUIDE.md)

## 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。

## 贡献

欢迎贡献！请随时提交 issue 或 pull request。

## 致谢

- 设计灵感来源于 Boost.SIMD 和 xsimd
- 使用 Google Benchmark 进行性能测试（可选）
