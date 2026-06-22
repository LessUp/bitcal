# 2026-06-22 小位宽性能回归修复

## 目标

- 消除 128/192 位操作相比 `std::bitset` 的 5-20 倍性能差距
- 修复 `get_optimal_alignment()` 中 AVX-512 残留声明

## 根因

128/192 位操作慢 5-20 倍的三个原因：

1. **零初始化无法消除** — `bit_block` 的 `words_{}` 零初始化被算法完全覆写，但由于覆写通过函数指针发生，编译器无法证明零初始化是死代码
2. **AVX2 dispatch 空转** — 128 位（2 words）进入 AVX2 dispatch 后 `i+4 <= 2` 不成立，AVX2 循环完全不执行，直接跳到标量尾循环
3. **函数指针阻碍内联** — `compose_binary_block` 通过函数指针调用 `and_words`，编译器可能无法完全内联

192 位额外问题：
- `get_optimal_alignment<192>()` 返回 32 字节对齐，导致编译器用 32 字节 AVX2 存储做零初始化，而 3-word（24 字节）写入无法覆盖 32 字节存储，零初始化无法被消除

## 代码改动

### `include/bitcal/algorithms.hpp`

- `compose_binary_block` 添加 `if constexpr (wc < 4)` 小位宽快速路径
  - 小位宽（< 4 words = < 256 bits）：直接逐字计算，绕过 SIMD dispatch
  - 大位宽（>= 4 words）：保持现有 AVX2 dispatch 路径
  - 新增 `WordOp` 模板参数接收 word 级 lambda
- `bit_and` / `bit_or` / `bit_xor` / `bit_andnot` 传入 word 级 lambda
- `shift_left` / `shift_right` 直接内联 copy + shift，消除 `compose_shifted_block` 和 `shift_left_words` / `shift_right_words` 的函数指针调用

### `include/bitcal/detail/word_ops.hpp`

- 删除死代码：`copy_words`、`assert_fixed_word_layout`、`shift_left_words`、`shift_right_words`
  - 这些函数仅被已删除的 `compose_shifted_block` 调用

### `include/bitcal/config.hpp`

- `get_optimal_alignment()` 重构
  - 256 位+：32 字节对齐（AVX2 路径）
  - < 256 位：自然对齐 `alignof(std::uint64_t)` = 8 字节
  - 删除 AVX-512 残留注释和 64 字节对齐

## 性能改善

| 宽度 | 操作 | 修复前 | 修复后 | 提升 |
|------|------|--------|--------|------|
| 128 位 | bit_and | ~10.6ns | ~1.0ns | ~10x |
| 128 位 | shift_left | ~11.1ns | ~1.2ns | ~9x |
| 192 位 | bit_and | ~11.0ns | ~2.0ns | ~5.5x |
| 192 位 | shift_right | ~10.0ns | ~2.6ns | ~3.9x |
| 256 位 | 所有 | ~1.0ns | ~1.0ns | 无变化（已持平） |
| 512 位 | 所有 | ~2.0ns | ~2.0ns | 无变化（已持平） |

128/192 位操作现在与 `std::bitset` 基本持平或接近。

> **硬件变更说明**：上表"修复前"数据来自旧 retained 基线（Intel i7-12700，commit `007c740`），"修复后"数据来自新 retained 基线（AMD Ryzen 7 5800H，commit `27e0742`）。两次测量跨硬件，提升倍数含硬件差异；纯软件收益应在同机对照下另行量化。

## 验证

- `ctest` 全部通过
- `benchmark_compare` 确认 128/192 位大幅改善
- benchmark 结果 JSON 已更新
