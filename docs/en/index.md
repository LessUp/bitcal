---
layout: home
hero:
  name: BitCal
  text: Header-only SIMD bit manipulation for C++17
  tagline: Zero build step. Zero dependencies. Maximum performance.
  actions:
    - theme: brand
      text: Get Started
      link: /en/getting-started/installation
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/bitcal
features:
  - icon: ⚡
    title: Header-Only
    details: Single include seam. No build system needed. Just copy headers and compile.
  - icon: 🚀
    title: SIMD Optimized
    details: Automatic dispatch to SSE2/AVX/AVX2 on x86-64 and NEON on ARM64.
  - icon: 🎯
    title: Fixed Widths
    details: bit64, bit128, bit256, bit512, bit1024, plus custom multiples of 64 bits.
---

## Quick Start

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 lhs(0xDEADBEEF);
    bitcal::bit256 rhs(0xCAFEBABE);

    auto masked = lhs.andnot(rhs);
    auto ones = masked.popcount();

    return static_cast<int>(ones);
}
```

## Platform Support

| Platform | Compiler | Status |
|----------|----------|--------|
| Linux x64 | GCC 7+ | Tested |
| Linux x64 | Clang 6+ | Tested |
| Windows x64 | MSVC 2017+ | Tested |
| macOS ARM64 | AppleClang | Tested |
| Linux ARM64 | Cross-compile | Verified |
