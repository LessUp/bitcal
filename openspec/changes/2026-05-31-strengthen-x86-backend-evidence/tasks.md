## 1. Spec truth

- [x] 1.1 Add this OpenSpec change for x86 backend evidence strengthening.
- [x] 1.2 Add product/API/testing/architecture deltas for support-evidence levels and accelerator boundaries.

## 2. TDD coverage

- [x] 2.1 Add failing internal tests for `detail::popcount_x64`, `detail::is_zero_x64`, and `detail::equals_x64`.
- [x] 2.2 Cover word counts 1, 3, 4, 7, 8, and 9 to prove vector-tail behavior.
- [x] 2.3 Preserve public behavior coverage through `<bitcal/bitcal.hpp>`.

## 3. Backend implementation

- [x] 3.1 Add exact AVX-512 feature macros for x86 backend decisions.
- [x] 3.2 Prefer AVX-512 binary/query chunks when available, otherwise AVX2, otherwise scalar.
- [x] 3.3 Add AVX512VPOPCNTDQ and AVX2 popcount paths with scalar fallback.
- [x] 3.4 Route runtime word query/count functions through the x86 dispatch seam while preserving scalar constexpr evaluation.

## 4. Evidence and documentation

- [x] 4.1 Update retained specs/docs so x86 SIMD acceleration is not confused with ARM/GPU/NPU support.
- [x] 4.2 Run retained C++ verification and benchmark smoke checks.
