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
- **Current shipped free algorithms:** `bit_and<Bits>()`, `and_into()`, `is_zero()`, and `popcount()` are the namespace-level algorithms exported today.
- **Broader redesign note:** names such as `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, and `shift_right` still belong to redesign discussion rather than current shipped free-function docs.
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
├── bit_and<Bits>()                              returning AND results
├── and_into()                                   reusing writable storage for AND
├── is_zero() / popcount()                       query algorithms
└── redesign-only free-function names            not documented as shipped surface
```

The redesign still aims at a broader block/view/algorithm story, but these docs describe only the free-function names that the public umbrella header exports today.

## Validation posture

The current contract is anchored by the retained repository validation path:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

Use the whitepaper pages to understand the model, then use the guide and reference sections to apply it.
