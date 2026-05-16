# 安装

BitCal vNext 是一个 **header-only 的 C++23 库**。安装的重点基本只有两件事：把 `include/bitcal/` 放进 include 路径，以及决定是否需要顺手跑一遍仓库内保留的验证基线。

## 环境要求

| 组件 | 基线 | 说明 |
| --- | --- | --- |
| 语言模式 | **C++23** | 接口库显式要求 `cxx_std_23` |
| 编译器 | 新版 GCC、Clang、Apple Clang 或 MSVC | 如果要走主要验证路径，优先使用 x86-64 工具链 |
| 构建系统 | 对使用者可选 | CMake 主要用于测试、示例、benchmark 与安装/导出流程 |
| 交付方式 | Header-only | 没有额外运行时库，也没有生成源码步骤 |

## 推荐安装方式

### 1. 直接 vendor 头文件

把 `include/bitcal/` 复制进项目，然后正常包含：

```cpp
#include <bitcal/bitcal.hpp>
```

示例编译命令：

```bash
g++ -std=c++23 -O3 -march=native main.cpp -I/path/to/vendor -o app
```

### 2. 作为 CMake 子目录引入

```cmake
add_subdirectory(third_party/bitcal EXCLUDE_FROM_ALL)
target_link_libraries(my_app PRIVATE bitcal)
```

`bitcal` 是一个 `INTERFACE` 库。它负责提供 include 目录与 C++23 要求，但不会替使用者决定 CPU 优化标志。

### 3. 安装后使用导出的包

```bash
cmake -S . -B build-install -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --build build-install --target install
```

用用户本地前缀可以避免权限混淆，也能把导出包放在当前用户可写的路径下。

然后在你自己的项目里：

```cmake
find_package(bitcal CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

如果你安装到了 `$HOME/.local`，配置消费项目时记得把这个前缀加入 CMake 搜索路径：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local"
```

## 编译器建议

| 工具链 | 常用命令 |
| --- | --- |
| GCC / Clang | 本机验证可用 `-std=c++23 -O3 -march=native`，需要显式 AVX2 构建时用 `-std=c++23 -O3 -mavx2` |
| MSVC | 验证面向 AVX2 的路径时使用 `/std:c++23 /O2 /arch:AVX2` |
| 泛化可移植构建 | 保持 C++23，去掉本机 CPU 标志，并接受更保守的后端选择 |

## vNext 验证路径

如果你要验证仓库保留的开发基线，而不仅仅是消费头文件，请使用：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

这条路径会在同一个构建目录中同时验证公开 include、保留的 smoke tests，以及当前 benchmark 基线。

## 快速自检

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit_block<256> block;
    return bitcal::is_zero(block.view()) ? 0 : 1;
}
```

只要它能以 C++23 和你选择的目标标志编译通过，就说明 include 路径正确，vNext 公开模型已经可达。

## 下一步

- [快速开始](./quickstart.md)
- [构建选项](./build-options.md)
- [类型参考](../api/types.md)
