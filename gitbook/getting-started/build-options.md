# 编译选项

## CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BITCAL_BUILD_TESTS` | ON | 构建单元测试 |
| `BITCAL_BUILD_EXAMPLES` | ON | 构建示例程序 |
| `BITCAL_NATIVE_ARCH` | ON | 为 tests/examples 启用 `-march=native`（GCC/Clang）或 `/arch:AVX2`（MSVC） |

## 编译器标志

### x86/x64

```bash
# GCC/Clang: 自动检测本机最优指令集
g++ -std=c++17 -O3 -march=native main.cpp

# 指定具体指令集
g++ -std=c++17 -O3 -mavx2 main.cpp
g++ -std=c++17 -O3 -msse2 main.cpp

# MSVC
cl /std:c++17 /O2 /arch:AVX2 main.cpp
```

### ARM

```bash
# AArch64
g++ -std=c++17 -O3 -march=armv8-a+simd main.cpp

# ARMv7 with NEON
g++ -std=c++17 -O3 -mfpu=neon main.cpp
```

## 宏定义

BitCal 会自动检测平台和指令集，以下宏由库内部定义：

| 宏 | 值 | 说明 |
|----|-----|------|
| `BITCAL_ARCH_X86` | 0/1 | x86/x64 架构 |
| `BITCAL_ARCH_ARM` | 0/1 | ARM 架构 |
| `BITCAL_HAS_SSE2` | 0/1 | SSE2 可用 |
| `BITCAL_HAS_AVX` | 0/1 | AVX 可用 |
| `BITCAL_HAS_AVX2` | 0/1 | AVX2 可用 |
| `BITCAL_HAS_AVX512` | 0/1 | AVX-512 可用 |
| `BITCAL_HAS_NEON` | 0/1 | NEON 可用 |

## 对齐控制

```cpp
// 默认对齐 64 字节（一个缓存行）
// 可在包含头文件前覆盖：
#define BITCAL_ALIGNMENT 32
#include <bitcal/bitcal.hpp>
```
