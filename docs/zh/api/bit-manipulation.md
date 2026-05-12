# 位操作

## get_bit — 读取单个位

```cpp
bool get_bit(size_t bit_index) const;
```

读取位置 `bit_index` 处的位值（从最低有效位开始，0 索引）。

**参数**：
- `bit_index`：要读取的位位置（0 到 Bits-1）

**返回**：如果位被设置则为 `true`，否则为 `false`

**复杂度**：O(1)

## set_bit — 设置单个位

```cpp
void set_bit(size_t bit_index, bool value = true);
```

设置位置 `bit_index` 处的位。`value=true` 设为 1，`value=false` 清为 0。

**参数**：
- `bit_index`：要修改的位位置
- `value`：新值（默认：true）

**复杂度**：O(1)

## flip_bit — 翻转单个位

```cpp
void flip_bit(size_t bit_index);
```

翻转位置 `bit_index` 处的位（0→1，1→0）。

**参数**：
- `bit_index`：要翻转的位位置

**复杂度**：O(1)

## reverse — 位反转

```cpp
void reverse();
```

反转所有位：位 0 与位 (Bits-1) 交换，位 1 与位 (Bits-2) 交换，依此类推。

**实现**：使用从两端交换的原地操作（v2.1+）。之前版本使用临时数组。

**复杂度**：O(Bits/64)

## 示例

### 单个位操作

```cpp
bitcal::bit256 arr;

arr.set_bit(0);      // 将位 0 设为 1
arr.set_bit(255);    // 将位 255 设为 1
arr.set_bit(100, false);  // 清除位 100

bool b0 = arr.get_bit(0);    // true
bool b100 = arr.get_bit(100); // false

arr.flip_bit(0);     // 翻转位 0 → 现在 0
arr.flip_bit(42);    // 翻转位 42 → 现在 1
```

### 位反转

```cpp
bitcal::bit64 x(1);   // 位 0 = 1
x.reverse();           // 现在位 63 = 1
// x[0] == 0x8000000000000000

bitcal::bit256 y;
y[0] = 0x0000000000000001;  // 位 0 = 1
y[3] = 0x8000000000000000;  // 位 255 = 1
y.reverse();
// 反转后：位 0 = 1（原位 255），位 255 = 1（原位 0）
```

## 实现细节

### 访问模式

```cpp
// 内部实现
bool get_bit(size_t bit_index) const {
    size_t word = bit_index / 64;
    size_t bit = bit_index % 64;
    return (data_[word] >> bit) & 1;
}

void set_bit(size_t bit_index, bool value) {
    size_t word = bit_index / 64;
    size_t bit = bit_index % 64;
    if (value) {
        data_[word] |= (1ULL << bit);
    } else {
        data_[word] &= ~(1ULL << bit);
    }
}

void flip_bit(size_t bit_index) {
    size_t word = bit_index / 64;
    size_t bit = bit_index % 64;
    data_[word] ^= (1ULL << bit);
}
```

### 反转算法

```cpp
// 简化伪代码
void reverse() {
    // 步骤 1：反转每个 64 位字内的位
    for (size_t i = 0; i < u64_count; ++i) {
        data_[i] = reverse_bits_64(data_[i]);
    }

    // 步骤 2：从两端交换字
    for (size_t i = 0, j = u64_count - 1; i < j; ++i, --j) {
        std::swap(data_[i], data_[j]);
    }
}
```

## 性能说明

| 操作 | 复杂度 | 备注 |
|------|--------|------|
| get_bit | O(1) | 单次内存访问 |
| set_bit | O(1) | 读-改-写 |
| flip_bit | O(1) | XOR 操作 |
| reverse | O(Bits/64) | 两遍数据扫描 |

## 边界检查

**注意**：为性能考虑，这些函数不执行边界检查。传递 `bit_index` >= `Bits` 会导致未定义行为。

对于带边界检查的调试构建，建议：
```cpp
assert(bit_index < Bits);
arr.set_bit(bit_index, true);
```

## 用例

### 位标志

```cpp
bitcal::bit1024 feature_flags;

feature_flags.set_bit(FEATURE_A, enabled_a);
feature_flags.set_bit(FEATURE_B, enabled_b);

if (feature_flags.get_bit(FEATURE_A)) {
    // 功能 A 已启用
}
```

### 位矩阵转置

```cpp
// 使用反转和移位转置 8×8 位矩阵
bitcal::bit64 matrix;
// ... 加载矩阵位 ...
matrix.reverse();  // 转置操作的一部分
```
