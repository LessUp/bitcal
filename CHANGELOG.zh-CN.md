# 更新日志

<p align="center">
  <a href="CHANGELOG.md">English</a> | <strong>简体中文</strong>
</p>

本项目所有重要变更都将记录在此文件中。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
本项目遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

---

## [未发布]

## [2.1.0] - 2026-04-16

### 🚀 亮点

- **新增：** 全 SIMD 后端原生 ANDNOT 运算
- **性能：** `is_zero()` 提速最高 2.3×，`~` 提速 1.7×，`reverse()` 提速 1.4×
- **测试：** bit1024 完整测试覆盖
- **基础设施：** ARM32 CI 支持，CMake 标志自动检测

### ✨ 新增功能

- **ANDNOT 运算** — 使用原生 SIMD 指令的 `bitarray::andnot(mask)`
  - SSE: `_mm_andnot_si128`
  - AVX: `_mm256_andnot_si256`  
  - NEON: `vbicq_u64`
- **统一二元运算分派** — `dispatch_binop<Op>` 模板，减少约 40% 代码重复
- **bit1024 单元测试** — 1024 位运算的完整测试覆盖
- **基准测试目录** — 基于 Google Benchmark 的性能测试套件
- **文档改进** — 专业级双语（英文/中文）文档

### ⚡ 性能提升

| 优化项 | v2.0 | v2.1 | 提升 |
|--------|------|------|------|
| `operator~` | 标量回退 | SIMD 非运算 | **1.7×** (256 位) |
| `is_zero()` | 多指令 | 单指令 | **2.3×** (256 位) |
| `clear()` | 循环实现 | `std::memset` | **1.9×** (1024 位) |
| `reverse()` | 使用临时数组 | 原地操作 | **1.4×** (256 位) |

### 🔧 错误修复

| 问题 | 平台 | 解决方案 |
|------|------|---------|
| MSVC SSE2 检测 | Windows | 添加 `_M_X64` 检查 + `<intrin.h>` |
| NEON NOT 运算 | ARM | `vmvnq_u64` → `veorq_u64`（兼容性） |
| NEON 变位移 | ARM | `vshlq_n_u64` → `vshlq_u64`（运行时支持） |
| SSE 256 位位移进位 | x86 | 修复 4 字进位链 |
| AVX 256/512 位位移 | x86 | 修复跨通道进位传播 |
| 包含顺序 | 所有 | 内部函数必须在命名空间外 |

### 🏗️ 基础设施

- 添加 ARM32 交叉编译 CI 作业
- CMake 自动检测 SIMD 编译器标志
- 专业级文档重构（中英文）

### 🔗 链接

- **完整变更日志**: [v2.0.0...v2.1.0](https://github.com/LessUp/bitcal/compare/v2.0.0...v2.1.0)
- **下载**: [v2.1.0 发布](https://github.com/LessUp/bitcal/releases/tag/v2.1.0)
- **文档**: [文档入口](docs/zh/index.html)

---

## [2.0.0] - 2026-01-08

### 🔄 完全重写

这是一次从面向对象继承设计到现代 C++17 模板设计的**完全架构重构**。

### 🏗️ 架构变更

| 方面 | v1.x | v2.0 |
|------|------|------|
| 设计模式 | 继承 + 虚函数 | 模板 + `if constexpr` |
| 分派方式 | 运行时 | 编译期 |
| 依赖项 | 外部（spdlog） | 无（纯头文件） |
| API 风格 | 单例模式 | 值类型 + 运算符 |

### ✨ 新增功能

- **纯头文件设计** — 零编译依赖
- **编译期 SIMD 选择** — 通过 `if constexpr` 自动选择后端
- **完整 ARM NEON 支持** — ARM 上的 128/256/512 位运算
- **运算符重载** — `&`, `|`, `^`, `~`, `<<`, `>>`
- **位计数** — `popcount()`, `count_leading_zeros()`, `count_trailing_zeros()`
- **位操作** — `get_bit()`, `set_bit()`, `flip_bit()`, `is_zero()`, `clear()`, `reverse()`
- **类型别名** — `bit64`, `bit128`, `bit256`, `bit512`, `bit1024`
- **底层 API** — 面向原始指针操作的 `ops` 命名空间

### 📊 性能对比

| 平台 | 运算 | 标量 | SIMD | 加速比 |
|------|------|------|------|--------|
| x86 AVX2 | 与运算-256 | 12.3 ns | 2.1 ns | **5.9×** |
| x86 AVX2 | 异或运算-512 | 24.8 ns | 4.3 ns | **5.8×** |
| ARM NEON | 与运算-128 | 8.4 ns | 3.2 ns | **2.6×** |

### ❌ 移除项

- 虚函数开销
- 单例模式
- 运行时类型检查
- 外部依赖（spdlog）

### ✅ 平台支持

- Linux (x86-64, ARM64, ARM32)
- Windows (x86-64, MSVC 2017+)
- macOS (x86-64, ARM64 Apple Silicon)

### 🔗 链接

- **完整变更日志**: [v1.0...v2.0.0](https://github.com/LessUp/bitcal/releases/tag/v2.0.0)
- **迁移指南**: [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)

### ⚠️ 迁移说明

v1.x **不再维护**。用户应迁移到 v2.x。

详见 [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) 了解详细迁移指南。

---

## [1.x] - 旧版

基于继承和虚函数的原始实现。

**状态**：不再维护。用户应迁移到 v2.x。

---

## 版本历史

| 版本 | 日期 | 状态 | 亮点 |
|------|------|------|------|
| v2.1.0 | 2026-04-16 | ✅ 稳定 | ANDNOT，性能提升 |
| v2.0.0 | 2026-01-08 | ✅ 稳定 | 完全重写，纯头文件 |
| v1.x | 2025 | ⚠️ 旧版 | 基于继承的设计 |

---

[未发布]: https://github.com/LessUp/bitcal/compare/v2.1.0...HEAD
[2.1.0]: https://github.com/LessUp/bitcal/compare/v2.0.0...v2.1.0
[2.0.0]: https://github.com/LessUp/bitcal/releases/tag/v2.0.0
