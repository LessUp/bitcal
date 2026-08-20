# 更新日志

本项目所有重要变更都将记录在此文件中。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
本项目遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

---

## [未发布]

### 💥 破坏性变更

- 平台支持收敛为 Linux x86-64（GCC / Clang）：移除 `config.hpp` 中 `_MSC_VER` / `_M_X64` 预处理分支与 `CMakeLists.txt` 中 MSVC `/arch:AVX2` 分支（CI 从未覆盖 Windows，benchmark harness 依赖 GCC/Clang 内联汇编与 `__VERSION__`，MSVC 路径为未经测试的死代码）
- `bit_block::from_words()` 与 `copy_words_to()` 参数从动态 `std::span<const std::uint64_t>` 改为静态 `std::span<const std::uint64_t, word_count>`：从 `std::array` 构造的调用获得编译期宽度保证；手动 `(ptr, count)` 构造仍可行但退化为 precondition UB（原 `assert` 保护移除）
- `BITCAL_BUILD_TESTS` / `BITCAL_BUILD_EXAMPLES` 默认值改为“仅顶层项目 ON”：以 `add_subdirectory` / FetchContent 集成 BitCal 的下游项目不再默认构建测试/示例（此前默认 ON 会污染下游目标空间）
- 返回型算法（`bit_*` / `shift_*`）视图形态从 `const_bit_view` 改为静态宽度 word range（`std::span<const uint64_t, N>` / `std::array` / C 数组，`static_word_range` concept 约束）：宽度从 extent 推导（无需手写 `<Bits>`），宽度错配从 Release UB 降级为编译错误；原 `bit_and<256>(view, view)` 形态移除

### ✨ 新增

- `bit_block` 增加 `words()` 访问器（静态 span，与 `from_words` / `copy_words_to` 对称），作为返回型算法静态宽度形态的入口

### 📚 文档

- `NOTES.md` 已知限制「变换类算法整体非 constexpr」缩窄为「仅 `bit_*` 非 constexpr」（`shift_*` 随融合内核化成为 constexpr；「不做 AVX2 shift」的跨 lane 复杂度理由保留）
- 版本历史表删除"状态"列（全标"✅ 稳定"与 README"实验性"定位矛盾）
- 4.0.0 亮点段移除已退役的"vNext"命名
- README 核心类型表补 512 宽度示例（测试与基准已覆盖）
- AGENTS.md 补充平台约束（§1）、注释原则（§4.4）、禁止事项（§8），修正 `cmake/` 过时引用
- `config.hpp` 对齐注释澄清：32 字节对齐是 `bit_block` 自身存储的保证，AVX2 分派路径用 unaligned `loadu`/`storeu`，对齐非正确性前提（原注释"AVX2 dispatch path loads/stores `__m256i`"暗示需 32 字节对齐，与实现不符）
- 审核文档收敛：`findings.md` / `progress.md` / `task_plan.md` 收敛为根 `NOTES.md`，只留跨版本的设计取舍与已知限制；过程性产物不再入库（已修复 bug 靠 CHANGELOG，实施过程靠 git log）；`AGENTS.md` §2.1 加 `NOTES.md` 指向
- `README.md` 后端段对齐措辞澄清：32 字节对齐是存储保证、非 `__m256i` 路径的正确性前提（与 NOTES/config.hpp 口径统一）
- `README.md` 契约说明补别名契约：`*_into` 内核逐字独立，`out` 可别名 `lhs` / `rhs`（含同一视图）；`copy_words_to` 支持自拷贝
- `NOTES.md` 新增「popcount 走 scalar 路径」设计取舍（含实测数据与原因）；已知限制澄清变换类算法（`bit_*` / `shift_*`）整体非 constexpr；`AGENTS.md` §4.3 同步 CI benchmark 编译覆盖
- `NOTES.md` 新增「亚纳秒基准在高负载机器上不可信」已知限制：记录 median-of-samples + 先测 BitCal 的顺序在负载波动下的系统性失真（含 `is_zero<512>` 摆动实证）与绑核 + min-of-N 复测方法；「popcount 走 scalar 路径」条目同步修正“残留差距”为测量伪影（绑核隔离实验证伪，4 路拆分累加器尝试无收益已回退）
- `README.md` 调用形态与契约说明重写为静态宽度形态；`NOTES.md` 视图宽度条目重写为「视图携带运行时宽度；返回型算法取静态宽度 range」

### ⚒️ 工程化收敛

- CI 新增 Clang build/test job（`CC=clang CXX=clang++`），与 GCC job 并行；AGENTS.md §4.3 同步修订为"format-check + GCC/Clang 两个 build/test job"
- `benchmark_bitcal.cpp` 防优化从 `volatile` 改为 `bitcal::bench::do_not_optimize`（asm barrier），消除不必要的内存写回对微基准测量的扭曲
- `.clang-format` Standard 从 `c++17` 改为 `Latest`（原值与 C++23 基线矛盾）
- `benchmarks/CMakeLists.txt` 删除冗余 `cmake_minimum_required`（子目录无需重复声明）
- 删除 benchmark 文件的 Doxygen `@file`/`@brief` 头（项目无 Doxygen 工具链）
- 删除 `examples/basic_usage.cpp` 与 `benchmarks/benchmark_compare.cpp` 中静态 span 改造后遗留的未使用 `#include <span>`（`from_words` 改为接收 `std::array` 隐式转换，调用方不再显式使用 span）
- CI 新增 scalar build/test job（`-mno-avx2` + `BITCAL_NATIVE_ARCH=OFF`），覆盖 `BITCAL_HAS_AVX2 == 0` 分支；此前 CI 两个 job 均走 AVX2 路径，scalar 分支从未被编译和测试
- `README.md` 与 `AGENTS.md` §4.3 scalar 路径描述同步（"本地手动执行，不进 CI" -> "已进 CI"；sanitizer 验证仍为本地手动）
- `benchmark_bitcal.cpp` 计时收敛到 `bitcal::bench::measure_ns`（删除自研 `SimpleTimer` / `run_benchmark`）：基线基准与对比基准统一为同一测量方法（warmup + 25 样本中位数/CV），两组可执行文件数字可直接互比
- CI GCC job 开启 `BITCAL_BUILD_BENCHMARKS=ON`（仅编译覆盖、不运行）：此前 benchmarks 不在任何 CI job 编译，代码可能静默腐烂
- `benchmark_compare` / `benchmark_bitcal` 报告主列从 median 改为 min：负载尖峰只会向上污染测量，min 是稳健下界（实证见 NOTES「亚纳秒基准在高负载机器上不可信」）

### ♻️ 重构

- `shift_left` / `shift_right` 改为单遍融合内核（`detail::shift_left_fused` / `shift_right_fused`，分离读写、单遍完成词移+位移、无串行 carry 依赖链），`compose_shifted_block` 删除预拷贝循环；`shift_left` / `shift_right` 整体 constexpr；`word_ops.hpp` 删除不再使用的 `<cstring>`
- `algorithms.hpp` 二元运算的 AVX2/scalar lambda 对提取为 `detail::and_ops` / `or_ops` / `xor_ops` / `andnot_ops`（`static constexpr` 成员），`*_into` 与 `bit_*<Bits>` 共享同一定义，消除 8 处重复（特别是 `andnot` 的 `_mm256_andnot_si256(b, a)` 参数反转只写一次）
- `algorithms.hpp` 注释精简：删除编译器优化细节（死存储消除）和与 README 重复的 CTAD 说明
- 删除冗余测试 `test_public_contract_core_types_accessible_through_umbrella`，其断言并入 `test_block_view_smoke`（含 `const_bit_view` 运行时路径）
- `benchmark_compare.cpp` 提取 `append_row` helper，消除 9 处重复的 row 构造模式
- `CMakeLists.txt` 删除 MSVC `/arch:AVX2` 死分支与仅服务该分支的 `include(CheckCXXCompilerFlag)`，`BITCAL_SIMD_FLAGS` 简化为 `-march=native`（平台收敛收尾）
- `popcount` 回退逐字 `std::popcount` 循环，删除 AVX2 LUT 路径（详见 ⚡ 性能）；`popcount_words` 不再需要 `if consteval` 分叉（纯 scalar 循环天然 constexpr）
- 修正 `test_block_storage_alignment` 过时注释（"alignment for __m256i load/store" → 存储保证、非正确性前提）
- 删除 `tests/support/random_cases.hpp` 遗留的未使用 `#include <span>`（此前同类遗留仅清理了 examples/benchmarks）
- `compose_binary_block` / `compose_shifted_block` 删除运行时宽度 assert（span 静态 extent 已在编译期保证）；测试与基准迁移至 block / span 形态

### ⚡ 性能

- `popcount` 删除 AVX2 LUT 路径（`popcount_bytes_avx2` + `_mm256_sad_epu8`），回退逐字 `std::popcount`：`benchmark_compare` 实测（AVX2 + POPCNT 机器）LUT 路径在其生效的 256/512 位块上慢于 scalar popcnt（`popcount<256>` 1.51 ns vs 0.58 ns、`popcount<512>` 1.72 ns vs 1.11 ns），4.1.0「AVX2 加速 popcount」声明被项目自身基准证伪（取舍记录见 `NOTES.md`）；`is_zero` / `equals` AVX2 分派保留（实测 ≤512 位与 scalar 持平），措辞不再称“加速”

### 🧪 测试

- 补 shift 的 constexpr 编译期断言（`static_assert(test_shifts_remain_constexpr())`，覆盖 count=64/65/256）+ 128 位词内位移极端（count=1/63）与纯词移（count=64）确定性用例 + 512 位随机对照（此前 shift 确定性用例与随机对照最大只到 256 位）
- 补 `equals` 的 AVX2 路径回归覆盖：256/512 位确定性用例（块内部分相等、跨 vec 块不等）+ 256 位随机对照参考模型。此前 `equals` 的"不等 -> 提前返回"分支仅在 ≤192 位（走 scalar 尾部）或自比（恒真）下被覆盖，AVX2 路径（≥4 字）无断言保护
- 补 `shift_left` / `shift_right` 的 256 位随机对照测试：随机 count ∈ [0, Bits] 对照逐 bit 参考模型，覆盖此前手工用例未碰的中间 shift 值

## [4.1.0] - 2026-07-20

### 💥 破坏性变更

- 工程化定位收敛为"业余练习库"：移除分发库基础设施（install/export/LTO/hardening/cmake config 包），不再保留预编译二进制分发姿态
- 移除 `README.en.md` 双轨镜像（AGENTS.md §5 已声明不保留 Markdown 双轨）
- 后端契约收敛为 `scalar` 与 `avx2`；移除 `sse2` / `avx512` 的保留公开承诺
- 从活动主流程移除 OpenSpec-first 资产（`openspec/`、opsx 命令集）
- 移除 `backend_kind` enum 与 `default_backend()`（不存在运行时选择）；替换为编译期 `active_backend_name` 字符串常量
- `shift_left<Bits>()` / `shift_right<Bits>()` 的 count 参数从 `int` 改为 `size_t`（负数不再静默视为零移位）
- 删除 `vcpkg.json`（声明 google-benchmark 但 benchmark harness 自研 `std::chrono`，`install-features` 为死配置）

### ⚒️ 工程化收敛

- `CMakeLists.txt` 从 ~140 行收敛到 ~55 行：删 `install(EXPORT ...)`、`bitcal-config.cmake.in`、`cmake/` 目录、`BITCAL_ENABLE_LTO` + `check_ipo_supported`、`BITCAL_ENABLE_HARDENING` + 6 个 hardening flag、版本号正则解析、`GNUInstallDirs` / `CMakePackageConfigHelpers`
- `project()` 版本改为独立硬编码（仅 CMake 元数据；代码版本号单一事实源仍是 `include/bitcal/config.hpp`）
- `benchmarks/CMakeLists.txt` 删 git commit 探测、`BITCAL_BUILD_TYPE` 宏、`find_package(benchmark)` 与 `BITCAL_HAS_GBENCH` 宏（vcpkg.json 已删，永远找不到，benchmark_harness 自研 `std::chrono`）
- `benchmarks/benchmark_bitcal.cpp` 删 `#ifdef BITCAL_HAS_GBENCH` 双轨分支（死代码）

### ✨ 新增功能

- 返回型算法（`bit_and` / `bit_or` / `bit_xor` / `bit_andnot` / `shift_left` / `shift_right`）新增 `bit_block<Bits>` 重载，CTAD 推导 `Bits`：`bit_and(a, b)` 替代 `bit_and<256>(a.view(), b.view())`。原地算法与查询算法不受影响（视图转换已自动生效）

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

- 新增 CTAD `bit_block` 重载测试：编译期 `static_assert`（6 个算法返回类型 + 视图形态与 CTAD 形态值等价）+ 运行时值验证（`bit_and` / `bit_or` / `bit_xor` / `bit_andnot` / `shift_left(64)` / `shift_right(64)` 各字结果）
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

- CI 矩阵收敛：删 scalar matrix job（`-mno-avx2` + `BITCAL_NATIVE_ARCH=OFF`）、删 sanitizer-test job（ASan + UBSan）。scalar 路径与 sanitizer 验证改为本地手动执行
- CI 保留最小可信路径：`format-check` + 单 `build-and-test` job（avx2 Release）
- CI 触发路径移除 `cmake/**` 与 `README.en.md` 引用
- CI 矩阵降到最小保留集，并移除 `openspec/**` 触发
- CI 增加 `clang-format` dry-run 检查 job
- CI 触发路径列表移除 `vcpkg.json`，新增 `.clang-format`

### 📚 文档

- README 收敛：删英文 README 链接、删"vNext 活动重设计"措辞（v4.0.0 已发布）、删迁移说明段、补"实验性业余练习库 + 源码集成分发模型 + scalar 本地手动验证 + 无 google-benchmark 依赖"说明
- README 补"宽度约束说明"：明示 `Bits % 64 == 0` 是硬约束，仅服务 64 倍数宽度场景，不覆盖任意位宽需求（Curve25519 / Bloom / 位图索引）
- README quick start 改用 CTAD 形态（`bit_and(a, b)`），补"调用形态"段说明视图形态与拥有型形态的差异
- AGENTS.md §4.3 工程化约束补全：显式声明"不引入 install/export/LTO/hardening/cmake config 包"与"scalar + sanitizer 本地手动验证，不进 CI 矩阵"
- README 后端与 API 描述改为与实现一致
- README 增加 scalar build 命令、契约说明（`equals` 容忍宽度不一致，其他算法 Release 下宽度不一致为 UB）、benchmark_compare 用途说明
- 重写项目治理文档（`AGENTS.md`）为轻流程策略
- 删除低价值白皮书附加资产（`design-evolution` 页面、`docs/diagrams/`）
- 删除 VitePress 文档站点（`docs/` 全部）、`PRODUCT.md`、`CONTRIBUTING.md`、issue/PR 模板、`release.yml` / `docs-pages.yml` CI、`CLAUDE.md` / copilot 指令（并入 `AGENTS.md`）
- README / CHANGELOG 单语化（中文为准），删除 `.zh-CN` 镜像

## [4.0.0] - 2026-05-15

### 🚀 亮点

本版本基于 C++23 完成全新三层架构重设计：

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

| 版本 | 日期 | 亮点 |
|------|------|------|
| v4.1.0 | 2026-07-20 | 工程化收敛 + CTAD 重载 |
| v4.0.0 | 2026-05-15 | C++23 重设计，全新三层架构 |
| v3.0.0 | 2026-05-08 | 公开接口收口、迁移说明 |
| v2.1.0 | 2026-04-16 | ANDNOT，性能提升 |
| v2.0.0 | 2026-01-08 | 完全重写，纯头文件 |
| v1.x | 2025 | 基于继承的设计（已废弃） |
