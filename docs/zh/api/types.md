# 类型参考

BitCal 3.0 保留的公共类型完整参考。

> **BitCal 3.0 迁移说明：** `bitcal::ops`、`is_bitarray`、`is_bitarray_v`、`bitarray_traits` 和显式 `bit64` 转换辅助函数不再是公共 API。请使用 `bitarray` 成员函数，需要 64 位值时使用 `word(0)`。

## 目录

- [bitarray 模板](#bitarray-模板)
- [预定义类型](#预定义类型)
- [构造函数](#构造函数)
- [数据访问](#数据访问)
- [静态成员](#静态成员)
- [SIMD 后端枚举](#simd-后端枚举)

---

## bitarray 模板

所有公共 BitCal 操作的核心类模板。

```cpp
namespace bitcal {
    template<size_t Bits,
             simd_backend Backend = get_default_backend()>
    class bitarray;
}
```

### 模板参数

| 参数 | 类型 | 约束 | 描述 |
|------|------|------|------|
| `Bits` | `size_t` | `Bits >= 64` 且为 64 的倍数 | 总位数 |
| `Backend` | `simd_backend` | 目标构建可用的后端 | 操作所选的后端 |

**静态断言：**
```cpp
static_assert(Bits >= 64, "Bits must be at least 64");
static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");
```

### 内存布局

```
bitarray<256> 内存表示：
┌──────────────────────────────────────────────────────────────────────┐
│ 对齐       │  Word 0   │  Word 1   │  Word 2   │  Word 3   │ 填充   │
│  32 字节   │ 位 0-63   │位 64-127  │位 128-191 │位 192-255 │对齐用  │
└──────────────────────────────────────────────────────────────────────┘
                  小端序：word 0 存储最低有效位范围
```

**关键属性：**
- 对齐由 `get_optimal_alignment<Bits>()` 选择
- 存储为连续的 `uint64_t` 字
- 公共 API 通过 `data()` 暴露只读原始访问

---

## 预定义类型

常用位宽的便捷别名：

| 类型别名 | 定义 | 用途 | 典型后端 |
|----------|------|------|----------|
| `bit64` | `bitarray<64>` | 单字操作 | Scalar |
| `bit128` | `bitarray<128>` | 128 位 SIMD 宽度 | SSE2 / NEON / AVX-512 VL |
| `bit256` | `bitarray<256>` | 256 位 SIMD 宽度 | AVX2 / AVX-512 VL |
| `bit512` | `bitarray<512>` | 大规模并行操作 | AVX-512 或循环 AVX2 |
| `bit1024` | `bitarray<1024>` | 超大固定位宽操作 | 循环所选后端 |

### 声明

```cpp
namespace bitcal {
    using bit64   = bitarray<64>;
    using bit128  = bitarray<128>;
    using bit256  = bitarray<256>;
    using bit512  = bitarray<512>;
    using bit1024 = bitarray<1024>;
}
```

---

## 构造函数

### 默认构造函数

初始化所有位为零。

```cpp
bitarray() noexcept;
```

### 值构造函数

初始化最低 64 位，清零其余字。

```cpp
explicit bitarray(uint64_t value) noexcept;
```

| 参数 | 描述 |
|------|------|
| `value` | 存储在 word 0；高位字被零填充 |

### 拷贝和移动操作

```cpp
bitarray(const bitarray& other) noexcept = default;
bitarray(bitarray&& other) noexcept = default;
bitarray& operator=(const bitarray& other) noexcept = default;
bitarray& operator=(bitarray&& other) noexcept = default;
```

---

## 数据访问

### 原始数据指针

对底层存储的只读访问。

```cpp
const uint64_t* data() const noexcept;
```

**属性：**
- 指针在对象生命周期内保持有效
- 包含 `u64_count` 个连续字
- 使用 `set_word()` 进行修改，而不是通过指针写入

### 字访问辅助函数

```cpp
uint64_t word(size_t index) const noexcept;
void set_word(size_t index, uint64_t value) noexcept;
uint64_t operator[](size_t index) const noexcept;
```

| API | 描述 |
|-----|------|
| `word(index)` | 读取单个 64 位字，调试构建中有边界断言 |
| `set_word(index, value)` | 替换一个 64 位字 |
| `operator[](index)` | 只读索引访问 |

**示例：**
```cpp
bitcal::bit256 arr(0xDEADBEEF);
arr.set_word(1, 0xCAFEBABE);

uint64_t low = arr.word(0);
uint64_t high = arr[1];
const uint64_t* ptr = arr.data();
```

---

## 静态成员

每个 `bitarray` 特化可用的编译期常量：

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
public:
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;
};
```

**示例：**
```cpp
using MyType = bitcal::bit256;

static_assert(MyType::bits == 256);
static_assert(MyType::u64_count == 4);
```

---

## SIMD 后端枚举

可用的后端标签：

```cpp
namespace bitcal {
    enum class simd_backend {
        scalar,
        sse2,
        avx2,
        avx512,
        neon
    };
}
```

### 默认后端

```cpp
constexpr simd_backend get_default_backend() noexcept;
```

返回当前构建在编译期选择的后端。

**示例：**
```cpp
auto backend = bitcal::get_default_backend();
switch (backend) {
    case bitcal::simd_backend::avx512: std::cout << "AVX-512\n"; break;
    case bitcal::simd_backend::avx2:   std::cout << "AVX2\n"; break;
    case bitcal::simd_backend::sse2:   std::cout << "SSE2\n"; break;
    case bitcal::simd_backend::neon:   std::cout << "NEON\n"; break;
    default:                           std::cout << "Scalar\n";
}
```

---

## 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 arr(0xDEADBEEF);
    arr.set_word(1, 0xCAFEBABE);

    std::cout << "bits: " << bitcal::bit256::bits << "\n";
    std::cout << "words: " << bitcal::bit256::u64_count << "\n";
    std::cout << "word0: 0x" << std::hex << arr.word(0) << "\n";
    std::cout << "word1: 0x" << std::hex << arr[1] << "\n";

    return 0;
}
```

---

## 另见

- [核心操作](core-operations.md) — 位运算符和 ANDNOT
- [位计数](bit-counting.md) — popcount、CLZ 和 CTZ
- [架构概览](../architecture/overview.md) — 高层实现布局
