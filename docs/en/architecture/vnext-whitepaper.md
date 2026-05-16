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
- public metadata through `bits` and `word_count`
- explicit `view()` accessors for borrowed use

### Non-owning view

`bit_view` and `const_bit_view` let algorithms work on existing buffers without transferring ownership.

Views matter because BitCal wants algorithm composition to be cheap and explicit:

- algorithms can read existing storage through `const_bit_view`
- mutable access stays visible through `bit_view`
- ownership decisions stay visible in user code

```cpp
bitcal::bit_block<256> block;
auto writable = block.view();
writable.data()[0] = 0xFULL;

const auto& block_const = block;
auto readable = block_const.view();
auto first = readable.word(0);
```

### Free algorithms

The redesign direction centers on free functions over views and blocks, but the shipped namespace-level algorithm set is still intentionally narrow today.

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
const auto& lhs_const = lhs;
const auto& rhs_const = rhs;

auto produced = bitcal::bit_and<256>(lhs_const.view(), rhs_const.view());
bitcal::bit_block<256> scratch;
bitcal::and_into(lhs_const.view(), rhs_const.view(), scratch.view());

const auto& produced_const = produced;
const auto& scratch_const = scratch;

auto empty = bitcal::is_zero(produced_const.view());
auto ones = bitcal::popcount(scratch_const.view());
```

The current shipped free-function surface includes:

- `bit_and<Bits>()`
- `and_into()`
- `is_zero()`
- `popcount()`
- `backend_kind` as the documented backend vocabulary

Broader redesign names such as `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, and `shift_right` are not exported today as namespace-level free algorithms, so the docs do not present them as currently usable public surface.

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
- compile-time target/config macros decide whether retained x86-64 kernels can use AVX2 or must stay scalar
- the current implementation emphasis is still concentrated on the AND write path and related kernels
- `backend_kind` names the backend vocabulary visible in the public docs, while backend-selection mechanics remain implementation details unless later specified
- benchmark and correctness claims are grounded in the retained local validation path

This is a priority statement, not a claim that every platform receives the same optimization effort.

## What the whitepaper does not promise

The whitepaper is intentionally narrow. It does **not** promise:

- a compatibility shim for the older public model
- helper APIs outside the persistent public spec as retained compatibility commitments
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
