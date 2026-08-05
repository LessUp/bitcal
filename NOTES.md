# 设计取舍与已知限制

> 跨版本稳定的设计决策与已知不修的限制。
> 已修复 bug 与版本变更见 `CHANGELOG.md`，实施过程见 git log。
> 新发现的设计取舍或已知限制追加至此；已修复的问题不在此记录。

## 设计取舍

### 视图携带运行时宽度；返回型算法取静态宽度 range

`bit_view` / `const_bit_view` 仅存 `(ptr, word_count)`，运行时宽度，服务 `*_into` 原地算法与查询算法——它们需要宽度无关的灵活性（`equals` 容忍宽度错配；外部存储的查询天然是运行时宽度）。

返回型算法（`bit_*` / `shift_*`）取静态宽度 word range（`std::span<const uint64_t, N>` / `std::array` / C 数组，`static_word_range` concept 约束）：宽度从 extent 推导，无需手写模板参数，宽度错配是编译错误。4.1.0 及更早的视图形态（手写 `<Bits>`、Release 下宽度错配 UB）已移除。

不模板化视图本身的原因不变：会使 `*_into` 失去"不关心宽度"的简洁性。静态宽度形态在不触动视图的前提下拿回编译期宽度安全，两类 API 各司其职。

### `bit_block` 32 字节对齐与 AVX2 unaligned load

`bit_block<Bits>` 在 `Bits >= 256 && BITCAL_HAS_AVX2` 时 `alignas(32)`，但 AVX2 内核用 `_mm256_loadu_si256` / `_mm256_storeu_si256`（不要求对齐）。对齐是 `bit_block` 自身存储的保证；unaligned load 为兼容外部未对齐 `bit_view` 存储。两者不矛盾：算法必须对任意对齐的视图工作。

### popcount 走 scalar 路径（不走 AVX2 分派）

`popcount` 为逐字 `std::popcount` 循环；`is_zero` / `equals` 保留 AVX2 分派。

4.1.0 曾为 popcount 引入 AVX2 LUT 方案（拆 nibble + `_mm256_shuffle_epi8` 查表 + `_mm256_sad_epu8` 归约），`benchmark_compare` 实测将其证伪（AVX2 + POPCNT 机器，中位数）：

| 操作 | BitCal（AVX2 LUT） | std::bitset | 比值 |
|------|--------------------|-------------|------|
| `popcount<256>` | 1.51 ns | 0.58 ns | 0.39x |
| `popcount<512>` | 1.72 ns | 1.11 ns | 0.64x |

原因：LUT 方案每 32 字节约 9~10 条指令；`popcnt` 指令每 8 字节 1 条、吞吐量 1/cycle。256/512 位（LUT 路径生效的 4~8 字块）规模下 scalar popcnt 更快，LUT 只在无 POPCNT 的机器处理宽缓冲时才有意义——本项目支持的平台上 POPCNT 已基本普及，`std::popcount` 直接映射 `popcnt` 指令。

顺带实测：`is_zero` / `equals` 的 AVX2 分派在 ≤512 位上与 scalar 持平（约 1.0x，`is_zero<512>` 0.67x 略慢），不构成加速；保留是为 >512 位宽视图留余地，文档不再称“加速”。

回退后复核曾观察到 128/192 位约 0.5x 于 `std::bitset` 的“残留差距”，进一步隔离实验（绑核 + min-of-N 微基准，对照 scalar 循环 / 4 路拆分累加器 / `std::bitset`）证明该差距是测量伪影而非代码差距：干净测量下三者全部宽度持平（0.7~0.9ns）。曾尝试手工 4 路拆分累加器以隐藏 `popcnt` 延迟，干净测量与负载下均无收益，按“小巧”原则回退。

### 亚纳秒基准在高负载机器上不可信

`benchmark_compare` 用“每样本多次迭代取中位数”，且每对场景先测 BitCal 后测 `std::bitset`。在负载波动的共享机器上，亚纳秒级操作（如 `popcount<128>`）的比值会系统性失真：负载尖峰优先击中先测一方，中位数无法剔除持续数百毫秒的尖峰。实证：`is_zero<512>` 代码未变，相邻两轮比值在 0.50x 与 1.16x 之间摆动。

解读/复测亚纳秒行时：绑定 CPU（`taskset -c`）并用 min-of-N（而非 median-of-samples）；或只关注 ≥10ns 量级的行。本轮 popcount LUT 证伪结论不受影响（差距为 2~3 倍且机制上 LUT 指令数更多，不依赖亚纳秒精度）。

## 已知限制（不修）

### Shift 无 AVX2 路径；变换类算法整体非 constexpr

`shift_*_array` 纯 scalar，`compose_shifted_block` 用 `memset` 非 constexpr。变换类算法（`bit_*` / `shift_*`）均非 constexpr（运行时 AVX2 分派）；查询类（`popcount` / `is_zero` / `equals`）为 constexpr（`is_zero` / `equals` 走 `if consteval` scalar 分支，popcount 纯 scalar 循环天然 constexpr）。

不做 AVX2 的原因：跨 lane 移位需 `_mm256_permutevar8x32_epi32` + 跨 128-bit lane 进位处理，复杂度高；≤512 位块 scalar shift 绝对耗时足够低，`benchmark_compare` 已有计时基线可复评。非 constexpr：README 未声称，改 constexpr 无明确诉求。
