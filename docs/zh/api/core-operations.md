# 核心操作

本页只记录 `<bitcal/bitcal.hpp>` **今天实际导出的命名空间级算法**。

## 公开术语表

| 术语 | 含义 |
| --- | --- |
| 拥有型 block | 由结果型算法返回的 `bit_block<Bits>` |
| 非拥有视图 | 传入算法的 `bit_view` / `const_bit_view` |
| 自由算法 | 例如 `bit_and<Bits>()` 这样的命名空间级函数 |
| 当前已发货表面 | 今天的公开 umbrella header 实际导出的符号 |

## 当前已发货的按位算法

```cpp
template <std::size_t Bits>
bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept;

void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept;
```

### 适用场景

- 需要新的 owning result block 时使用 `bit_and<Bits>()`
- 已经有可写存储可复用时使用 `and_into()`
- 两种形式都要求输入与输出描述同一个保留宽度

### 契约说明

- `Bits` 必须匹配你期望返回的结果 block 宽度
- 调用方负责保证传入 view 的 `word_count()` 与 `bit_block<Bits>::word_count` 一致
- `and_into()` 还要求 `out.word_count()` 与两个输入一致

## 当前已发货的查询算法

```cpp
bool is_zero(const const_bit_view value) noexcept;
std::uint64_t popcount(const const_bit_view value) noexcept;
```

### 它们回答什么问题

| 需求 | 函数 |
| --- | --- |
| 所有 bit 是否都为零？ | `is_zero()` |
| 一共有多少个置位 bit？ | `popcount()` |

## 关于更宽算法族的状态说明

一些 redesign 文档仍会提到 `bit_or`、`bit_xor`、`bit_andnot`、`equals`、`shift_left`、`shift_right` 这些目标形态。但它们今天**并没有**作为命名空间级自由算法导出，所以本参考页不会把它们写成当前可直接使用的算法表面。

## 组合示例

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;

auto lhs_words = lhs.view();
lhs_words.data()[0] = 0x0FULL;
lhs_words.data()[2] = 0x00F0ULL;

auto rhs_words = rhs.view();
rhs_words.data()[0] = 0x0005ULL;
rhs_words.data()[1] = 0x0001ULL;
rhs_words.data()[2] = 0x00CCULL;

const auto& lhs_const = lhs;
const auto& rhs_const = rhs;

auto produced = bitcal::bit_and<256>(lhs_const.view(), rhs_const.view());
bitcal::bit_block<256> scratch;
bitcal::and_into(lhs_const.view(), rhs_const.view(), scratch.view());

const auto& produced_const = produced;
const auto& scratch_const = scratch;

auto empty = bitcal::is_zero(produced_const.view());
auto ones = bitcal::popcount(scratch_const.view());
```

## 如何选择合适的算法形态

| 需求 | 优先选择 |
| --- | --- |
| 返回一个新的 AND 结果 | `bit_and<Bits>()` |
| 复用可写存储写入 AND 结果 | `and_into()` |
| 不修改数据地做查询 | `is_zero()`、`popcount()` |

## 关于断言与安全性

当前已发货表面依赖调用方自行维持位宽纪律。请把字数匹配视为 API 契约的一部分，而不是可有可无的运行时便利功能。

## 后端边界提醒

这些函数定义的是公开行为。真正执行它们的 SIMD 内核仍然是实现细节，相关说明见 [SIMD 分发](../architecture/simd-dispatch.md)。

## 下一步

- [类型参考](./types.md)
- [SIMD 分发](../architecture/simd-dispatch.md)
- [性能基线](../architecture/performance-baseline.md)
