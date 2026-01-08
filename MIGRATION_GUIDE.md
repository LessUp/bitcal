# BitCal 2.0 迁移指南

本文档帮助你从旧版 BitCal 迁移到 2.0 版本。

## 快速迁移检查清单

- [ ] 更新包含路径：`#include "BitCal.h"` → `#include <bitcal/bitcal.hpp>`
- [ ] 移除单例调用：`BitCal::getInstance()` → 直接使用类型和操作符
- [ ] 更新 CMakeLists.txt
- [ ] 测试编译和运行
- [ ] 性能验证

## 详细迁移步骤

### 1. 代码迁移模式

#### 模式 1: 位移操作

**旧代码**:
```cpp
uint64_t data[2] = {0x123, 0x456};
uint64_t carryout;
BitCal::getInstance().bitShiftLeft128(data, 10, &carryout);
```

**新代码**:
```cpp
bitcal::bit128 data(0x123);
data[1] = 0x456;
data.shift_left(10);
// 如果需要 carry，可以在 shift 前保存最高位
```

#### 模式 2: 逻辑运算

**旧代码**:
```cpp
uint64_t a[4], b[4], result[4];
// 初始化 a, b...
BitCal::getInstance().bitAnd256(a, b, result);
```

**新代码**:
```cpp
bitcal::bit256 a, b;
// 初始化 a, b...
auto result = a & b;
```

#### 模式 3: 位计数

**旧代码**:
```cpp
uint64_t data[8];
uint8_t cnt_arr[8];
uint64_t total = BitCal::getInstance().bitPopcnt512(data, cnt_arr);
```

**新代码**:
```cpp
bitcal::bit512 data;
// 初始化 data...
uint64_t total = data.popcount();
// 如果需要每个 word 的计数，可以遍历
for (size_t i = 0; i < 8; ++i) {
    uint64_t count = bitcal::ops::popcount<64>(&data[i]);
}
```

#### 模式 4: 位反转

**旧代码**:
```cpp
uint64_t input[4], output[4];
BitCal::getInstance().bitReverse256(input, output);
```

**新代码**:
```cpp
bitcal::bit256 data;
// 初始化 data...
data.reverse();
// data 现在包含反转后的结果
```

### 2. 新功能使用

#### 位操作

```cpp
bitcal::bit256 arr;

// 设置单个位
arr.set_bit(42, true);

// 读取单个位
bool bit = arr.get_bit(42);

// 翻转单个位
arr.flip_bit(42);

// 清零
arr.clear();

// 检查是否为零
if (arr.is_zero()) {
    // ...
}
```

#### 新增计数操作

```cpp
bitcal::bit256 arr;

// 前导零计数
int clz = arr.count_leading_zeros();

// 尾部零计数
int ctz = arr.count_trailing_zeros();

// popcount
uint64_t ones = arr.popcount();
```

#### 运算符

```cpp
bitcal::bit256 a, b;

// 位运算
auto c = a & b;   // AND
auto d = a | b;   // OR
auto e = a ^ b;   // XOR
auto f = ~a;      // NOT

// 位移
auto g = a << 10; // 左移
auto h = a >> 5;  // 右移

// 复合赋值
a &= b;
a |= b;
a ^= b;
a <<= 10;
a >>= 5;

// 比较
if (a == b) { }
if (a != b) { }
```

### 3. CMakeLists.txt 迁移

**旧配置**:
```cmake
add_library(bitcal
  BitCal.cpp
  BitCalGpr.cpp
  # ...
)
target_link_libraries(your_app bitcal)
```

**新配置**:
```cmake
# 使用新的 CMakeLists_new.txt 或直接添加头文件路径
target_include_directories(your_app PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 或使用安装后的版本
find_package(bitcal REQUIRED)
target_link_libraries(your_app bitcal::bitcal)
```

### 4. 性能优化提示

#### 指定 SIMD 后端

```cpp
// 让编译器自动选择最优后端
bitcal::bit256 auto_opt;

// x86: 强制使用 AVX2（如果支持）
#if BITCAL_HAS_AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> avx2_opt;
#endif

// ARM: 使用 NEON
#if BITCAL_HAS_NEON
bitcal::bitarray<128, bitcal::simd_backend::neon> neon_opt;
#endif

// 可移植的标量版本
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable;
```

#### 编译选项

**x86/x64**:
```bash
# GCC/Clang
g++ -std=c++17 -O3 -march=native main.cpp

# 或指定具体指令集
g++ -std=c++17 -O3 -mavx2 main.cpp
```

**ARM**:
```bash
# AArch64
g++ -std=c++17 -O3 -march=armv8-a+simd main.cpp

# ARMv7 with NEON
g++ -std=c++17 -O3 -mfpu=neon main.cpp
```

### 5. 常见问题

#### Q: 如何获取 shift 操作的 carry out？

A: 新版本不直接返回 carry，可以在 shift 前检查：
```cpp
bitcal::bit256 arr;
// 保存最高位用于检查 carry
uint64_t carry = (arr[3] >> 32) & 0xFFFFFFFF; // 假设 shift 32位
arr.shift_left(32);
```

#### Q: 性能是否有下降？

A: 不会。新版本通过编译期优化性能更好：
- 消除了虚函数调用
- 消除了单例访问
- 更好的内联
- 编译期 SIMD 选择

#### Q: 如何在运行时选择 SIMD 后端？

A: 新版本在编译期选择后端以获得最佳性能。如果需要运行时选择，可以使用函数指针或策略模式封装。

#### Q: 旧代码中的 EncodeBase 去哪了？

A: Encode 相关功能保持独立。如果你使用了 Encode 功能，可以继续使用旧的 `Encode.h`，或考虑迁移到新的设计模式。

### 6. 兼容性包装器（临时方案）

如果你有大量旧代码需要迁移，可以创建临时的兼容性包装器：

```cpp
// bitcal_compat.hpp
#pragma once
#include <bitcal/bitcal.hpp>

class BitCalCompat {
public:
    static BitCalCompat& getInstance() {
        static BitCalCompat instance;
        return instance;
    }
    
    uint64_t bitShiftLeft64(uint64_t* data, int count) {
        bitcal::bit64 arr(*data);
        uint64_t carry = (count > 0 && count < 64) ? (*data >> (64 - count)) : 0;
        arr.shift_left(count);
        *data = arr[0];
        return carry;
    }
    
    bool bitShiftLeft128(uint64_t* data, int count, uint64_t* carryout) {
        bitcal::bit128 arr;
        arr[0] = data[0];
        arr[1] = data[1];
        *carryout = (count > 0 && count < 64) ? (data[1] >> (64 - count)) : 0;
        arr.shift_left(count);
        data[0] = arr[0];
        data[1] = arr[1];
        return true;
    }
    
    // 添加其他需要的兼容函数...
};

// 使用别名保持兼容
using BitCal = BitCalCompat;
```

### 7. 测试验证

迁移后建议运行以下测试：

```cpp
#include <bitcal/bitcal.hpp>
#include <cassert>

void validate_migration() {
    // 基本操作测试
    bitcal::bit64 a(0xFF00FF00);
    bitcal::bit64 b(0xF0F0F0F0);
    
    auto c = a & b;
    assert(c[0] == 0xF000F000);
    
    a.shift_left(8);
    assert(a[0] == 0xFF00FF0000);
    
    // 性能测试
    // 运行 examples/performance_comparison.cpp
    
    std::cout << "迁移验证通过!" << std::endl;
}
```

## 回滚计划

如果迁移遇到问题，可以：

1. 保留旧版本代码在单独分支
2. 使用兼容性包装器
3. 逐步迁移，新旧代码并存

## 获取帮助

- 查看 `examples/` 目录下的示例代码
- 阅读 `README.md` 获取完整文档
- 运行 `tests/test_bitcal.cpp` 了解用法
- 查看 `changelog/2026-01-08-major-refactor-v2.0.md` 了解详细变更

## 总结

新版本带来的优势：
- ✅ **更快**: 编译期优化，零运行时开销
- ✅ **更简单**: 直观的运算符和方法
- ✅ **更安全**: 编译期类型检查
- ✅ **跨平台**: 完整的 ARM 支持
- ✅ **易维护**: Header-only，无需编译

建议尽快迁移以获得这些优势！
