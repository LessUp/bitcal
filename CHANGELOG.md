# 更新日志

本项目所有重要变更都将记录在此文件中。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
本项目遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

---

## [未发布]

### 💥 破坏性变更

- 后端契约收敛为 `scalar` 与 `avx2`；移除 `sse2` / `avx512` 的保留公开承诺
- 从活动主流程移除 OpenSpec-first 资产（`openspec/`、opsx 命令集）
- 移除 `backend_kind` enum 与 `default_backend()`（不存在运行时选择）；替换为编译期 `active_backend_name` 字符串常量
- `shift_left<Bits>()` / `shift_right<Bits>()` 的 count 参数从 `int` 改为 `size_t`（负数不再静默视为零移位）
- 删除 `vcpkg.json`（声明 google-benchmark 但 benchmark harness 自研 `std::chrono`，`install-features` 为死配置）

### 🐛 修复

- `get_optimal_alignment<Bits>()` 增加 `BITCAL_HAS_AVX2` 条件：仅在 `Bits >= 256 && BITCAL_HAS_AVX2` 时返回 32 字节对齐。此前 scalar build 下 `bit_block<256>` 也强制 32 字节对齐，与"scalar 路径用自然对齐以避免死零初始化存储"的注释自相矛盾
- `detail::shift_left_array` 反向循环改用 `i < N` 守护（与 `shift_right_array` 一致）：消除 `word_shift == 0` 时 `size_t` 下溢导致死循环的潜在隐患

### ♻️ 重构

- 公开 in-place 位运算 API 补齐：`or_into()`、`xor_into()`、`andnot_into()`（与 `and_into()` 并列）
- 删除死 SIMD 与宏层：`avx_ops.hpp`、`avx512_ops.hpp`、`sse_ops.hpp`、`neon_ops.hpp`、`scalar_ops.hpp`、`backend_ops.def`、`detail/backend.hpp`
- `*_into` 与 `bit_*` 二元算法统一为经 `binary_words` 的单一分派路径
- 移位 `count >= Bits` 时短路返回零 block（不再先 copy 再清零）
- scalar 辅助函数移除 `BITCAL_FORCEINLINE`（仅 AVX2 分派路径保留）
- 测试收敛为单套 `test_bitcal`，删除 `test_bitcal_detail`
- `bit_view` 解耦为独立类，存 `uint64_t*` 并提供 `operator const_bit_view()` 隐式转换；移除原继承基类 + `const_cast` 恢复可变性的微妙写法
- `popcount` / `is_zero` / `equals` 走 AVX2 分派（运行时），`if consteval` 保留 scalar constexpr 路径
- `detail/scalar_impl.hpp` 与 `detail/x64_dispatch.hpp` 合并入 `detail/word_ops.hpp`，detail 层从三文件收敛为单文件
- 全量 `clang-format` 应用于 `include/`、`tests/`、`examples/`、`benchmarks/`，CI 增加 `clang-format --dry-run --Werror` 步骤

### ⚡ 性能

- AVX2 加速 `popcount`（LUT + `_mm256_sad_epu8`）、`is_zero`（`_mm256_testz_si256`）、`equals`（`_mm256_cmpeq_epi64` + `_mm256_testc_si256`），>= 4 字块走 SIMD + scalar 尾部回退

### 🧪 测试

- 新增 `bit_or` / `bit_xor` / `bit_andnot` 的 128 位确定性测试矩阵
- 新增 `shift_left(0)` / `shift_right(0)` 空操作回归测试
- 新增 shift 边界用例：`shift(Bits-1)` 单 bit 保留、`shift(Bits+1)` 超宽短路、`shift(SIZE_MAX)` 极端 count
- 新增 `popcount` / `is_zero` / `equals` 的 constexpr `static_assert` 覆盖
- `test_vnext_block_storage_alignment` 断言条件从 `BITCAL_ARCH_X86` 改为 `BITCAL_HAS_AVX2`，与 `get_optimal_alignment` 新逻辑一致

### ⚙️ 构建

- `BITCAL_ARCH_X86` 排除 32 位 x86（`__i386__` / `_M_IX86`）；项目为 x86-64-first
- `<immintrin.h>` 仅在 `BITCAL_HAS_AVX2` 时引入（非 AVX2 TU 编译更快）
- 不再跟踪 `benchmarks/results/`（加入 `.gitignore`）；结果本地重新生成
- 移除 `changelog/` fragment 目录（唯一事实源为根 `CHANGELOG.md`）
- 清理 `.gitignore`：删除 `build_arm64/`、`node_modules/`、`_site/`、`_book/`（honkit 已删）、`.windsurf/`、`_bmad/`、`_bmad-output/`、`.omc/`（AI 工具残留）

### ⚙️ CI 与工具链

- CI 矩阵降到最小保留集，并移除 `openspec/**` 触发
- CI 增加 scalar 路径 matrix job（`-mno-avx2` + `BITCAL_NATIVE_ARCH=OFF`），覆盖 `BITCAL_HAS_AVX2 == 0` 分支
- CI 增加 `clang-format` dry-run 检查 job
- CI 触发路径列表移除 `vcpkg.json`，新增 `README.en.md` 与 `.clang-format`

### 📚 文档

- 新增 `README.en.md`：英文版面向贡献者与设计评审，风格与中文版独立（不逐句对应）
- README 后端与 API 描述改为与实现一致
- README 增加 scalar build 命令、契约说明（`equals` 容忍宽度不一致，其他算法 Release 下宽度不一致为 UB）、benchmark_compare 用途说明
- 重写项目治理文档（`AGENTS.md`）为轻流程策略
- 删除低价值白皮书附加资产（`design-evolution` 页面、`docs/diagrams/`）
- 删除 VitePress 文档站点（`docs/` 全部）、`PRODUCT.md`、`CONTRIBUTING.md`、issue/PR 模板、`release.yml` / `docs-pages.yml` CI、`CLAUDE.md` / copilot 指令（并入 `AGENTS.md`）
- README / CHANGELOG 单语化（中文为准），删除 `.zh-CN` 镜像

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
- 自由算法：`bit_and<Bits>()`、`bit_into()`、`is_zero()`、`popcount()`
- `and_into()` 内核的 AVX2 快速路径
- 基于 span 的字导入/导出辅助函数

### 🔧 内部架构

```
bitcal/bitcal.hpp          # 唯一稳定公开入口
├── bit_block.hpp          # 拥有型存储
├── bit_view.hpp           # 非拥有型视图
├── algorithms.hpp         # 公开自由算法
└── detail/
    └── word_ops.hpp       # scalar 内核 + x86-64 分派 + 视图层字操作
```

### 📊 性能基线

本版本建立 x86-64 benchmark 基线（结果本地生成、不入库，运行 `benchmarks/` 可执行文件复现）；ARM 路径暂无 benchmark。

### 🔗 链接

- **完整变更日志**: [v3.0.0...v4.0.0](https://github.com/LessUp/bitcal/compare/v3.0.0...v4.0.0)


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

### 🔗 链接

- **完整变更日志**: [v2.0.0...v2.1.0](https://github.com/LessUp/bitcal/compare/v2.0.0...v2.1.0)
- **下载**: [v2.1.0 发布](https://github.com/LessUp/bitcal/releases/tag/v2.1.0)

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
