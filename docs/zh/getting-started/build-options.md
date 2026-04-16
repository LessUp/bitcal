# 编译选项

## CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BITCAL_BUILD_TESTS` | ON | 构建单元测试 |
| `BITCAL_BUILD_EXAMPLES` | ON | 构建示例程序 |
| `BITCAL_BUILD_BENCHMARKS` | OFF | 构建性能测试 |
| `BITCAL_NATIVE_ARCH` | ON | 启用 `-march=native` 或 `/arch:AVX2` |

## 编译器标志

### x86/x64 (GCC/Clang)

```bash
# 自动检测最优指令集
g++ -std=c++17 -O3 -march=native main.cpp

# 指定具体指令集
g++ -std=c++17 -O3 -mavx2 main.cpp
g++ -std=c++17 -O3 -msse2 main.cpp
```

### x86/x64 (MSVC)

```cmd
cl /std:c++17 /O2 /arch:AVX2 main.cpp
```

### ARM (GCC/Clang)

```bash
# ARM64 with NEON
g++ -std=c++17 -O3 -march=armv8-a+simd main.cpp

# ARM32 with NEON
g++ -std=c++17 -O3 -mfpu=neon main.cpp
```

## 宏定义

以下宏由库自动检测定义：

| 宏 | 说明 |
|----|------|
| `BITCAL_ARCH_X86` | x86/x64 架构 |
| `BITCAL_ARCH_ARM` | ARM 架构 |
| `BITCAL_HAS_SSE2` | SSE2 可用 |
| `BITCAL_HAS_AVX` | AVX 可用 |
| `BITCAL_HAS_AVX2` | AVX2 可用 |
| `BITCAL_HAS_AVX512` | AVX-512 可用 |
| `BITCAL_HAS_NEON` | NEON 可用 |

## 对齐控制

```cpp
// 默认 64 字节对齐（缓存行）
// 可在包含前覆盖：
#define BITCAL_ALIGNMENT 32
#include <bitcal/bitcal.hpp>
```

## 构建示例

```bash
# 完整构建
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBITCAL_NATIVE_ARCH=ON ..
make

# 仅构建测试
cmake -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=OFF ..
make
./tests/test_bitcal
```
