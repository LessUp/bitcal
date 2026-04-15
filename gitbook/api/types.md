# 类型别名

## bitarray 模板类

```cpp
template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray;
```

### 模板参数

| 参数 | 类型 | 说明 |
|------|------|------|
| `Bits` | `size_t` | 位宽，必须是 64 的倍数 |
| `Backend` | `simd_backend` | SIMD 后端，默认自动选择 |

### 内存布局

- 底层存储：`uint64_t data_[Bits/64]`
- 对齐：`alignas(64)` 字节
- 字节序：小端（`data_[0]` 存储最低位）

```
bit256 内存布局:
┌─────────────┬─────────────┬─────────────┬─────────────┐
│   data_[0]  │   data_[1]  │   data_[2]  │   data_[3]  │
│  bits 0-63  │ bits 64-127 │bits 128-191│bits 192-255 │
└─────────────┴─────────────┴─────────────┴─────────────┘
     低位                                    高位
```

## 预定义类型

| 类型别名 | 定义 | Word 数 | 典型用途 |
|----------|------|---------|----------|
| `bit64` | `bitarray<64>` | 1 | 单个机器字 |
| `bit128` | `bitarray<128>` | 2 | SSE2/NEON 原生宽度 |
| `bit256` | `bitarray<256>` | 4 | AVX2 原生宽度 |
| `bit512` | `bitarray<512>` | 8 | 大位宽数据 |
| `bit1024` | `bitarray<1024>` | 16 | 超大位宽 |

## 构造函数

```cpp
// 默认构造：所有位清零
bitarray();

// 从 uint64_t 构造：初始化低 64 位，其余清零
explicit bitarray(uint64_t value);
```

## 数据访问

```cpp
// 获取底层 word 数组指针
uint64_t* data();
const uint64_t* data() const;

// 读写单个 word
uint64_t  operator[](size_t index) const;
uint64_t& operator[](size_t index);
```

### 示例

```cpp
bitcal::bit256 arr;

// 初始化
arr[0] = 0xDEADBEEF;
arr[1] = 0xCAFEBABE;
arr[2] = 0x12345678;
arr[3] = 0x9ABCDEF0;

// 读取
std::cout << std::hex << arr[0] << std::endl;  // deadbeef

// 直接指针访问
uint64_t* ptr = arr.data();
ptr[0] = 0;  // 清零第一个 word
```

## 静态成员

```cpp
static constexpr size_t bits = Bits;              // 位宽
static constexpr size_t u64_count = Bits / 64;    // Word 数量
static constexpr simd_backend backend = Backend;  // SIMD 后端
```

## 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>
#include <iomanip>

void print_bits(const bitcal::bit256& arr) {
    for (int i = 3; i >= 0; --i) {
        std::cout << std::hex << std::setw(16) << std::setfill('0')
                  << arr[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    // 默认构造
    bitcal::bit256 a;
    print_bits(a);  // 0000000000000000 0000000000000000 ...

    // 从值构造
    bitcal::bit256 b(0xDEADBEEF);
    print_bits(b);  // 0000000000000000 ... deadbeef

    // Word 访问
    b[1] = 0xCAFEBABE;
    print_bits(b);  // 0000000000000000 ... cafebabe deadbeef

    // 静态成员
    std::cout << "bits: " << bitcal::bit256::bits << std::endl;        // 256
    std::cout << "words: " << bitcal::bit256::u64_count << std::endl;  // 4

    return 0;
}
```

## 下一步

- [核心操作](core-operations.md) — 位运算 API
- [位移操作](shift-operations.md) — 位移 API
- [SIMD 后端](simd-backend.md) — 后端选择
