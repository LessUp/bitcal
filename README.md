# BitCal - 高性能位运算加速库

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

BitCal 是一个现代化的、跨平台的高性能位运算加速库，针对 Linux 和 ARM 平台进行了优化。

> **当前版本**: v2.1.0

## 特性

### 🚀 性能优化
- **多指令集支持**: 
  - x86/x64: SSE2, AVX, AVX2, AVX-512
  - ARM: NEON
  - 通用标量实现作为后备
- **编译期优化**: 基于 C++17 模板和 constexpr 的零开销抽象
- **SIMD 加速**: 自动选择最优指令集实现

### 🎯 功能完整
- 位移操作（左移/右移）: 64/128/256/512/1024 位
- 逻辑运算（AND/OR/XOR/NOT）
- 位计数操作:
  - `popcount` - 统计1的个数
  - `clz` - 前导零计数
  - `ctz` - 尾部零计数
- 位反转操作
- 字节序转换

### 🏗️ 架构优雅
- **Header-only**: 无需编译，直接包含即可使用
- **模板化设计**: 编译期类型安全
- **跨平台支持**: Linux/Windows/macOS, x86/ARM
- **零依赖**: 仅依赖标准库和编译器内置函数

## 快速开始

### 要求
- C++17 或更高版本
- CMake 3.16+（仅用于构建测试）
- 支持的编译器：
  - GCC 7+
  - Clang 6+
  - MSVC 2017+

### 安装

#### 方法 1: Header-only (推荐)
```bash
# 复制 include 目录到你的项目
cp -r include/bitcal /path/to/your/project/include/
```

```cpp
#include <bitcal/bitcal.hpp>
```

#### 方法 2: CMake 集成
```bash
git clone https://github.com/user/bitcal.git
cd bitcal
mkdir build && cd build
cmake ..
make
make install
```

在你的 CMakeLists.txt 中:
```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target bitcal::bitcal)
```

### 基本使用

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // 创建 256 位的位数组
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);
    
    // 位运算
    auto c = a & b;  // AND
    auto d = a | b;  // OR
    auto e = a ^ b;  // XOR
    
    // 位移操作
    a <<= 10;  // 左移 10 位
    b >>= 5;   // 右移 5 位
    
    // 位计数
    uint64_t ones = a.popcount();              // 统计1的个数
    int leading = a.count_leading_zeros();      // 前导零
    int trailing = a.count_trailing_zeros();    // 尾部零
    
    // 位操作
    a.set_bit(42, true);   // 设置第 42 位
    bool bit = a.get_bit(42);  // 读取第 42 位
    a.flip_bit(42);        // 翻转第 42 位
    
    // 位反转
    a.reverse();
    
    std::cout << "Popcount: " << ones << std::endl;
    
    return 0;
}
```

### 高级用法

#### 自定义位宽
```cpp
// 创建 1024 位数组
bitcal::bitarray<1024> big_number;
big_number[0] = 0x123456789ABCDEF0;
big_number.shift_left(128);
```

#### 指定 SIMD 后端
```cpp
// 强制使用 AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> optimized;

// 强制使用 NEON（ARM 平台）
bitcal::bitarray<128, bitcal::simd_backend::neon> arm_optimized;

// 使用标量实现（可移植）
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable;
```

#### 低级操作
```cpp
// 直接访问内部数据
uint64_t* data = bit_array.data();
data[0] = 0xFFFFFFFFFFFFFFFF;

// 使用函数式 API
uint64_t result = bitcal::ops::popcount<256>(data);
int clz = bitcal::ops::count_leading_zeros<256>(data);
```

## 性能对比

在 Intel Core i7-12700K @ 3.6GHz 上的性能测试（单位：ns/op）:

| 操作 | 标量 | SSE2 | AVX2 | 加速比 |
|------|------|------|------|--------|
| AND-256 | 12.3 | 4.5 | 2.1 | 5.9x |
| XOR-512 | 24.8 | 9.2 | 4.3 | 5.8x |
| ShiftLeft-256 | 18.6 | 8.4 | 5.2 | 3.6x |
| Popcount-512 | 45.2 | 28.1 | 22.3 | 2.0x |

在 ARM Cortex-A72 @ 2.0GHz (Raspberry Pi 4) 上:

| 操作 | 标量 | NEON | 加速比 |
|------|------|------|--------|
| AND-128 | 8.4 | 3.2 | 2.6x |
| XOR-256 | 16.9 | 6.8 | 2.5x |
| ShiftLeft-128 | 12.5 | 5.1 | 2.5x |

## ARM 平台支持

BitCal 完全支持 ARM 平台，包括：

- **ARM32**: ARMv7-A with NEON
- **ARM64**: ARMv8-A and later
- **设备支持**: 
  - Raspberry Pi 3/4/5
  - NVIDIA Jetson series
  - Apple Silicon (M1/M2/M3)
  - AWS Graviton
  - 移动设备处理器

### ARM 编译

```bash
# ARM64 Linux
cmake -DCMAKE_CXX_FLAGS="-march=armv8-a+simd" ..

# 交叉编译
cmake -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake ..
```

## API 参考

### 类型别名
```cpp
bitcal::bit64    // 64位位数组
bitcal::bit128   // 128位位数组
bitcal::bit256   // 256位位数组
bitcal::bit512   // 512位位数组
bitcal::bit1024  // 1024位位数组
```

### 核心操作
- `shift_left(int count)` / `operator<<(int)`
- `shift_right(int count)` / `operator>>(int)`
- `operator&`, `operator|`, `operator^`, `operator~`
- `andnot(mask)` - ANDNOT (`a & ~mask`)，利用原生 SIMD 指令
- `popcount()` - 统计置位数
- `count_leading_zeros()` - CLZ
- `count_trailing_zeros()` - CTZ
- `reverse()` - 位反转
- `is_zero()` - 零检测（SIMD 加速）
- `get_bit(size_t)`, `set_bit(size_t, bool)`, `flip_bit(size_t)`

### SIMD 后端
```cpp
enum class simd_backend {
    scalar,   // 通用标量实现
    sse2,     // SSE2 (x86)
    avx,      // AVX (x86)
    avx2,     // AVX2 (x86)
    avx512,   // AVX-512 (x86)
    neon      // NEON (ARM)
};
```

## 架构设计

```
bitcal/
├── include/bitcal/
│   ├── config.hpp        # 平台检测和配置
│   ├── simd_traits.hpp   # SIMD 特征定义
│   ├── scalar_ops.hpp    # 标量操作实现
│   ├── sse_ops.hpp       # SSE/SSE2 实现
│   ├── avx_ops.hpp       # AVX/AVX2 实现
│   ├── neon_ops.hpp      # ARM NEON 实现
│   └── bitcal.hpp        # 主头文件
├── tests/                # 单元测试
├── benchmarks/           # 性能基准测试
└── examples/             # 示例代码
```

## 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。

## 致谢

- 感谢所有贡献者
- 使用了 Google Benchmark 进行性能测试
- 参考了 Boost.SIMD 和 xsimd 的设计思想

## 联系方式

- 问题反馈: GitHub Issues

---

**注意**: 本库专为高性能计算场景设计，建议在生产环境使用前进行充分测试。
