# Architecture Overview

This section is the BitCal vNext whitepaper landing. The thesis is simple: keep ownership explicit, make views first-class, and express work as free algorithms across a narrow header-only boundary.

## Reading order

1. [vNext Whitepaper](./vnext-whitepaper.md) — the architectural thesis and public model
2. [SIMD Dispatch](./simd-dispatch.md) — how algorithms cross the backend boundary
3. [Platform Support](./platform-support.md) — what is primary, secondary, and intentionally out of contract
4. [Performance Baseline](./performance-baseline.md) — how to read the retained benchmark evidence

## Executive summary

- **Owning block:** `bit_block<Bits>` owns fixed-width contiguous `std::uint64_t` storage and exposes `view()` accessors plus `bits` / `word_count` metadata.
- **Non-owning view:** `bit_view` and `const_bit_view` let BitCal operate on contiguous words without changing ownership.
- **Persistent algorithm contract:** `bit_and<Bits>()`, `bit_or<Bits>()`, `bit_xor<Bits>()`, `bit_andnot<Bits>()`, `equals()`, `shift_left<Bits>()`, `shift_right<Bits>()`, `is_zero()`, and `popcount()` define the retained vNext API surface.
- **Current helper surface:** today's headers also expose `and_into()` for in-place AND writes, but helper entry points outside the persistent spec should not be treated as long-term compatibility promises.
- **Backend boundary:** callers include `<bitcal/bitcal.hpp>` and use the public types plus `backend_kind`; backend selection and ISA-specific kernels stay behind internal implementation headers.

## What changed in vNext

BitCal now documents one mental model from top to bottom:

- the language baseline is **C++23**
- delivery remains **header-only**
- the public model is block/view/algorithm, not one monolithic value type
- x86-64 is the primary optimization and validation target
- benchmark claims are treated as reproducible baselines, not evergreen marketing copy

## Current retained surface

```text
<bitcal/bitcal.hpp>
├── bit_block<Bits>                              owning storage
├── bit_view / const_bit_view                    borrowed word access
├── backend_kind                                 documented backend vocabulary
├── bit_and<Bits>() / bit_or<Bits>()             returning bitwise algorithms
├── bit_xor<Bits>() / bit_andnot<Bits>()         returning bitwise algorithms
├── equals() / is_zero() / popcount()            comparison and query algorithms
├── shift_left<Bits>() / shift_right<Bits>()     returning shift algorithms
└── current shipped helper: and_into()           implementation helper, not persistent spec
```

The persistent compatibility contract is the one described in `openspec/specs/api/bitcal-public-api.md`. Current implementation helpers may exist in headers, but docs should not present them as retained public promises unless OpenSpec adds them to that contract.

## Validation posture

The current contract is anchored by the retained repository validation path:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

Use the whitepaper pages to understand the model, then use the guide and reference sections to apply it.
