# BitCal

<p align="center">
  <strong>Header-only C++23 bit manipulation library in active vNext redesign</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://lessup.github.io/bitcal/"><img src="https://img.shields.io/badge/docs-GitHub_Pages-blue.svg" alt="Docs"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23"></a>
  <a href="#status"><img src="https://img.shields.io/badge/status-vNext-redesign-orange.svg" alt="Status"></a>
</p>

<p align="center">
  <strong>English</strong> | <a href="README.zh-CN.md">简体中文</a>
</p>

---

## Overview

**BitCal** is a header-only bit manipulation library being redesigned for **C++23** with an **x86-64-first** performance posture.

The new public model is centered on:
- `bit_block<Bits>` for owning fixed-width storage
- `bit_view` / `const_bit_view` for non-owning access
- free algorithms such as `bit_and<Bits>()` and `and_into()`

`<bitcal/bitcal.hpp>` remains the only stable public include seam.

## Status

BitCal is in an active **vNext / 4.0.0** redesign.

- **Breaking:** the older `bitarray`-centered public model is being replaced
- **Breaking:** the minimum language baseline is now C++23
- **Breaking:** no code-level compatibility layer is planned for the older API
- **Focus:** Linux and Windows x86-64 are the primary optimization and validation targets

## Quick Start

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto out = bitcal::bit_and<256>(lhs.view(), rhs.view());

    std::cout << out.word(0) << '\n';
}
```

## API Reference

### Core Types

| Type | Description |
|------|-------------|
| `bit_block<Bits>` | Owning fixed-width storage. `Bits >= 64` and a multiple of 64 (e.g. 64, 128, 192, 256) |
| `bit_view` | Non-owning mutable view |
| `const_bit_view` | Non-owning read-only view |

### Algorithms

| Category | Functions |
|----------|-----------|
| Bitwise | `bit_and<Bits>()`, `bit_or<Bits>()`, `bit_xor<Bits>()`, `bit_andnot<Bits>()` |
| In-place bitwise | `and_into()`, `or_into()`, `xor_into()`, `andnot_into()` |
| Query | `equals()`, `is_zero()`, `popcount()` |
| Shift | `shift_left<Bits>()`, `shift_right<Bits>()` |

### Backends

x86-64: `scalar` / `avx2` (compile-time fixed via `BITCAL_HAS_AVX2`; no runtime selection)

## Build

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
```

## Documentation

- Documentation architecture policy lives in `docs/README.md`
- Project status lives in `docs/en/status/index.md` and `docs/zh/status/index.md`
- Release history lives only in `CHANGELOG.md` and `CHANGELOG.zh-CN.md`

## Migration note

The legacy `bitarray` API is no longer part of the shipped vNext public surface. BitCal 4.x expects consumers to migrate to `bit_block`, `bit_view` / `const_bit_view`, and free algorithms through `<bitcal/bitcal.hpp>`.
