# 快速开始指南

在 5 分钟内上手 BitCal。

## 目录

- [第一个程序](#第一个-bitcal-程序)
- [核心概念](#核心概念)
- [API 概览](#api-概览)
- [性能建议](#性能建议)
- [下一步](#下一步)

---

## 第一个 BitCal 程序

### 1. 创建 `hello_bitcal.cpp`

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // 创建两个 256 位位数组
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);
    
    // SIMD 加速的位运算
    auto c = a & b;  // 与运算
    auto d = a | b;  // 或运算
    auto e = a ^ b;  // 异或运算
    
    // 统计置位数量（popcount）
    std::cout << "a & b 有 " << c.popcount() << " 个位被设置\n";
    std::cout << "a | b 有 " << d.popcount() << " 个位被设置\n";
    std::cout << "a ^ b 有 " << e.popcount() << " 个位不同\n";
    
    return 0;
}
```

### 2. 编译并运行

```bash
g++ -std=c++17 -O2 -march=native hello_bitcal.cpp -o hello_bitcal
./hello_bitcal
```

**预期输出：**
```
a & b 有 11 个位被设置
a | b 有 24 个位被设置
a ^ b 有 13 个位不同
```

> 💡 **提示：** `-march=native` 启用当前 CPU 支持的所有 SIMD 指令。

---

## 核心概念

### BitArray 类型

BitCal 为常用位宽提供了预定义的类型别名：

| 类型 | 位宽 | 字数 | 典型用例 |
|------|------|------|----------|
| `bit64` | 64 | 1 | 机器字运算 |
| `bit128` | 128 | 2 | SSE2/NEON 原生位宽 |
| `bit256` | 256 | 4 | AVX2 原生位宽 |
| `bit512` | 512 | 8 | 大数据并行处理 |
| `bit1024` | 1024 | 16 | 超大位宽数据 |

```cpp
// 预定义类型
bitcal::bit64 small;
bitcal::bit256 medium;
bitcal::bit1024 large;

// 自定义位宽（必须是 64 的倍数）
bitcal::bitarray<2048> custom;
```

### SIMD 后端选择

BitCal 自动选择最优后端：

```cpp
// 自动选择
bitcal::bit256 arr;  // Intel 上用 AVX2，ARM 上用 NEON

// 显式指定后端
bitcal::bitarray<256, bitcal::simd_backend::avx2>   avx_arr;
bitcal::bitarray<256, bitcal::simd_backend::neon>   neon_arr;
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable_arr;
```

**后端检测：**
```cpp
auto backend = bitcal::get_default_backend();
std::cout << "使用: " << (backend == bitcal::simd_backend::avx2 ? "AVX2" : "其他");
```

### 内存布局

```
bit256（4 × 64 位字）：
┌──────────────────────────────────────────────────────────┐
│  对齐 64B  │ 字 0     │ 字 1      │ 字 2      │ 字 3     │补位│
│           │ bits 0-63│bits 64-127│bits 128-191│bits 192-255│64B │
└──────────────────────────────────────────────────────────┘
                    小端序：字 0 存储最低位
```

---

## API 概览

### 1. 位运算

```cpp
bitcal::bit256 a(0xFF00);
bitcal::bit256 b(0x0FF0);

// 二元运算（都返回新的 bitarray）
auto and_result = a & b;       // 0x0F00
auto or_result  = a | b;       // 0xFFF0
auto xor_result = a ^ b;       // 0xF0F0
auto not_result = ~a;          // ~0xFF00
auto andnot     = a.andnot(b); // a & ~b = 0xF000（更快！）

// 复合赋值
a &= b;  // 等价于：a = a & b（可能避免临时对象）
a |= b;
a ^= b;
```

### 2. 位移操作

```cpp
bitcal::bit256 arr(1);

// 字内位移（最快）
arr <<= 10;            // 左移
arr >>= 5;             // 右移

// 跨字位移（处理进位）
arr.shift_left(128);   // 跨字边界位移
arr.shift_right(64);

// 注意事项：
// - 位移 >= 位宽时结果为零
// - 负位移是未定义行为
```

### 3. 位计数

```cpp
bitcal::bit256 arr(0xF0F0F0F0F0F0F0F0);

// 统计置位数量（人口计数）
uint64_t ones = arr.popcount();

// 前导零
int lz = arr.count_leading_zeros();

// 尾部零
int tz = arr.count_trailing_zeros();
```

### 4. 单个位操作

```cpp
bitcal::bit256 arr;

// 获取/设置/翻转
arr.set_bit(42, true);   // 将第 42 位设为 1
arr.set_bit(42, false);  // 清零第 42 位
bool bit = arr.get_bit(42);   // 读取第 42 位
arr.flip_bit(42);        // 翻转第 42 位

// 边界检查：位索引范围 [0, 位宽-1]
// 越界访问是未定义行为
```

### 5. 批量操作

```cpp
bitcal::bit256 arr(0x123456789ABCDEF0);

arr.reverse();         // 反转所有位
arr.clear();           // 清零所有位（快速 SIMD memset）

if (arr.is_zero()) {   // 快速 SIMD 零值检测
    // 所有位都是零
}
```

### 6. 数据访问

```cpp
bitcal::bit256 arr(0xDEADBEEF);

// 直接字访问
arr[0] = 0x12345678;   // 设置最低 64 位
arr[1] = 0x9ABCDEF0;   // 设置第 64-127 位
uint64_t word = arr[0]; // 获取字

// 指针访问
uint64_t* data = arr.data();
data[0] = 0xFFFFFFFFFFFFFFFF;

// 类型信息
constexpr size_t bits = bitcal::bit256::bits;      // 256
constexpr size_t words = bitcal::bit256::u64_count; // 4
```

### 7. 低级 API（ops 命名空间）

用于原始指针操作，无对象开销：

```cpp
#include <bitcal/ops.hpp>

uint64_t data[4] = {1, 2, 3, 4};

// 函数式 API
uint64_t ones = bitcal::ops::popcount<256>(data);
int lz = bitcal::ops::count_leading_zeros<256>(data);

// 工具函数
uint64_t reversed = bitcal::ops::reverse_bits_64(0xF0F0F0F0);
uint64_t swapped = bitcal::ops::byte_swap_64(0x12345678);
```

---

## 性能建议

### 1. 编译器标志很重要

```bash
# 在当前机器上最快
g++ -std=c++17 -O3 -march=native program.cpp

# 可移植性好且性能不错
g++ -std=c++17 -O3 -msse2 program.cpp
```

### 2. 优先使用 ANDNOT

```cpp
// 快速（单条指令）
auto fast = a.andnot(b);  // ~2.1ns

// 慢速（两条指令 + 临时对象）
auto slow = a & ~b;       // ~4.5ns
```

### 3. 使用复合赋值

```cpp
// 可能避免临时对象
a &= b;  // 可能更快

// 创建临时对象
a = a & b;
```

### 4. 使用 is_zero() 进行比较

```cpp
// 快速 SIMD 路径（AVX2 下约 1.8ns）
if (arr.is_zero()) { ... }

// 慢速手动比较
bool zero = (arr.popcount() == 0);  // 慢得多
```

### 5. 理解对齐

```cpp
// BitCal 类型是 64 字节对齐的
alignas(64) bitcal::bit256 arr;  // 最优

// 栈分配也正常
bitcal::bit256 arr;  // 仍然是 64 字节对齐
```

---

## 完整示例：位图操作

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>
#include <vector>

// 1024 位位图，用于追踪 1024 个标志
using Bitmap = bitcal::bit1024;

class UserPermissions {
    Bitmap permissions_;
public:
    enum Permission { READ = 0, WRITE = 1, EXECUTE = 2, ADMIN = 3 };
    
    void grant(Permission p) { permissions_.set_bit(p, true); }
    void revoke(Permission p) { permissions_.set_bit(p, false); }
    bool has(Permission p) const { return permissions_.get_bit(p); }
    bool none() const { return permissions_.is_zero(); }
    
    Bitmap get() const { return permissions_; }
    void merge(const UserPermissions& other) {
        permissions_ |= other.permissions_;
    }
};

int main() {
    UserPermissions alice, bob;
    
    alice.grant(UserPermissions::READ);
    alice.grant(UserPermissions::WRITE);
    
    bob.grant(UserPermissions::EXECUTE);
    
    std::cout << "Alice 有 " << alice.get().popcount() << " 个权限\n";
    
    // 权限并集
    alice.merge(bob);
    std::cout << "合并后: " << alice.get().popcount() << " 个权限\n";
    
    return 0;
}
```

---

## 下一步

| 主题 | 文档 | 描述 |
|------|------|------|
| 完整 API | [类型](../api/types.html) | 所有类型及其详细信息 |
| 运算 | [核心运算](../api/core-operations.html) | 位运算参考 |
| 性能 | [架构设计](../architecture/overview.html) | 设计和优化 |
| 故障排除 | [安装指南](installation.html) | 常见问题和解决方案 |

---

<p align="center">
  <sub>准备好查看完整 API 参考了吗？→ <a href="../api/types.md">继续到类型文档</a></sub>
</p>
