# 安装指南

BitCal 的完整安装指南。

## 目录

- [系统要求](#系统要求)
- [安装方式](#安装方式)
- [编译器配置](#编译器配置)
- [验证安装](#验证安装)
- [故障排除](#故障排除)

---

## 系统要求

### 最低要求

| 组件 | 最低版本 | 说明 |
|------|---------|------|
| C++ 标准 | C++17 | 需要 `if constexpr` 和内联变量 |
| GCC | 7.0+ | 推荐使用 GCC 11+ 以获得最佳优化 |
| Clang | 6.0+ | 推荐使用 LLVM 14+ |
| MSVC | 2017 (19.14+) | 推荐使用 Visual Studio 2022 |
| CMake | 3.16+ | 仅构建测试/示例时需要 |

### 推荐开发环境

| 平台 | 编译器 | 架构 |
|------|--------|------|
| Linux | GCC 11+ 或 Clang 14+ | x86-64, ARM64, ARM32 |
| macOS | Apple Clang 14+ | x86-64, ARM64 (Apple Silicon) |
| Windows | MSVC 2022 | x86-64 |

### SIMD 支持矩阵

| 平台 | 架构 | SSE2 | AVX2 | NEON | 自动检测 |
|------|------|------|------|------|----------|
| Linux | x86-64 | ✅ | ✅ | N/A | `-march=native` |
| Linux | ARM64 | N/A | N/A | ✅ | `-march=armv8-a` |
| Linux | ARM32 | N/A | N/A | ✅ | `-mfpu=neon` |
| macOS | x86-64 | ✅ | ✅ | N/A | `-march=native` |
| macOS | ARM64 | N/A | N/A | ✅ | `-mcpu=apple-m1` |
| Windows | x86-64 | ✅ | ✅ | N/A | `/arch:AVX2` |

---

## 安装方式

### 方式一：复制头文件（推荐）

最简单、最常用的方法。无需编译、构建系统或包管理器。

```bash
# 步骤 1：克隆或下载
git clone https://github.com/LessUp/bitcal.git
cd bitcal

# 步骤 2：复制头文件到项目目录
cp -r include/bitcal /path/to/your/project/include/

# 或者创建 vendor 目录
mkdir -p /path/to/your/project/vendor
cp -r include/bitcal /path/to/your/project/vendor/
```

**在源代码中：**
```cpp
#include <bitcal/bitcal.hpp>  // 如果在系统 include 路径
// 或
#include "bitcal/bitcal.hpp"  // 如果在本地目录
```

**编译：**
```bash
# 基础编译（可移植，标量回退）
g++ -std=c++17 -O2 your_program.cpp -o your_program

# 推荐：启用针对当前 CPU 的 SIMD
g++ -std=c++17 -O3 -march=native your_program.cpp -o your_program
```

### 方式二：CMake find_package（系统安装）

将 BitCal 安装到系统范围，使用 CMake 的 `find_package`。

```bash
# 步骤 1：克隆并安装
git clone https://github.com/LessUp/bitcal.git
cd bitcal
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install  # 可能需要 sudo
```

**在 `CMakeLists.txt` 中：**
```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject)

find_package(bitcal REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

**构建项目：**
```bash
mkdir build && cd build
cmake ..
make
```

### 方式三：FetchContent（CMake 3.14+）

无需手动安装，自动下载并使用 BitCal 作为依赖。

**在 `CMakeLists.txt` 中：**
```cmake
cmake_minimum_required(VERSION 3.14)
project(MyProject)

include(FetchContent)
FetchContent_Declare(
    bitcal
    GIT_REPOSITORY https://github.com/LessUp/bitcal.git
    GIT_TAG v2.1.0  # 固定到特定版本
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(bitcal)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

**优势：**
- 自动依赖管理
- 版本固定确保可复现构建
- 无手动安装步骤
- 适合 CI/CD 流水线

### 方式四：Git Submodule

将 BitCal 作为 git 子模块包含，实现版本控制的依赖管理。

```bash
# 添加子模块
git submodule add https://github.com/LessUp/bitcal.git third_party/bitcal
git submodule update --init

# 固定到特定版本
cd third_party/bitcal
git checkout v2.1.0
cd ../..
git add third_party/bitcal
git commit -m "添加 bitcal v2.1.0 子模块"
```

**在 `CMakeLists.txt` 中：**
```cmake
add_subdirectory(third_party/bitcal EXCLUDE_FROM_ALL)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

---

## 编译器配置

### 优化标志

#### GCC / Clang

| 标志 | 效果 | 使用场景 |
|------|------|----------|
| `-O2` | 标准优化 | 生产构建 |
| `-O3` | 激进优化 | 生产构建（推荐） |
| `-march=native` | 针对当前 CPU 优化 | 开发、本地部署 |
| `-mavx2` | 手动启用 AVX2 | 已知目标硬件 |
| `-msse2` | 启用 SSE2（安全回退） | 最大兼容性 |

**推荐组合：**
```bash
# 开发环境（本地机器最佳性能）
g++ -std=c++17 -O3 -march=native -DNDEBUG program.cpp

# 生产部署（指定目标）
g++ -std=c++17 -O3 -mavx2 -mtune=generic program.cpp

# 最大兼容性
g++ -std=c++17 -O2 -msse2 program.cpp
```

#### MSVC

| 标志 | 效果 |
|------|------|
| `/O2` | 最大化速度 |
| `/arch:AVX2` | 启用 AVX2 |
| `/std:c++17` | C++17 标准 |

```cmd
cl /std:c++17 /O2 /arch:AVX2 /EHsc program.cpp
```

### CMake 选项

构建测试、示例或基准测试时：

| 选项 | 默认值 | 描述 |
|------|--------|------|
| `BITCAL_BUILD_TESTS` | `ON` | 构建单元测试套件 |
| `BITCAL_BUILD_EXAMPLES` | `ON` | 构建示例程序 |
| `BITCAL_BUILD_BENCHMARKS` | `OFF` | 构建 Google Benchmark 套件 |
| `BITCAL_NATIVE_ARCH` | `ON` | 启用 `-march=native` 优化 |

**示例：**
```bash
cmake -DBITCAL_BUILD_TESTS=ON \
      -DBITCAL_BUILD_BENCHMARKS=ON \
      -DBITCAL_NATIVE_ARCH=ON \
      ..
make -j$(nproc)
```

---

## 验证安装

### 快速验证测试

创建 `test_install.cpp`：
```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    std::cout << "BitCal v2.1.0 安装测试\n";
    std::cout << "======================\n\n";
    
    // 测试基本运算
    bitcal::bit256 a(0xFFFFFFFF);
    bitcal::bit256 b(0xAAAAAAAA);
    
    auto result = a & b;
    std::cout << "与运算: " << result.popcount() << " 个位被设置\n";
    
    // 显示检测到的后端
    std::cout << "SIMD 后端: ";
    switch (bitcal::get_default_backend()) {
        case bitcal::simd_backend::avx2:  std::cout << "AVX2"; break;
        case bitcal::simd_backend::sse2:  std::cout << "SSE2"; break;
        case bitcal::simd_backend::neon:  std::cout << "NEON"; break;
        default:                          std::cout << "Scalar";
    }
    std::cout << "\n";
    
    // 类型信息
    std::cout << "\n类型信息:\n";
    std::cout << "  bit256 位宽: " << bitcal::bit256::bits << "\n";
    std::cout << "  bit256 字数: " << bitcal::bit256::u64_count << "\n";
    
    std::cout << "\n✓ 所有测试通过！\n";
    return 0;
}
```

**编译并运行：**
```bash
g++ -std=c++17 -O2 -march=native test_install.cpp -o test_install
./test_install
```

**预期输出：**
```
BitCal v2.1.0 安装测试
======================

与运算: 16 个位被设置
SIMD 后端: AVX2

类型信息:
  bit256 位宽: 256
  bit256 字数: 4

✓ 所有测试通过！
```

---

## 故障排除

### "未找到 SIMD 内部函数"

**症状：**
```
fatal error: immintrin.h: No such file or directory
```

**解决方案：**
1. 检查编译器版本：`gcc --version`（必须是 7+）
2. 添加正确的标志：
   ```bash
   # GCC/Clang x86
   g++ -std=c++17 -mavx2 -O2 program.cpp
   
   # GCC ARM
   g++ -std=c++17 -march=armv8-a+simd -O2 program.cpp
   ```

### "未定义引用"错误

**原因：** BitCal 是纯头文件库，链接错误表明设置不正确。

**解决方案：**
- 确保没有尝试链接库：`g++ -std=c++17 file.cpp`（不要加 `-lbitcal`）
- 检查所有头文件是否在 include 路径中

### 性能低于预期

**诊断清单：**
1. **优化是否启用？**
   ```bash
   # 检查: 应该显示 -O2 或 -O3
   g++ -std=c++17 -Q --help=optimizers program.cpp 2>&1 | grep -E '^\s+-O'
   ```

2. **SIMD 是否激活？**
   ```bash
   g++ -std=c++17 -march=native -dM -E - < /dev/null | grep -E '(AVX|SSE|NEON)'
   ```

3. **对比后端：**
   ```cpp
   // 强制标量，然后对比
   bitcal::bitarray<256, bitcal::simd_backend::scalar> scalar_arr;
   bitcal::bit256 simd_arr;  // 默认自动选择
   ```

### CMake 找不到包

**解决方案：**
```bash
# 选项 1：指定前缀
cmake -DCMAKE_PREFIX_PATH=/path/to/bitcal/install ..

# 选项 2：使用 FetchContent（见方式三）
```

---

## 下一步

- 🚀 [快速开始](quickstart.md) — 编写第一个 BitCal 程序
- 📚 [API 参考](../api/types.md) — 探索完整 API
- 🏗️ [架构设计](../architecture/overview.md) — 了解设计原理

---

**需要更多帮助？** [在 GitHub 上提交 Issue](https://github.com/LessUp/bitcal/issues)
