# 2026-06-22 Clean Code 优化：dispatch 三层 DRY

## 目标

按《代码整洁之道》DRY 原则消除 `*_into_scalar` / `*_into_x64` / `*_words` 三层共 12 个同构函数的重复，统一为模板化 dispatch。

## 根因

三层各 4 个函数（and/or/xor/andnot）结构同构，仅运算符差异：

- `scalar_impl.hpp`：4 个 `*_into_scalar`，各 4 行循环，仅 `& | ^ & ~` 差异
- `x64_dispatch.hpp`：4 个 `*_into_x64`，各 ~10 行 `#if AVX2 → binary_into_avx2_or_scalar(...) #else → *_into_scalar(...)`，仅 lambda 体差异
- `word_ops.hpp`：4 个 `*_words`，各 3 行 `assert + *_into_x64`，仅函数名差异

共 ~97 行重复代码。任何 dispatch 流程修改需在 12 处同步。

## 代码改动

### `include/bitcal/detail/scalar_impl.hpp`

- 删除 `and_into_scalar` / `or_into_scalar` / `xor_into_scalar` / `andnot_into_scalar`
- 新增 `binary_into_scalar<WordOp>(lhs, rhs, out, wc, word_op)` 模板

### `include/bitcal/detail/x64_dispatch.hpp`

- 保留 `binary_into_avx2_or_scalar`（AVX2 循环骨架）
- 删除 `and_into_x64` / `or_into_x64` / `xor_into_x64` / `andnot_into_x64`
- 新增 `binary_into_x64<VectorOp, WordOp>(lhs, rhs, out, wc, vector_op, word_op)` 模板
  - AVX2 平台：路由到 `binary_into_avx2_or_scalar`
  - 非 AVX2 平台：`(void)vector_op;` 后走 `binary_into_scalar`
  - 注释说明 `vector_op` 用泛型 lambda（auto 参数），非 AVX2 平台不实例化体，AVX2 intrinsic 引用安全

### `include/bitcal/detail/word_ops.hpp`

- 删除 `and_words` / `or_words` / `xor_words` / `andnot_words`
- 新增 `binary_words<VectorOp, WordOp>(lhs, rhs, out, vector_op, word_op)` 模板

### `include/bitcal/algorithms.hpp`

- `compose_binary_block` 参数 `BinaryOp` → `VectorOp`，大位宽路径改为调用 `binary_words(lhs, rhs, out, vector_op, word_op)`
- `and_into` / `or_into` / `xor_into` / `andnot_into` 改为调用 `binary_words` + 泛型 vop lambda + wop lambda
- `bit_and` / `bit_or` / `bit_xor` / `bit_andnot` 传入泛型 vop lambda（auto 参数，引用 AVX2 intrinsic）+ wop lambda
- `andnot` 路径保留 AVX2 参数顺序说明：`_mm256_andnot_si256(b, a)` 计算 `a & ~b`
- 注释更新：`vector_op` 替代 `binary_op`，说明泛型 lambda 延迟实例化机制

## 收益

- 三层 12 函数 → 3 模板，消除 ~78 行重复
- 单一修改点：dispatch 流程只在 `binary_into_x64` 一处
- 调用方（algorithms.hpp）显式传 vop + wop，运算符语义在调用点可见，不再隐藏于具名函数后
- 非 AVX2 平台可移植性增强：vop 体延迟实例化，无需 `#if` 包裹调用方

## 验证

- `ctest` 全部通过
- `benchmark_compare` 确认无性能回归：
  - 256 位 AVX2 路径：`bit_and<256>` ~1ns，`bit_or<256>` ~0.76ns（持平）
  - 512 位 AVX2 路径：`bit_and<512>` ~1.5ns（持平）
  - 128/192 位小位宽快速路径：`bit_and<128>` ~0.7ns，`bit_and<192>` ~1.6ns（持平）
  - shift 路径无变化

## 后续

Clean Code 评估识别的其余优化项（view 统一 #7、copy_words_helper #8、测试清理 #9/#11）待后续批次推进。
