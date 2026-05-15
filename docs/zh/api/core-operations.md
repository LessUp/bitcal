# 核心操作

本页把 **持久 vNext API 契约** 与 **当前头文件里恰好存在的辅助入口** 明确区分开来。

## 公开术语表

| 术语 | 含义 |
| --- | --- |
| 拥有型 block | 由结果型算法返回的 `bit_block<Bits>` |
| 非拥有视图 | 传入算法的 `bit_view` / `const_bit_view` |
| 自由算法 | 例如 `bit_and<Bits>()` 这样的命名空间级函数 |
| 持久契约 | `openspec/specs/api/bitcal-public-api.md` 中承诺的 API |

## 持久契约中的按位结果算法

```cpp
template <std::size_t Bits>
bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_xor(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_andnot(const const_bit_view lhs, const const_bit_view rhs) noexcept;
```

### 适用场景

- 你需要一个新的 owning result block
- 结果位宽在编译期已知
- 两个输入 view 都描述同一个保留宽度

### 契约说明

- `Bits` 必须匹配你期望返回的结果 block 宽度
- 调用方负责保证传入 view 的 `word_count()` 与 `bit_block<Bits>::word_count` 一致
- 持久契约把这些自由算法，而不是成员越来越多的值类型，视为架构中心

## 持久契约中的比较与查询算法

```cpp
bool equals(const const_bit_view lhs, const const_bit_view rhs) noexcept;
bool is_zero(const const_bit_view value) noexcept;
std::uint64_t popcount(const const_bit_view value) noexcept;
```

### 它们回答什么问题

| 需求 | 函数 |
| --- | --- |
| 两个 view 是否按位完全相同？ | `equals()` |
| 所有 bit 是否都为零？ | `is_zero()` |
| 一共有多少个置位 bit？ | `popcount()` |

## 持久契约中的移位算法

```cpp
template <std::size_t Bits>
bit_block<Bits> shift_left(const const_bit_view value, int count) noexcept;

template <std::size_t Bits>
bit_block<Bits> shift_right(const const_bit_view value, int count) noexcept;
```

### 边界行为

| 条件 | 行为 |
| --- | --- |
| `count == 0` | 返回不变结果 |
| `count >= Bits` | 返回全零结果 |
| `count < 0` | 未定义行为 |

## 当前头文件中的辅助入口：`and_into()`

```cpp
void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept;
```

当你已经有可写存储并且希望就地写入 AND 结果时，`and_into()` 很有用。它出现在当前头文件里，但**不**属于 `openspec/specs/api/bitcal-public-api.md` 中定义的持久 API 契约。

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

auto produced_and = bitcal::bit_and<256>(lhs_const.view(), rhs_const.view());
auto produced_or = bitcal::bit_or<256>(lhs_const.view(), rhs_const.view());
auto produced_xor = bitcal::bit_xor<256>(lhs_const.view(), rhs_const.view());
auto produced_andnot = bitcal::bit_andnot<256>(lhs_const.view(), rhs_const.view());
const auto& produced_and_const = produced_and;
const auto& produced_or_const = produced_or;
const auto& produced_xor_const = produced_xor;
const auto& produced_andnot_const = produced_andnot;

auto shifted = bitcal::shift_left<256>(produced_or_const.view(), 4);
const auto& shifted_const = shifted;

auto same = bitcal::equals(produced_and_const.view(), produced_xor_const.view());
auto empty = bitcal::is_zero(produced_andnot_const.view());
auto ones = bitcal::popcount(shifted_const.view());

bitcal::bit_block<256> scratch;
bitcal::and_into(lhs_const.view(), rhs_const.view(), scratch.view());
```

## 如何选择合适的算法形态

| 需求 | 优先选择 |
| --- | --- |
| 返回一个新的按位结果 | `bit_and<Bits>()`、`bit_or<Bits>()`、`bit_xor<Bits>()`、`bit_andnot<Bits>()` |
| 比较或查询现有数据 | `equals()`、`is_zero()`、`popcount()` |
| 返回一个移位后的 owning result | `shift_left<Bits>()`、`shift_right<Bits>()` |
| 在当前头文件里复用可写存储 | `and_into()` helper |

## 关于断言与安全性

当前保留契约依赖调用方自行维持位宽纪律。请把字数匹配与非负移位计数视为 API 契约的一部分，而不是可有可无的运行时便利功能。

## 后端边界提醒

这些函数定义的是公开行为。真正执行它们的 SIMD 内核仍然是实现细节，相关说明见 [SIMD 分发](../architecture/simd-dispatch.md)。

## 下一步

- [类型参考](./types.md)
- [SIMD 分发](../architecture/simd-dispatch.md)
- [性能基线](../architecture/performance-baseline.md)
