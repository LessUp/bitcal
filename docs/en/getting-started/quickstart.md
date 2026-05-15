# Quick Start

This guide shows the vNext mental model in one small program: **own with a block, borrow with views, compute with free algorithms**.

## Example program

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

    std::cout << "word0 = 0x" << std::hex << produced.word(0) << '\n';
    std::cout << "word2 = 0x" << std::hex << produced.word(2) << '\n';
    std::cout << "popcount = " << std::dec << bitcal::popcount(produced.view()) << '\n';
    std::cout << "scratch is zero? " << std::boolalpha << bitcal::is_zero(scratch.view()) << '\n';
}
```

Expected output:

```text
word0 = 0x5
word2 = 0xc0
popcount = 4
scratch is zero? false
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

`view()` turns an owning block into a lightweight borrowed handle. That handle is what algorithms consume.

### 3. Choose the right algorithm shape

- `bit_and<256>()` returns a new owning block
- `and_into()` writes into an existing output block
- `is_zero()` and `popcount()` are read-only queries over a view

## When to use each algorithm style

| Need | Use |
| --- | --- |
| Produce a new owned result | `bit_and<Bits>()` |
| Reuse a preallocated output buffer | `and_into()` |
| Inspect data without mutation | `is_zero()` / `popcount()` |
| Bridge external buffers | `bit_block<Bits>::from_words(...)` and `copy_words_to(...)` |

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
