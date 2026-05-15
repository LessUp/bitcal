# 核心操作

本页记录 vNext 当前保留的核心算法表面。

## 公开术语表

| 术语 | 含义 |
| --- | --- |
| 拥有型 block | 算法返回或写入的 `bit_block<Bits>` |
| 非拥有视图 | 传入算法的 `bit_view` / `const_bit_view` |
| 自由算法 | 例如 `bit_and<Bits>()` 这样的命名空间级函数 |
| 后端边界 | 这些算法之后的内部执行层 |

## `and_into()`

```cpp
void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept;
```

### 适用场景

- 你已经有一个输出 block 或外部缓冲区
- 你希望避免再创建一个新的拥有型结果

### 行为

- 要求 `lhs`、`rhs`、`out` 的字数一致
- 把按位与结果写入 `out`
- 在内部跨过后端边界进入当前执行路径

## `bit_and<Bits>()`

```cpp
template <std::size_t Bits>
[[nodiscard]] bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept;
```

### 适用场景

- 你需要一个新的 owning result block
- 结果位宽在编译期已知

### 行为

- 要求两个输入视图都匹配 `bit_block<Bits>::word_count`
- 内部通过新建 owning block 并复用 `and_into()` 完成计算

## `is_zero()`

```cpp
[[nodiscard]] constexpr bool is_zero(const const_bit_view value) noexcept;
```

当视图中的每个字都为零时返回 `true`。

## `popcount()`

```cpp
[[nodiscard]] constexpr std::uint64_t popcount(const const_bit_view value) noexcept;
```

返回整个视图中置位比特的总数。

## 组合示例

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
bitcal::bit_block<256> scratch;

auto produced = bitcal::bit_and<256>(lhs.view(), rhs.view());
bitcal::and_into(lhs.view(), rhs.view(), scratch.view());

auto empty = bitcal::is_zero(produced.view());
auto ones = bitcal::popcount(scratch.view());
```

## 如何选择合适的算法形态

| 需求 | 优先选择 |
| --- | --- |
| 返回一个新的拥有型结果 | `bit_and<Bits>()` |
| 复用现有可写存储 | `and_into()` |
| 只读零值检查 | `is_zero()` |
| 只读计数查询 | `popcount()` |

## 关于断言与安全性

写路径算法依赖调试断言来保证字数匹配。因此调用方应把位宽匹配视为契约的一部分，而不是可有可无的运行时便利功能。

## 后端边界提醒

这些算法定义的是公开行为。实际执行它们的 SIMD 内核仍然是实现细节，相关说明见 [SIMD 分发](../architecture/simd-dispatch.md)。

## 下一步

- [类型参考](./types.md)
- [SIMD 分发](../architecture/simd-dispatch.md)
- [性能基线](../architecture/performance-baseline.md)
