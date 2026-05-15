# 快速开始

本页用一个小程序展示 vNext 的心智模型：**用 block 持有数据，用 view 借用数据，用自由算法计算结果**。

## 示例程序

```cpp
#include <bitcal/bitcal.hpp>
#include <cstdint>
#include <iomanip>
#include <iostream>

int main() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto lhs_words = lhs.view();
    lhs_words.data()[0] = 0xFULL;
    lhs_words.data()[2] = 0xF0ULL;

    auto rhs_words = rhs.view();
    rhs_words.data()[0] = 0x5ULL;
    rhs_words.data()[1] = 0x1ULL;
    rhs_words.data()[2] = 0xCCULL;

    const auto& lhs_const = lhs;
    const auto& rhs_const = rhs;

    auto produced = bitcal::bit_and<256>(lhs_const.view(), rhs_const.view());

    bitcal::bit_block<256> scratch;
    bitcal::and_into(lhs_const.view(), rhs_const.view(), scratch.view());

    const auto& produced_const = produced;
    const auto& scratch_const = scratch;

    std::cout << "word0 = 0x" << std::hex << produced_const.view().word(0) << '\n';
    std::cout << "word2 = 0x" << std::hex << produced_const.view().word(2) << '\n';
    std::cout << "popcount = " << std::dec << bitcal::popcount(produced_const.view()) << '\n';
    std::cout << "scratch equals produced? "
              << std::boolalpha
              << bitcal::equals(scratch_const.view(), produced_const.view())
              << '\n';
}
```

预期输出：

```text
word0 = 0x5
word2 = 0xc0
popcount = 4
scratch equals produced? true
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

`view()` 会把 owning block 转成轻量借用句柄。持久 API 契约承诺这些 view 具备 `data()`、`word_count()` 与 `word(index)`。

### 3. 使用持久契约中的算法族

- `bit_and<Bits>()`、`bit_or<Bits>()`、`bit_xor<Bits>()`、`bit_andnot<Bits>()` 返回新的 owning result
- `shift_left<Bits>()` 与 `shift_right<Bits>()` 返回移位后的 owning result
- `equals()`、`is_zero()`、`popcount()` 回答基于 view 的只读问题
- 当前头文件还暴露了写入辅助函数 `and_into()`，但它不属于持久 API spec

## 什么时候用哪种算法

| 需求 | 选择 |
| --- | --- |
| 产生新的按位结果 | `bit_and<Bits>()`、`bit_or<Bits>()`、`bit_xor<Bits>()`、`bit_andnot<Bits>()` |
| 产生移位后的 owning result | `shift_left<Bits>()`、`shift_right<Bits>()` |
| 只读查询数据 | `equals()`、`is_zero()`、`popcount()` |
| 在当前头文件里复用可写存储 | `and_into()` helper |

## 与 spec 对齐的提醒

持久 API spec 并不承诺 `from_words(...)`、`copy_words_to(...)`、`bit_block::word(...)` 这类辅助接口。优先采用 API 参考页里定义的 block / view / algorithm 模型。

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
