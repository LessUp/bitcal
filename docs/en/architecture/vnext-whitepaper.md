# BitCal vNext Technical Whitepaper

## Summary

BitCal vNext is a C++23 redesign of the library around three explicit layers:

1. **Owning storage** via `bit_block<Bits>`
2. **Non-owning access** via `bit_view` / `const_bit_view`
3. **Free algorithms** such as `bit_and<Bits>()` and `and_into()`

The redesign keeps `<bitcal/bitcal.hpp>` as the only stable public include seam while moving backend decisions into internal implementation headers.

## Design goals

- Make the public API easier to compose with modern C++23 code
- Keep the library header-only
- Prioritize x86-64 correctness and performance first
- Avoid carrying an indefinite compatibility layer for the older `bitarray` model
- Rebuild performance claims from retained benchmark evidence instead of legacy marketing language

## Public model

### `bit_block<Bits>`

`bit_block<Bits>` is the owning fixed-width storage type. It keeps:

- compile-time width
- contiguous `uint64_t` word layout
- explicit `view()` accessors
- span-based import/export helpers for integration with external buffers

### `bit_view` / `const_bit_view`

Views are the stable, non-owning way to operate on existing storage without copying.

This separation avoids overloading the owning type with every algorithm and makes kernel code easier to reuse.

### Free algorithms

Algorithms are exposed as free functions:

- `bit_and<Bits>()`
- `and_into()`
- `is_zero()`
- `popcount()`

This makes the public contract about **observable behavior** rather than about one large member-heavy class.

## Internal architecture

Current vNext layering:

```text
bitcal/bitcal.hpp
├── bit_block.hpp
├── bit_view.hpp
├── algorithms.hpp
└── detail/
    ├── backend.hpp
    └── x64_dispatch.hpp
```

### Why hide backend details?

The public API no longer encodes ISA/backend choices in the type system. Backend selection is an implementation concern so that:

- public contracts stay stable
- x86-64 kernels can evolve independently
- scalar fallbacks remain available without public API churn

## x86-64-first kernel posture

The first internal optimization path is:

- scalar fallback for portability
- AVX2 fast path for `and_into()`

Current owner storage also guarantees **32-byte alignment on x86-64**, which is a practical baseline for AVX2-era kernels.

AVX-512 remains an optional future extension, not the center of the design.

## Correctness posture

The redesign treats correctness as the precondition for optimization.

Current retained smoke coverage includes:

- block/view construction
- free algorithm smoke tests
- `is_zero()`
- `popcount()`
- `and_into()`
- x86-64 alignment guarantee for owning storage
- span-based word import/export

## Benchmark posture

The current benchmark page is intentionally labeled as a **baseline**, not a final marketing claim. It exists to:

- verify the benchmark target builds against the new public model
- provide a reproducible starting point
- create a stable comparison point before deeper kernel work

Future benchmark work should add:

- scalar vs AVX2 vs AVX-512 comparisons
- aligned vs unaligned paths
- owner vs view-oriented workloads
- real workload traces in addition to synthetic loops

## Current limitations

- The benchmark suite is still a minimal baseline, not a full methodology harness
- The public algorithm surface is still intentionally small
- Secondary targets such as ARM64 and macOS are not yet first-class optimization targets
- More kernel families still need migration from scalar-only placeholders to measured fast paths

## Reproducibility

Build and run the current local baseline with:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```
