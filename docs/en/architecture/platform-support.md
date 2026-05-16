# Platform Support

This page defines the retained support boundary for BitCal vNext.

## Contract summary

| Area | Status | Meaning |
| --- | --- | --- |
| Language baseline | **Required** | Public and development targets assume **C++23** |
| Delivery model | **Stable** | BitCal remains **header-only** |
| Primary optimization target | **Primary** | x86-64 with compile-time target/config-driven dispatch |
| Portability floor | **Retained** | Scalar execution remains available when SIMD paths are not enabled |

## Supported-by-design matrix

| Target | Status | What BitCal currently promises |
| --- | --- | --- |
| Linux x86-64 | **Primary validated path** | GCC or Clang builds, compile-time backend selection, retained correctness and benchmark path |
| Windows x86-64 | **Primary validation target** | MSVC CI runs the same public model as a native build/test path; `/arch:AVX2` enables AVX2-oriented development targets |
| macOS ARM64 | **Secondary validated path** | Apple Clang CI validates the public model on Apple Silicon runners; this is retained integration coverage, not the x86-64 optimization center |
| Linux ARM64 | **Retained secondary build-only path** | The retained support matrix keeps ARM64 visible through cross-compile build coverage; it is not a retained native test or benchmark truth path |
| Non-x86 targets | **Portable floor only** | Public model may still build through scalar code paths, but these targets do not define BitCal's optimization contract |

## Backend boundary in practice

The public model does not encode the backend into storage types. Instead, callers interact with:

- `bit_block<Bits>`
- `bit_view` / `const_bit_view`
- free algorithms
- optional diagnostics such as `backend_kind` and `default_backend()`

Current backend kinds are:

```cpp
enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512,
};
```

`default_backend()` reports the backend label implied by the active compile target/config. On non-x86 builds today, that label is `scalar`; treat it as a build summary, not as a guarantee of the exact runtime kernel path for every call.

For the retained write path, the actual algorithm flow is `bit_and<Bits>()` / `and_into()` → `detail::and_words()` → `detail::x64_dispatch.hpp`, with the build target deciding whether the x86-64 AVX2 loop is compiled in. `default_backend()` is best read as a diagnostic summary of that build state.

## Compiler expectations

| Toolchain | Recommended baseline |
| --- | --- |
| GCC / Clang / Apple Clang | `-std=c++23 -O3` plus an explicit target such as `-march=native` or `-mavx2` when appropriate |
| MSVC | `/std:c++23 /O2` plus `/arch:AVX2` when you want AVX2-enabled development targets |
| CMake consumers | Link the `bitcal` interface target; choose your own CPU flags in the consumer build |

The repository's development targets use `BITCAL_NATIVE_ARCH` to add native SIMD flags for tests, examples, and benchmarks. The interface library itself does **not** force those flags onto downstream consumers.

## What is intentionally out of contract

BitCal vNext does not currently promise:

- runtime CPU dispatch
- equal optimization depth across every ISA
- separate public type families for each backend
- performance claims detached from the retained build and benchmark path

## Validation path

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --parallel
ctest --test-dir build-test --output-on-failure -C Release
```

Then run the generated `bitcal_benchmark` executable from the `build-test` tree. For multi-config generators, use the `Release` output location.
