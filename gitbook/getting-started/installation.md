# 安装指南

## 方法 1: Header-only（推荐）

BitCal 是 header-only 库，直接复制头文件即可使用：

```bash
cp -r include/bitcal /path/to/your/project/include/
```

在代码中包含：

```cpp
#include <bitcal/bitcal.hpp>
```

## 方法 2: CMake 集成

### 作为子目录

```cmake
add_subdirectory(path/to/bitcal)
target_link_libraries(your_target bitcal::bitcal)
```

### 安装后使用

```bash
git clone https://github.com/LessUp/bitcal.git
cd bitcal
mkdir build && cd build
cmake ..
make install
```

在你的 `CMakeLists.txt` 中：

```cmake
find_package(bitcal REQUIRED)
target_link_libraries(your_target bitcal::bitcal)
```

## 要求

| 项目 | 最低版本 |
|------|----------|
| C++ 标准 | C++17 |
| CMake | 3.16+（仅构建测试时需要） |
| GCC | 7+ |
| Clang | 6+ |
| MSVC | 2017+ |
| Apple Clang | 支持 |
