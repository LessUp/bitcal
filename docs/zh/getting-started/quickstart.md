# 快速开始

本页用一个小程序展示当前已发货的心智模型：**用 block 持有数据，用 view 借用数据，用今天真实存在的自由算法计算结果**。

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
    std::cout << "scratch popcount = " << std::dec << bitcal::popcount(scratch_const.view()) << '\n';
    std::cout << "produced is zero? " << std::boolalpha << bitcal::is_zero(produced_const.view()) << '\n';
}
```

预期输出：

```text
word0 = 0x5
word2 = 0xc0
scratch popcount = 4
produced is zero? false
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

### 3. 使用当前已发货的算法族

- `bit_and<Bits>()` 返回新的 AND owning result
- `and_into()` 复用已有可写存储来写入同样的 AND 结果
- `is_zero()` 与 `popcount()` 回答基于 view 的只读问题
- `bit_or`、`bit_xor`、`bit_andnot`、`equals`、`shift_left`、`shift_right` 这些更宽的 redesign 名称并不是当前头文件中的自由算法

## 什么时候用哪种算法

| 需求 | 选择 |
| --- | --- |
| 产生新的 AND 结果 | `bit_and<Bits>()` |
| 复用可写存储写入 AND | `and_into()` |
| 只读查询数据 | `is_zero()`、`popcount()` |

## 表面提醒

本页刻意只使用今天已经发货的符号。如果 redesign 材料提到更宽的自由函数家族，请把它理解为前瞻性的架构讨论，而不是当前 quickstart 代码可以直接依赖的表面。

## 按仓库基线继续验证

如果你需要的不只是单文件自检，请运行保留的 vNext 验证路径：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --parallel
ctest --test-dir build-test --output-on-failure -C Release
```

然后从 `build-test` 构建树中运行生成的 `bitcal_benchmark` 可执行文件；如果使用 multi-config generator，请使用 `Release` 配置对应的输出位置。

## 下一步

- [构建选项](./build-options.md)
- [类型参考](../api/types.md)
- [核心操作](../api/core-operations.md)
