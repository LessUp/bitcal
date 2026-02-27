# 核心操作

## 逻辑运算

所有逻辑运算都会根据编译期选择的 SIMD 后端自动分派到最优实现。

### AND

```cpp
bitarray operator&(const bitarray& other) const;
bitarray& operator&=(const bitarray& other);
```

### OR

```cpp
bitarray operator|(const bitarray& other) const;
bitarray& operator|=(const bitarray& other);
```

### XOR

```cpp
bitarray operator^(const bitarray& other) const;
bitarray& operator^=(const bitarray& other);
```

### NOT

```cpp
bitarray operator~() const;
```

v2.1 起，`operator~` 对 128/256/512-bit 使用 SIMD 指令加速。

### ANDNOT

```cpp
bitarray andnot(const bitarray& mask) const;
```

计算 `*this & ~mask`。利用原生 SIMD 指令：
- x86 SSE: `_mm_andnot_si128`
- x86 AVX: `_mm256_andnot_si256`
- ARM NEON: `vbicq_u64`

比分开做 `~mask` 再 `& mask` 快约 2 倍。

**示例**:

```cpp
bitcal::bit256 data(0xFFFF0000FFFF0000);
bitcal::bit256 mask(0xFF00FF00FF00FF00);
auto result = data.andnot(mask);  // data & ~mask
```

## 比较

```cpp
bool operator==(const bitarray& other) const;
bool operator!=(const bitarray& other) const;
```

## 零检测

```cpp
bool is_zero() const;
```

v2.1 起，对 128/256/512-bit 使用 SIMD 指令（`ptest` / `vtestz`）加速。

## 清零

```cpp
void clear();
```

将所有位设置为 0。内部使用 `std::memset`。
