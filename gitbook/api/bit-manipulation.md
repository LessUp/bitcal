# 位操作

## get_bit — 读取单个位

```cpp
bool get_bit(size_t bit_index) const;
```

读取第 `bit_index` 位的值（0-indexed，从最低位开始）。

## set_bit — 设置单个位

```cpp
void set_bit(size_t bit_index, bool value = true);
```

设置第 `bit_index` 位。`value=true` 置 1，`value=false` 清零。

## flip_bit — 翻转单个位

```cpp
void flip_bit(size_t bit_index);
```

翻转第 `bit_index` 位（0→1，1→0）。

## reverse — 位反转

```cpp
void reverse();
```

将所有位反转：bit 0 ↔ bit (Bits-1)。

v2.1 起改为原地操作（首尾交换），不再使用临时数组。

## 示例

```cpp
bitcal::bit256 arr;

arr.set_bit(0);      // 设置 bit 0
arr.set_bit(255);    // 设置 bit 255
arr.flip_bit(0);     // 翻转 bit 0 → 现在为 0

if (arr.get_bit(255)) {
    std::cout << "bit 255 is set" << std::endl;
}

// 位反转
bitcal::bit64 x(1);   // bit 0 = 1
x.reverse();           // 现在 bit 63 = 1
// x[0] == 0x8000000000000000
```
