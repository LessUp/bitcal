# 安装指南

## 系统要求

| 项目 | 要求 |
|------|------|
| C++ 标准 | C++17 或更高 |
| 编译器 | GCC 7+, Clang 6+, MSVC 2017+ |
| CMake | 3.16+（仅构建测试/示例时需要）|

## 安装方法

### 方法 1：Header-only（推荐）

最简单的方式，无需编译：

```bash
# 下载源码
git clone https://github.com/LessUp/bitcal.git

# 复制头文件到你的项目
cp -r bitcal/include/bitcal /path/to/your/project/include/
```

在代码中使用：

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 arr(0x12345678);
    return 0;
}
```

编译：

```bash
g++ -std=c++17 -O2 main.cpp -o main

# 启用 SIMD 加速（推荐）
g++ -std=c++17 -O2 -march=native main.cpp -o main
```

### 方法 2：CMake 集成

作为 CMake 项目的一部分：

```bash
git clone https://github.com/LessUp/bitcal.git
cd bitcal
mkdir build && cd build
cmake ..
make
make install
```

在 CMakeLists.txt 中：

```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target bitcal::bitcal)
```

### 方法 3：FetchContent（CMake 3.14+）

```cmake
include(FetchContent)
FetchContent_Declare(
    bitcal
    GIT_REPOSITORY https://github.com/LessUp/bitcal.git
    GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(bitcal)

target_link_libraries(your_target bitcal::bitcal)
```

## 编译选项

### SIMD 标志

要获得最佳性能，需要启用相应的 SIMD 指令集：

**GCC/Clang (x86)**:
```bash
# 自动检测 CPU 支持的指令集
g++ -std=c++17 -O2 -march=native main.cpp

# 手动指定
g++ -std=c++17 -O2 -mavx2 main.cpp
```

**GCC/Clang (ARM)**:
```bash
# ARM64 with NEON
g++ -std=c++17 -O2 -march=armv8-a+simd main.cpp

# ARM32 with NEON
g++ -std=c++17 -O2 -mfpu=neon main.cpp
```

**MSVC**:
```cmd
cl /std:c++17 /O2 /arch:AVX2 main.cpp
```

### CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BITCAL_BUILD_TESTS` | ON | 构建单元测试 |
| `BITCAL_BUILD_EXAMPLES` | ON | 构建示例程序 |
| `BITCAL_BUILD_BENCHMARKS` | OFF | 构建性能测试 |
| `BITCAL_NATIVE_ARCH` | ON | 启用 -march=native |

```bash
cmake -DBITCAL_BUILD_TESTS=OFF -DBITCAL_NATIVE_ARCH=ON ..
```

## 验证安装

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 a(0xFFFFFFFF);
    bitcal::bit256 b(0xAAAAAAAA);

    auto result = a & b;
    std::cout << "Popcount: " << result.popcount() << std::endl;

    std::cout << "Backend: ";
    switch (bitcal::get_default_backend()) {
        case bitcal::simd_backend::avx2: std::cout << "AVX2"; break;
        case bitcal::simd_backend::sse2: std::cout << "SSE2"; break;
        case bitcal::simd_backend::neon: std::cout << "NEON"; break;
        default: std::cout << "scalar";
    }
    std::cout << std::endl;

    return 0;
}
```

预期输出：
```
Popcount: 16
Backend: AVX2  # 或 SSE2、NEON、scalar，取决于你的 CPU
```

## 下一步

- [快速上手](quickstart.md) — 学习基本用法
- [API 参考](../api/types.md) — 查看完整 API
