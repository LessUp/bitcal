# 快速开始

本页用一个小程序展示 vNext 的心智模型：**用 block 持有数据，用 view 借用数据，用自由算法计算结果**。

## 示例程序

```cpp
#include <array>
#include <bitcal/bitcal.hpp>
#include <cstdint>
#include <iostream>
#include <span>

int main() {
    const std::array<std::uint64_t, 4> lhs_words{0xFULL, 0x0ULL, 0xF0ULL, 0x0ULL};
    const std::array<std::uint64_t, 4> rhs_words{0x5ULL, 0x1ULL, 0xCCULL, 0x0ULL};

    auto lhs = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(lhs_words));
    auto rhs = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(rhs_words));

    auto produced = bitcal::bit_and<256>(lhs.view(), rhs.view());

    bitcal::bit_block<256> scratch;
    bitcal::and_into(lhs.view(), rhs.view(), scratch.view());

    std::cout << "word0 = 0x" << std::hex << produced.word(0) << "
";
    std::cout << "word2 = 0x" << std::hex << produced.word(2) << "
";
    std::cout << "popcount = " << std::dec << bitcal::popcount(produced.view()) << "
";
    std::cout << "scratch is zero? " << std::boolalpha << bitcal::is_zero(scratch.view()) << "
";
}
```

预期输出：

```text
word0 = 0x5
word2 = 0xc0
popcount = 4
scratch is zero? false
```

## 编译运行

```bash
g++ -std=c++23 -O3 -march=native quickstart.cpp -I/path/to/bitcal/include -o quickstart
./quickstart
```

如果你在 MSVC 上验证面向 AVX2 的开发路径，请把 GCC/Clang 标志替换成 `/std:c++23 /O2 /arch:AVX2`。

## 这里发生了什么

### 1. 用 block 持有存储

`bit_block<256>` 持有四个连续的 64 位字。

### 2. 用 view 借用存储

`view()` 会把 owning block 转成轻量借用句柄，而算法正是消费这个句柄。

### 3. 选择合适的算法形态

- `bit_and<256>()` 返回新的 owning block
- `and_into()` 把结果写入现有输出 block
- `is_zero()` 与 `popcount()` 是基于 view 的只读查询

## 什么时候用哪种算法

| 需求 | 选择 |
| --- | --- |
| 产生一个新的拥有型结果 | `bit_and<Bits>()` |
| 复用预分配输出缓冲区 | `and_into()` |
| 只读查询数据 | `is_zero()` / `popcount()` |
| 对接外部缓冲区 | `bit_block<Bits>::from_words(...)` 与 `copy_words_to(...)` |

## 按仓库基线继续验证

如果你需要的不只是单文件自检，请运行保留的 vNext 验证路径：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## 下一步

- [构建选项](./build-options.md)
- [类型参考](../api/types.md)
- [核心操作](../api/core-operations.md)
