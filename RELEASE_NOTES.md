## 🚀 BitCal v2.1.0 Release Notes

### Overview

BitCal v2.1.0 is a significant performance improvement release focused on SIMD optimizations and bug fixes. This release maintains full backward compatibility with v2.0.x while delivering substantial performance gains.

---

### ✨ Highlights

#### New Features
- **ANDNOT Operation** — Native SIMD instruction for `a & ~b` operations
  - Up to 2× faster than separate NOT + AND operations
  - Supported on SSE2, AVX2, and NEON backends
- **Performance Benchmarks** — New comprehensive benchmarking suite using Google Benchmark
- **bit1024 Test Coverage** — Full unit test coverage for 1024-bit operations

#### Performance Improvements
| Operation | v2.0 | v2.1 | Speedup |
|-----------|------|------|---------|
| NOT (256-bit) | 3.5 ns | 2.1 ns | **1.67×** |
| is_zero (256-bit) | 4.2 ns | 1.8 ns | **2.33×** |
| reverse (256-bit) | 12.1 ns | 8.5 ns | **1.42×** |

- SIMD-optimized `operator~` for 128/256/512-bit operations
- Single-instruction `is_zero()` detection using `_mm256_testz`
- In-place `reverse()` eliminating temporary array allocation

#### Bug Fixes
- Fixed MSVC SSE2 macro detection on x64 platforms
- Fixed NEON `vmvnq_u64` compatibility issue (ARM)
- Fixed NEON variable shift support
- Fixed AVX2 cross-lane carry propagation in shift operations

---

### 📦 Installation

**Header-only (Recommended)**:
```bash
git clone https://github.com/LessUp/bitcal.git
cp -r bitcal/include/bitcal /path/to/your/project/include/
```

**CMake FetchContent**:
```cmake
include(FetchContent)
FetchContent_Declare(
    bitcal
    GIT_REPOSITORY https://github.com/LessUp/bitcal.git
    GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(bitcal)
target_link_libraries(your_target bitcal::bitcal)
```

Compile with optimizations:
```bash
g++ -std=c++17 -O3 -march=native your_program.cpp
```

---

### 📚 Documentation

- [Full Documentation](https://github.com/LessUp/bitcal/tree/v2.1.0/docs/en)
- [API Reference](https://github.com/LessUp/bitcal/tree/v2.1.0/docs/en/api)
- [Changelog](https://github.com/LessUp/bitcal/blob/v2.1.0/CHANGELOG.md)

---

### Compatibility

- **Minimum C++ Standard**: C++17
- **Breaking Changes**: None (backward compatible with v2.0.x)
- **Platforms**: Linux, Windows, macOS (x86-64, ARM64, ARM32)

---

### Full Changelog

See [CHANGELOG.md](https://github.com/LessUp/bitcal/blob/v2.1.0/CHANGELOG.md) for complete details.

---

## 🚀 BitCal v2.1.0 发行说明（中文版）

### 概述

BitCal v2.1.0 是一个专注于 SIMD 优化和 bug 修复的重要性能改进版本。此版本与 v2.0.x 完全向后兼容，同时提供了显著的性能提升。

---

### ✨ 亮点

#### 新增功能
- **ANDNOT 操作** — 用于 `a & ~b` 操作的原生 SIMD 指令
  - 比分开的 NOT + AND 操作快 2 倍
  - 支持 SSE2、AVX2 和 NEON 后端
- **性能基准测试** — 使用 Google Benchmark 的全新综合性能测试套件
- **bit1024 测试覆盖** — 1024 位操作的完整单元测试覆盖

#### 性能改进
| 操作 | v2.0 | v2.1 | 加速比 |
|------|------|------|--------|
| NOT (256 位) | 3.5 ns | 2.1 ns | **1.67×** |
| is_zero (256 位) | 4.2 ns | 1.8 ns | **2.33×** |
| reverse (256 位) | 12.1 ns | 8.5 ns | **1.42×** |

- 128/256/512 位操作使用 SIMD 优化的 `operator~`
- 使用 `_mm256_testz` 实现单指令 `is_zero()` 检测
- 原地 `reverse()` 消除临时数组分配

#### Bug 修复
- 修复 MSVC x64 平台 SSE2 宏检测问题
- 修复 NEON `vmvnq_u64` 兼容性问题（ARM）
- 修复 NEON 变量移位支持
- 修复 AVX2 跨 lane 进位传播的移位操作问题

---

### 📦 安装

**Header-only（推荐）**：
```bash
git clone https://github.com/LessUp/bitcal.git
cp -r bitcal/include/bitcal /path/to/your/project/include/
```

**CMake FetchContent**：
```cmake
include(FetchContent)
FetchContent_Declare(
    bitcal
    GIT_REPOSITORY https://github.com/LessUp/bitcal.git
    GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(bitcal)
target_link_libraries(your_target bitcal::bitcal)
```

带优化编译：
```bash
g++ -std=c++17 -O3 -march=native your_program.cpp
```

---

### 📚 文档

- [完整文档](https://github.com/LessUp/bitcal/tree/v2.1.0/docs/zh)
- [API 参考](https://github.com/LessUp/bitcal/tree/v2.1.0/docs/zh/api)
- [更新日志](https://github.com/LessUp/bitcal/blob/v2.1.0/CHANGELOG.zh-CN.md)

---

### 兼容性

- **最低 C++ 标准**：C++17
- **破坏性变更**：无（与 v2.0.x 向后兼容）
- **支持平台**：Linux、Windows、macOS（x86-64、ARM64、ARM32）

---

### 完整更新日志

详见 [CHANGELOG.zh-CN.md](https://github.com/LessUp/bitcal/blob/v2.1.0/CHANGELOG.zh-CN.md)。
