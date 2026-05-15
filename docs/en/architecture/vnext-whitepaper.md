# BitCal vNext Whitepaper

## Thesis

BitCal vNext is built around one architectural sentence:

> **Own storage in blocks, borrow storage through views, and express work as free algorithms across a narrow backend boundary.**

That sentence drives the public API shape, the validation story, and the performance posture.

## Public model

### Owning block

`bit_block<Bits>` is the owning unit. It gives BitCal a fixed-width storage object with:

- compile-time width via `Bits`
- contiguous `std::uint64_t` words
- explicit `view()` accessors for borrowed use
- span-based import/export helpers for interoperability

```cpp
std::array<std::uint64_t, 4> words{0xFULL, 0x0ULL, 0xF0ULL, 0x0ULL};
auto block = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(words));
```

### Non-owning view

`bit_view` and `const_bit_view` let algorithms work on existing buffers without transferring ownership.

Views matter because BitCal wants algorithm composition to be cheap and explicit:

- algorithms can read or write existing storage
- callers can preallocate outputs
- ownership decisions stay visible in user code

```cpp
bitcal::bit_block<256> block;
auto writable = block.view();
bitcal::const_bit_view readable = writable;
```

### Free algorithm

Algorithms live as free functions over views and blocks.

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
bitcal::bit_block<256> out;

auto produced = bitcal::bit_and<256>(lhs.view(), rhs.view());
bitcal::and_into(lhs.view(), rhs.view(), out.view());

auto zero = bitcal::is_zero(produced.view());
auto ones = bitcal::popcount(out.view());
```

This keeps the contract centered on observable behavior instead of on a large type with an ever-growing member surface.

## Why this split exists

### 1. Ownership should be explicit

When ownership is a real type, callers can see when they allocate, copy, or retain storage. That makes the API easier to reason about and easier to compose with modern standard-library facilities.

### 2. Borrowing should be cheap

A non-owning view is the correct bridge when data already exists elsewhere. BitCal should not require temporary owning objects just to run one algorithm.

### 3. Algorithms should be reusable

Free algorithms make it easy to expose behavior over both owned and borrowed data. They also let the implementation evolve backend kernels without redefining the public storage story.

## Backend boundary

BitCal keeps exactly one stable include seam:

```cpp
#include <bitcal/bitcal.hpp>
```

Below that seam, the current implementation separates public model from execution details:

```text
bitcal/bitcal.hpp
├── bit_block.hpp
├── bit_view.hpp
├── algorithms.hpp
└── detail/
    ├── backend.hpp
    └── x64_dispatch.hpp
```

The boundary matters because BitCal wants to preserve freedom to change internal dispatch and kernel structure without forcing users to rewrite application types.

## x86-64-first validation posture

vNext is intentionally x86-64-first.

Today that means:

- scalar execution always exists as the portability floor
- compile-time target/config macros decide whether the retained x86-64 write path can use AVX2 or must stay scalar
- the retained fast path is centered on AVX2-backed `and_into()` / `bit_and<Bits>()`
- `default_backend()` remains a diagnostic summary of that build posture, not the function that routes `and_into()` / `bit_and<Bits>()`
- benchmark and correctness claims are grounded in the retained local validation path

This is a priority statement, not a claim that every platform receives the same optimization effort.

## What the whitepaper does not promise

The whitepaper is intentionally narrow. It does **not** promise:

- a compatibility shim for the older public model
- runtime CPU dispatch as part of the current contract
- identical optimization depth across every operating system and ISA
- benchmark numbers divorced from reproducible build settings and hardware context

## Validation path

The current repository baseline that supports this narrative is:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## Read next

- [SIMD Dispatch](./simd-dispatch.md) for the current execution path
- [Platform Support](./platform-support.md) for retained support boundaries
- [Types Reference](../api/types.md) and [Core Operations](../api/core-operations.md) for the public glossary in reference form
