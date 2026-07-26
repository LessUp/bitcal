# 设计取舍与已知限制

> 跨版本稳定的设计决策与已知不修的限制。
> 已修复 bug 与版本变更见 `CHANGELOG.md`，实施过程见 git log。
> 新发现的设计取舍或已知限制追加至此；已修复的问题不在此记录。

## 设计取舍

### 视图不携带编译期宽度

`bit_view` / `const_bit_view` 仅存 `(ptr, word_count)`，运行时宽度。返回型算法需手写 `<Bits>`；CTAD 重载仅缓解 `bit_block<Bits>` 场景，外部存储视图仍需显式宽度。

不模板化的原因：会使 `*_into` 原地算法失去"不关心宽度"的简洁性，且波及全部公开 API + 测试 + 示例。宽度错配 Release 下 UB、Debug 下 `assert`（README 契约说明已覆盖）。

### `bit_block` 32 字节对齐与 AVX2 unaligned load

`bit_block<Bits>` 在 `Bits >= 256 && BITCAL_HAS_AVX2` 时 `alignas(32)`，但 AVX2 内核用 `_mm256_loadu_si256` / `_mm256_storeu_si256`（不要求对齐）。对齐是 `bit_block` 自身存储的保证；unaligned load 为兼容外部未对齐 `bit_view` 存储。两者不矛盾：算法必须对任意对齐的视图工作。

## 已知限制（不修）

### Shift 无 AVX2 路径且非 constexpr

`shift_*_array` 纯 scalar，`compose_shifted_block` 用 `memset` 非 constexpr（`popcount` / `is_zero` / `equals` 有 `if consteval` constexpr 路径）。

不做 AVX2 的原因：跨 lane 移位需 `_mm256_permutevar8x32_epi32` + 跨 128-bit lane 进位处理，复杂度高；≤512 位块 scalar shift 绝对耗时足够低，`benchmark_compare` 已有计时基线可复评。非 constexpr：README 未声称，改 constexpr 无明确诉求。
