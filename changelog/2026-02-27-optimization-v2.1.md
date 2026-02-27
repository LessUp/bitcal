# BitCal v2.1 优化 - 2026-02-27

## 概述

本次更新包含关键正确性修复、性能优化和代码质量改进。

---

## P0: 正确性修复

### 1. MSVC SSE2 宏检测修复 (`config.hpp`)
- **问题**: MSVC 在 x64 模式下不定义 `__SSE2__`，导致 SSE2 路径永远不会被启用
- **修复**: 添加 `_M_X64 || _M_AMD64` 判断；为 MSVC 添加 `#include <intrin.h>`

### 2. NEON `vmvnq_u64` 不存在 (`neon_ops.hpp`)
- **问题**: 标准 NEON 无 `vmvnq_u64` intrinsic，ARM 编译会失败
- **修复**: 改用 `veorq_u64(a, vdupq_n_u64(~0ULL))`

### 3. NEON 编译期常量要求 (`neon_ops.hpp`)
- **问题**: `vshlq_n_u64` / `vshrq_n_u64` 要求编译期常量，传入运行时变量会编译失败
- **修复**: 改用 `vshlq_u64` + `vdupq_n_s64(count)` 支持运行时移位量

### 4. SSE 256-bit shift carry 传播 bug (`sse_ops.hpp`)
- **问题**: `shift_left_256` / `shift_right_256` 中 4 个 qword 之间的进位链断裂：
  - `lo[1]` 未收到 `lo[0]` 的 carry
  - `hi[0]` 未收到 `lo[1]` 的 carry（cross-boundary）
  - `hi[1]` 收到了错误的 carry
- **修复**: 重写为"先标量 word-shift，再 SSE bit-shift + 标量 cross-carry"策略

### 5. AVX 256/512-bit shift cross-lane bug (`avx_ops.hpp`)
- **问题**: `_mm256_slli_si256` / `_mm256_srli_si256` / `_mm256_alignr_epi8` 按 128-bit lane 独立操作，无法跨 lane 移位，导致 word-shift 结果完全错误
- **修复**: 重写为"先标量 word-shift，再 AVX2 `permute4x64` bit-shift"策略
- **影响范围**: `shift_left_256`, `shift_right_256`, `shift_left_512`, `shift_right_512`

---

## P1: 性能优化

### 1. aligned load/store (`sse_ops.hpp`, `avx_ops.hpp`)
- 新增 `load_aligned()` / `store_aligned()` 函数
- 内部数据已 `alignas(64)` 对齐，可利用对齐加载指令减少微架构开销

### 2. SIMD 加速 `operator~` (`bitcal.hpp`)
- 原先纯标量循环逐 word 取反
- 现在对 128/256/512-bit 分派到 SSE/AVX/NEON NOT 指令

### 3. SIMD 加速 `is_zero()` (`bitcal.hpp`)
- 原先逐 word 循环 + early exit
- 现在对 128/256/512-bit 使用 `_mm_testz` / `_mm256_testz` 单指令检测

### 4. `clear()` 优化 (`bitcal.hpp`)
- 从逐元素赋零改为 `std::memset`，对编译器更友好

### 5. `reverse()` 原地化 (`bitcal.hpp`)
- 消除临时数组，改为首尾交换 + `reverse_bits`

### 6. NOT/is_zero 底层实现
- `sse_ops.hpp`: 新增 `bit_not_128`, `bit_not_256`, `is_zero_128`, `is_zero_256`
- `avx_ops.hpp`: 新增 `bit_not_256`, `bit_not_512`, `is_zero_256`, `is_zero_512`
- `neon_ops.hpp`: 新增 `bit_not_128`, `bit_not_256`, `bit_not_512`

---

## P2: 代码质量

### 1. 消除 bitcal.hpp 中的重复代码
- 原先 `bit_and` / `bit_or` / `bit_xor` 三个私有方法结构完全一致（各 ~35 行），仅操作不同
- 合并为统一的 `dispatch_binop<binop Op>` 模板，减少约 70 行重复代码
- 使用 `enum class binop { op_and, op_or, op_xor, op_andnot }` 作为编译期标签

### 2. 新增 ANDNOT 操作
- **公共 API**: `bitarray::andnot(mask)` → 返回 `*this & ~mask`
- **SIMD 实现**:
  - SSE: `_mm_andnot_si128` (128/256-bit)
  - AVX: `_mm256_andnot_si256` (256/512-bit)
  - NEON: `vbicq_u64` (128/256/512-bit)
  - Scalar: `a & ~b` (fallback)
- **性能**: 单指令完成 NOT+AND，比分开做快约 2x

---

## 修改的文件

| 文件 | 变更类型 |
|------|----------|
| `include/bitcal/config.hpp` | MSVC 兼容性修复 |
| `include/bitcal/scalar_ops.hpp` | 新增 `bit_andnot` / `bit_andnot_array` |
| `include/bitcal/sse_ops.hpp` | shift bug 修复 + aligned load/store + NOT/is_zero/ANDNOT |
| `include/bitcal/avx_ops.hpp` | shift bug 修复 + aligned load/store + NOT/is_zero/ANDNOT |
| `include/bitcal/neon_ops.hpp` | 编译错误修复 + NOT/ANDNOT |
| `include/bitcal/bitcal.hpp` | SIMD 分派优化 + 代码去重 + ANDNOT API |
