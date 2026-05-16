# Quick Start

This guide shows the current shipped mental model in one small program: **own with a block, borrow with views, compute with the free algorithms that exist today**.

## Example program

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

Expected output:

```text
word0 = 0x5
word2 = 0xc0
scratch popcount = 4
produced is zero? false
```

## Build it

```bash
g++ -std=c++23 -O3 -march=native quickstart.cpp -I/path/to/bitcal/include -o quickstart
./quickstart
```

On MSVC, use `/std:c++23 /O2 /arch:AVX2` instead of the GCC/Clang flags when you want the AVX2-oriented development path.

## What happened

### 1. Own storage in blocks

`bit_block<256>` owns four contiguous 64-bit words.

### 2. Borrow storage through views

`view()` turns an owning block into a lightweight borrowed handle. The persistent API contract promises `data()`, `word_count()`, and `word(index)` on those views.

### 3. Use the shipped algorithm families

- `bit_and<Bits>()` returns a fresh owning AND result
- `and_into()` reuses writable storage for the same AND write
- `is_zero()` and `popcount()` answer read-only questions over views
- broader redesign names such as `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, and `shift_right` are not current free algorithms in the shipped headers

## When to use each algorithm style

| Need | Use |
| --- | --- |
| Produce a new AND result | `bit_and<Bits>()` |
| Reuse writable storage for AND | `and_into()` |
| Inspect data without mutation | `is_zero()`, `popcount()` |

## Surface reminder

This page intentionally stays within symbols that ship today. If redesign materials mention a broader free-function family, treat that as forward-looking architecture discussion rather than current quickstart code.

## Verify against the repository baseline

If you want more than a one-file sanity check, run the retained vNext validation path:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## Read next

- [Build Options](./build-options.md)
- [Types Reference](../api/types.md)
- [Core Operations](../api/core-operations.md)
