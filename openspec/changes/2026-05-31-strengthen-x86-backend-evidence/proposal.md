## Why

BitCal vNext is documented as C++23 and x86-64-first, but the shipped implementation and evidence path are narrower than that wording suggests:

- the public algorithm path only routes binary bitwise operations through `detail/x64_dispatch.hpp`;
- query and counting operations still use scalar word loops in the vNext path;
- AVX-512 and NEON implementation files exist, but they are not the retained public algorithm path;
- GPU, CUDA, NPU, ARM SVE/SVE2, LoongArch LSX/LASX, and vendor-specific domestic CPU acceleration have no retained implementation or validation path.

This change makes the first optimization wave honest and reviewable: strengthen the retained x86-64 CPU backend evidence without widening platform or accelerator claims.

## What Changes

- Define support-evidence language for platform claims: buildable, correct, SIMD-accelerated, and retained benchmark evidence are separate levels.
- Strengthen the x86-64 retained backend by routing runtime public query/counting calls through an x86 dispatch seam while preserving constexpr scalar behavior.
- Prefer AVX-512 only when the specific required feature macros are present; otherwise fall back to AVX2 or scalar.
- Keep this phase as compile-target ISA optimization, not runtime CPU-feature dispatch.
- Keep ARM, domestic CPU SIMD, CUDA/GPU, and NPU acceleration out of the first implementation phase.

## Impact

- OpenSpec: product, public API/testing, and architecture wording must stop implying broad acceleration support.
- Implementation: `include/bitcal/detail/x64_dispatch.hpp` and `include/bitcal/detail/word_ops.hpp`.
- Tests: internal detail tests must cover the new x86 dispatch seam and scalar-tail behavior.
- Benchmarks/docs: retained benchmark claims remain x86-64 evidence only unless new retained artifacts are added later.

