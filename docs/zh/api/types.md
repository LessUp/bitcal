# 类型参考

BitCal 类型、模板和类型别名的完整参考。

## 目录

- [bitarray 模板](#bitarray-模板)
- [预定义类型](#预定义类型)
- [构造函数](#构造函数)
- [数据访问](#数据访问)
- [静态成员](#静态成员)
- [SIMD 后端枚举](#simd-后端枚举)
- [类型特征](#类型特征)

---

## bitarray 模板

所有位数组操作的核心类模板。

```cpp
namespace bitcal {
    template<size_t Bits, simd_backend Backend = get_default_backend()>
    class bitarray;
}
```

### 模板参数

| 参数 | 类型 | 约束 | 描述 |
|------|------|------|------|
| `Bits` | `size_t` | 64 的倍数 | 总位数 |
| `Backend` | `simd_backend` | 任意有效后端 | 要使用的 SIMD 实现 |

**静态断言：**
```cpp
static_assert(Bits % 64 == 0, "Bits must be multiple of 64");
```

### 内存布局

```
bitarray<256, avx2> 内存表示：
┌──────────────────────────────────────────────────────────────────────┐
│ 对齐 64B  │  字 0   │  字 1    │  字 2    │  字 3    │ 补位  │
│          │bits 0-63│bits 64-127│bits 128-191│bits 192-255│至 64B│
└──────────────────────────────────────────────────────────────────────┘
                    小端序：字 0 存储最低位
```

**关键特性：**
- **对齐：** 64 字节（缓存行对齐）
- **字节序：** 小端序（LSB 在 `data_[0]` 中）
- **存储：** 连续的 `uint64_t` 数组
- **补位：** 用于保持对齐的额外空间

---

## 预定义类型

常用位宽的便捷类型别名：

| 类型别名 | 定义 | 用例 | 最优平台 |
|----------|------|------|----------|
| `bit64` | `bitarray<64>` | 机器字运算 | 所有平台 |
| `bit128` | `bitarray<128>` | SSE2/NEON 原生位宽 | x86-SSE2, ARM-NEON |
| `bit256` | `bitarray<256>` | AVX2 原生位宽 | x86-AVX2 |
| `bit512` | `bitarray<512>` | 大位宽并行运算 | AVX2（两个寄存器） |
| `bit1024` | `bitarray<1024>` | 超大位宽运算 | AVX2（四个寄存器） |

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

### 性能特性

```cpp
// bit64 - 始终标量（单个 u64）
bitcal::bit64 a(0xFF);  // 小数据最快

// bit128 - 128 位 SIMD 最优
bitcal::bit128 b;  // 单个 SSE/NEON 寄存器

// bit256 - 256 位 SIMD 最优
bitcal::bit256 c;  // 单个 AVX 寄存器，或 2× SSE，或 2× NEON

// bit512 - 512 位位宽
bitcal::bit512 d;  // 2× AVX 或 4× SSE/NEON

// bit1024 - 大位宽
bitcal::bit1024 e;  // 循环展开运算
```

**性能表（Intel i7-12700K, AVX2）：**

| 类型 | 与运算延迟 | 内存占用 | 使用的后端 |
|------|-----------|----------|------------|
| bit64 | ~1ns | 8 字节 | 标量 |
| bit128 | ~1.5ns | 16+48 字节 | SSE2 |
| bit256 | ~2.1ns | 32+32 字节 | AVX2 |
| bit512 | ~4.2ns | 64 字节 | AVX2（2 次操作） |
| bit1024 | ~8.5ns | 128 字节 | AVX2（4 次操作） |

---

## 构造函数

### 默认构造函数

将所有位初始化为零。

```cpp
bitarray();
```

**示例：**
```cpp
bitcal::bit256 arr;  // 所有 256 位都是 0
```

**性能：** 内部使用 SIMD 优化的 `clear()`。

### 值构造函数

初始化最低 64 位，其余清零。

```cpp
explicit bitarray(uint64_t value);
```

| 参数 | 描述 |
|------|------|
| `value` | 低 64 位；高位零扩展 |

**示例：**
```cpp
bitcal::bit256 arr(0xDEADBEEF);
// arr[0] = 0xDEADBEEF
// arr[1..] = 0
```

### 拷贝和移动构造函数

```cpp
bitarray(const bitarray& other);           // 拷贝
bitarray(bitarray&& other) noexcept;       // 移动
```

两者都是平凡的并由编译器生成。移动是 `noexcept` 的以保证异常安全。

### 赋值运算符

```cpp
bitarray& operator=(const bitarray& other);      // 拷贝赋值
bitarray& operator=(bitarray&& other) noexcept;  // 移动赋值
```

---

## 数据访问

### 下标运算符

访问单个 64 位字。

```cpp
uint64_t  operator[](size_t index) const;
uint64_t& operator[](size_t index);
```

| 参数 | 范围 | 描述 |
|------|------|------|
| `index` | `[0, Bits/64 - 1]` | 字索引（0 = LSB） |

**前置条件：**
```cpp
assert(index < u64_count);  // 违反则为未定义行为
```

**示例：**
```cpp
bitcal::bit256 arr(0xDEADBEEF);

arr[0] = 0x12345678;      // 设置最低 64 位
arr[1] = 0x9ABCDEF0;      // 设置第 64-127 位
arr[2] = arr[0] | arr[1]; // 读-改-写

uint64_t low = arr[0];    // 读取字
```

### 数据指针

直接访问底层存储。

```cpp
uint64_t* data() noexcept;
const uint64_t* data() const noexcept;
```

**特性：**
- 指针按 64 字节对齐
- 指向连续的 `uint64_t` 数组
- 对 `u64_count` 个元素有效

**示例：**
```cpp
bitcal::bit256 arr;
uint64_t* ptr = arr.data();

// 直接操作
ptr[0] = 0xFFFFFFFFFFFFFFFF;
std::memcpy(ptr, source, bitcal::bit256::u64_count * sizeof(uint64_t));

// 与 C API 互操作
external_function(ptr, arr.u64_count);
```

### 大小信息

```cpp
static constexpr size_t u64_count = Bits / 64;  // 64 位字数
```

---

## 静态成员

编译期常量用于类型内省：

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
public:
    static constexpr size_t bits = Bits;                    // 总位宽
    static constexpr size_t u64_count = Bits / 64;          // 64 位字数
    static constexpr simd_backend backend = Backend;        // 选定的后端
};
```

**用法：**
```cpp
using MyType = bitcal::bit256;

static_assert(MyType::bits == 256);
static_assert(MyType::u64_count == 4);
static_assert(MyType::backend == bitcal::simd_backend::avx2 ||
              MyType::backend == bitcal::simd_backend::sse2);

// 模板元编程
template<typename T>
using is_bitcal_type = std::bool_constant<
    requires { typename T::backend; }
>;
```

---

## SIMD 后端枚举

可用 SIMD 后端的枚举。

```cpp
namespace bitcal {
    enum class simd_backend {
        scalar,  // 可移植标量实现
        sse2,    // x86 SSE2 (128 位)
        avx2,    // x86 AVX2 (256 位)
        neon     // ARM NEON (128 位)
    };
}
```

### 后端选择

```cpp
// 基于编译器标志的自动选择
template<size_t Bits>
constexpr simd_backend select_best_backend() {
    #if defined(BITCAL_HAS_AVX2) && Bits >= 256
        return simd_backend::avx2;
    #elif defined(BITCAL_HAS_SSE2) && Bits >= 128
        return simd_backend::sse2;
    #elif defined(BITCAL_HAS_NEON) && Bits >= 128
        return simd_backend::neon;
    #else
        return simd_backend::scalar;
    #endif
}
```

### 默认后端

```cpp
simd_backend get_default_backend() noexcept;
```

返回为当前平台在编译时选定的后端。

**示例：**
```cpp
auto backend = bitcal::get_default_backend();
switch (backend) {
    case bitcal::simd_backend::avx2:  std::cout << "AVX2\n"; break;
    case bitcal::simd_backend::sse2:  std::cout << "SSE2\n"; break;
    case bitcal::simd_backend::neon:  std::cout << "NEON\n"; break;
    default:                          std::cout << "Scalar\n";
}
```

---

## 类型特征

BitCal 为模板元编程提供类型特征。

### is_bitarray

```cpp
template<typename T>
struct is_bitarray : std::false_type {};

template<size_t Bits, simd_backend Backend>
struct is_bitarray<bitarray<Bits, Backend>> : std::true_type {};

template<typename T>
inline constexpr bool is_bitarray_v = is_bitarray<T>::value;
```

**示例：**
```cpp
static_assert(bitcal::is_bitarray_v<bitcal::bit256>);
static_assert(!bitcal::is_bitarray_v<int>);

// SFINAE
std::enable_if_t<bitcal::is_bitarray_v<T>, void>
process_bitarray(const T& arr);
```

### bitarray_traits

```cpp
template<typename T>
struct bitarray_traits;

template<size_t Bits, simd_backend Backend>
struct bitarray_traits<bitarray<Bits, Backend>> {
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;
    
    using word_type = uint64_t;
    using backend_type = /* 实现定义 */;
};
```

---

## 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>
#include <iomanip>
#include <type_traits>

template<typename T>
void print_array_info(const char* name) {
    std::cout << name << ":\n";
    std::cout << "  位宽: " << T::bits << "\n";
    std::cout << "  字数: " << T::u64_count << "\n";
    std::cout << "  后端: ";
    switch (T::backend) {
        case bitcal::simd_backend::avx2:  std::cout << "AVX2"; break;
        case bitcal::simd_backend::sse2:  std::cout << "SSE2"; break;
        case bitcal::simd_backend::neon:  std::cout << "NEON"; break;
        default:                          std::cout << "Scalar";
    }
    std::cout << "\n\n";
}

void print_hex(const bitcal::bit256& arr) {
    std::cout << "0x";
    for (int i = 3; i >= 0; --i) {
        std::cout << std::hex << std::setfill('0') << std::setw(16) 
                  << arr[i];
    }
    std::cout << std::dec << "\n";
}

int main() {
    // 类型信息
    print_array_info<bitcal::bit64>("bit64");
    print_array_info<bitcal::bit128>("bit128");
    print_array_info<bitcal::bit256>("bit256");
    print_array_info<bitcal::bit1024>("bit1024");
    
    // 自定义类型
    using custom_512 = bitcal::bitarray<512, bitcal::simd_backend::scalar>;
    print_array_info<custom_512>("bitarray<512, scalar>");
    
    // 构造和访问
    bitcal::bit256 a;  // 默认：全零
    print_hex(a);      // 0x00000000000000000000000000000000
    
    bitcal::bit256 b(0xDEADBEEF);
    print_hex(b);      // 0x000000000000000000000000DEADBEEF
    
    // 字访问
    b[1] = 0xCAFEBABE;
    print_hex(b);      // 0x00000000CAFEBABE00000000DEADBEEF
    
    // 类型特征
    static_assert(bitcal::is_bitarray_v<bitcal::bit256>);
    static_assert(bitcal::bitarray_traits<bitcal::bit256>::bits == 256);
    
    std::cout << "\n✓ 所有类型操作成功\n";
    return 0;
}
```

---

## 另请参阅

- [核心运算](core-operations.md) — 位运算操作
- [SIMD 后端](simd-backend.md) — 后端选择详情
- [架构设计概述](../architecture/overview.md) — 设计原则
