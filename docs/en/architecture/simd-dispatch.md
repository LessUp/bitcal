# SIMD Dispatch

BitCal vNext treats SIMD dispatch as an internal execution concern behind the public block/view/algorithm model.

## Public story first

Callers do not choose a backend by selecting different public storage types. They:

1. own data in `bit_block<Bits>`
2. borrow data through `bit_view` / `const_bit_view`
3. call free algorithms

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
bitcal::bit_block<256> out;

bitcal::and_into(lhs.view(), rhs.view(), out.view());
```

Everything below that point is the backend boundary.

## Current dispatch flow

```text
consumer compile flags / target ISA
        │
        ▼
config.hpp computes BITCAL_ARCH_X86 / BITCAL_HAS_SSE2 / BITCAL_HAS_AVX2 / BITCAL_HAS_AVX512
        │
        ▼
default_backend() chooses scalar / sse2 / avx2 / avx512
        │
        ▼
free algorithm entry point
        │
        ▼
detail::word_ops.hpp
        │
        ▼
detail::x64_dispatch.hpp
        │
        ├── AVX2 word loop on x86-64 when enabled
        └── scalar fallback otherwise
```

## What currently uses the dispatch layer

### `and_into()` and `bit_and<Bits>()`

The write path is currently the clearest example of the backend boundary:

- `and_into()` validates matching word counts with debug assertions
- it forwards to `detail::and_words()`
- `detail::and_words()` forwards to `and_into_x64()`
- `and_into_x64()` uses an AVX2 loop on x86-64 when available, otherwise scalar word operations

### `is_zero()` and `popcount()`

These queries currently iterate over words directly. They still benefit from the same public model, but they are not trying to present a fully vectorized story yet.

## Why BitCal keeps this boundary

The backend boundary lets BitCal evolve execution details without reshaping application-facing types.

That buys three things:

- the public glossary stays small and teachable
- x86-64 kernels can improve independently of storage ownership semantics
- scalar execution remains the portability floor for unsupported targets or generic builds

## Diagnostics, not coupling points

`backend_kind` and `default_backend()` are useful diagnostics:

```cpp
auto backend = bitcal::default_backend();
```

Use them to explain a build or record a benchmark run. Do **not** treat them as a reason to fork the public API into backend-specific application code unless you fully own that maintenance burden.

## Build knobs that affect dispatch

| Knob | Effect |
| --- | --- |
| `-march=native` | Lets GCC/Clang enable the best instructions for the current machine |
| `-mavx2` | Forces an AVX2-capable x86 build |
| `/arch:AVX2` | Enables AVX2-oriented development targets with MSVC |
| `BITCAL_NATIVE_ARCH=ON` | Applies native CPU flags to repository tests/examples/benchmarks |
| `BITCAL_NATIVE_ARCH=OFF` | Keeps development targets on a more generic portability path |

## What this page does not claim

- no runtime dispatch contract
- no promise that every query has a dedicated vector kernel today
- no promise that every non-x86 platform receives a specialized SIMD backend right now

For support boundaries, read [Platform Support](./platform-support.md). For the architectural rationale, read the [vNext Whitepaper](./vnext-whitepaper.md).
