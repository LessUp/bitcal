# Build Options

This page collects the build knobs that matter for BitCal vNext as a **header-only C++23** library.

## Public-consumer defaults

The `bitcal` CMake target is an `INTERFACE` library with two important behaviors:

1. it publishes the BitCal include directories
2. it requires **C++23** via `target_compile_features(... cxx_std_23)`

It does **not** force `-march=native`, `/arch:AVX2`, or similar CPU-specific flags on downstream projects.

## Repository CMake options

| Option | Default | Purpose |
| --- | --- | --- |
| `BITCAL_BUILD_TESTS` | `ON` | Build the retained smoke test executable |
| `BITCAL_BUILD_EXAMPLES` | `ON` | Build repository example programs |
| `BITCAL_BUILD_BENCHMARKS` | `OFF` | Build the retained benchmark baseline |
| `BITCAL_NATIVE_ARCH` | `ON` | Apply native CPU flags to repository tests/examples/benchmarks |
| `BITCAL_ENABLE_LTO` | `ON` | Enable link-time optimization when supported |
| `BITCAL_ENABLE_HARDENING` | `OFF` | Add opt-in hardening flags on supported toolchains |

## Common build profiles

### Full local validation

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --parallel
ctest --test-dir build-test --output-on-failure -C Release
```

Then run the generated `bitcal_benchmark` executable from the `build-test` tree. For multi-config generators, use the `Release` output location.

### More portable correctness pass

```bash
cmake -S . -B build-generic -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=OFF -DBITCAL_NATIVE_ARCH=OFF
cmake --build build-generic --config Release --parallel
ctest --test-dir build-generic --output-on-failure -C Release
```

### Install/export package build

```bash
cmake -S . -B build-install -DCMAKE_BUILD_TYPE=Release
cmake --build build-install --config Release --target install --parallel
```

## Compiler flag guidance

| Toolchain | Typical flags |
| --- | --- |
| GCC / Clang | `-std=c++23 -O3 -march=native` for local native work; `-std=c++23 -O3 -mavx2` for explicit AVX2 builds |
| Apple Clang | `-std=c++23 -O3` plus an explicit architecture target when needed |
| MSVC | `/std:c++23 /O2 /arch:AVX2` when validating the AVX2-oriented path |

## Useful compile-time diagnostics

These public macros and helpers are helpful when recording a build or benchmark run:

- `BITCAL_ARCH_X86`
- `BITCAL_HAS_SSE2`
- `BITCAL_HAS_AVX2`
- `BITCAL_HAS_AVX512`
- `BITCAL_VERSION_MAJOR`, `BITCAL_VERSION_MINOR`, `BITCAL_VERSION_PATCH`
- `bitcal::default_backend()`

Treat them as diagnostics and validation aids, not as reasons to fragment application code around backend-specific public types.

## Rule of thumb

- use **native** flags when you are measuring or validating on one known machine
- turn **native** flags off when you want a more conservative portability pass
- keep the public include stable: `#include <bitcal/bitcal.hpp>`

## Read next

- [Installation](./installation.md)
- [Quick Start](./quickstart.md)
- [SIMD Dispatch](../architecture/simd-dispatch.md)
