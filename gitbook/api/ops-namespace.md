# 函数式接口 (ops)

`bitcal::ops` 命名空间提供对原始 `uint64_t` 数组的函数式接口，适合不想构造 `bitarray` 对象时直接操作。

## popcount

```cpp
template<size_t Bits>
uint64_t bitcal::ops::popcount(const uint64_t* data);
```

统计 `data` 中 1 的个数。`Bits` 指定总位宽。

## count_leading_zeros

```cpp
template<size_t Bits>
int bitcal::ops::count_leading_zeros(const uint64_t* data);
```

## count_trailing_zeros

```cpp
template<size_t Bits>
int bitcal::ops::count_trailing_zeros(const uint64_t* data);
```

## reverse_bits_64

```cpp
uint64_t bitcal::ops::reverse_bits_64(uint64_t x);
```

反转单个 64-bit 整数的所有位。

## byte_swap_64

```cpp
uint64_t bitcal::ops::byte_swap_64(uint64_t x);
```

交换字节序（大端 ↔ 小端）。

## 示例

```cpp
uint64_t data[4] = {0xFF, 0, 0, 0};

uint64_t ones = bitcal::ops::popcount<256>(data);
int clz = bitcal::ops::count_leading_zeros<256>(data);
int ctz = bitcal::ops::count_trailing_zeros<256>(data);

uint64_t reversed = bitcal::ops::reverse_bits_64(0x0F);
uint64_t swapped = bitcal::ops::byte_swap_64(0x0102030405060708ULL);
```
