# 更新日志

<p align="center">
  <a href="CHANGELOG.md">English</a> | <strong>简体中文</strong>
</p>

本项目所有重要变更都将记录在此文件中。

当前带 benchmark 证据的性能数据统一以 [/zh/performance/](https://lessup.github.io/bitcal/zh/performance/) 为准。下面历史版本中出现的性能数字只保留为归档说明，不再作为当前 retained 证据链。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
本项目遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

---

## [未发布]

## [4.0.0] - 2026-05-15

### 🚀 亮点

本版本引入 **BitCal vNext**，基于 C++23 的全新三层架构重设计：

1. **拥有型存储** 通过 `bit_block<Bits>`
2. **非拥有型访问** 通过 `bit_view` / `const_bit_view`
3. **自由算法** 如 `bit_and<Bits>()` 和 `and_into()`

### ⚠️ 破坏性变更

- **语言基线升级至 C++23**（从 C++17）
- **新公开模型**：`bit_block` + `bit_view` + 自由算法替代旧的 `bitarray` 中心模型
- **无兼容层**：旧 `bitarray` API 不再保留
- **平台重心**：x86-64 现为首要优化目标

### ✨ 新增功能

- `bit_block<Bits>` — 拥有型定宽存储，x86-64 上保证 32 字节对齐
- `bit_view` / `const_bit_view` — 用于外部存储的非拥有型字视图
- 自由算法：`bit_and<Bits>()`、`and_into()`、`is_zero()`、`popcount()`
- `and_into()` 内核的 AVX2 快速路径
- 基于 span 的字导入/导出辅助函数

### 📚 文档

- 新增 vNext 技术白皮书
- 性能基线文档

### 🔧 内部架构

```
bitcal/bitcal.hpp          # 唯一稳定公开入口
├── bit_block.hpp          # 拥有型存储
├── bit_view.hpp           # 非拥有型视图
├── algorithms.hpp         # 公开自由算法
└── detail/
    ├── backend.hpp        # 后端标签
    └── x64_dispatch.hpp   # x86-64 分派层
```

### 📊 性能基线

本版本建立当前 retained 的 x86-64 benchmark baseline；ARM 数据继续留空，直到项目拥有同等级的 retained benchmark 路径。

当前提交进仓库的表格、benchmark 元数据与复现方式请统一查看 [/zh/performance/](https://lessup.github.io/bitcal/zh/performance/)，不要把 changelog 当作当前性能数字的事实来源。

### 🔗 链接

- **完整变更日志**: [v3.0.0...v4.0.0](https://github.com/LessUp/bitcal/compare/v3.0.0...v4.0.0)
- **vNext 白皮书**: https://lessup.github.io/bitcal/zh/whitepaper/
- **性能基线**: https://lessup.github.io/bitcal/zh/performance/


## [3.0.0] - 2026-05-08

### ⚠️ 破坏性变更

- 从受支持的公开 API 表面移除了 `bitcal::ops` 命名空间
- 移除了公开 traits：`is_bitarray`、`is_bitarray_v`、`bitarray_traits`
- 移除了公开契约中的显式 `bit64` 转换便捷入口
- 文档已收敛为仅描述保留的 `bitarray` 公开 API

### 🔁 迁移说明

- 将 `bitcal::ops` 调用替换为 `bitarray` 成员函数，例如 `popcount()`、`count_leading_zeros()`、`count_trailing_zeros()`、`andnot()`、`reverse()`
- 对已有 `uint64_t` 缓冲区使用 `set_word()` 赋值，使用 `word()` / `operator[]` 做只读提取
- 删除对已移除公开 traits 的依赖，直接使用文档化的类型别名或模板参数

### 📦 打包

- 仓库安装示例与徽章已统一固定到 `v3.0.0`
- `vcpkg.json` 包版本已更新为 `3.0.0`

### 🔗 链接

- **完整变更日志**: [v2.1.0...v3.0.0](https://github.com/LessUp/bitcal/compare/v2.1.0...v3.0.0)
- **下载**: [v3.0.0 发布](https://github.com/LessUp/bitcal/releases/tag/v3.0.0)
- **文档**: https://lessup.github.io/bitcal/zh/


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
- **文档**: https://lessup.github.io/bitcal/zh/

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

### ⚠️ 迁移说明

v1.x **不再维护**。用户应迁移到 v2.x。

---

## [1.x] - 旧版

基于继承和虚函数的原始实现。

**状态**：不再维护。用户应迁移到 v2.x。

---

## 版本历史

| 版本 | 日期 | 状态 | 亮点 |
|------|------|------|------|
| v4.0.0 | 2026-05-15 | ✅ 稳定 | vNext C++23 重设计，全新三层架构 |
| v3.0.0 | 2026-05-08 | ✅ 稳定 | 公开接口收口、迁移说明 |
| v2.1.0 | 2026-04-16 | ✅ 稳定 | ANDNOT，性能提升 |
| v2.0.0 | 2026-01-08 | ✅ 稳定 | 完全重写，纯头文件 |
| v1.x | 2025 | ⚠️ 旧版 | 基于继承的设计 |

---

[未发布]: https://github.com/LessUp/bitcal/compare/v4.0.0...HEAD
[4.0.0]: https://github.com/LessUp/bitcal/compare/v3.0.0...v4.0.0
[3.0.0]: https://github.com/LessUp/bitcal/compare/v2.1.0...v3.0.0
[2.1.0]: https://github.com/LessUp/bitcal/compare/v2.0.0...v2.1.0
[2.0.0]: https://github.com/LessUp/bitcal/releases/tag/v2.0.0
