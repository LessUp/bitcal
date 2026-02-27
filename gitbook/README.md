# BitCal - 高性能位运算加速库

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](../LICENSE)

**BitCal** 是一个现代化的、跨平台的高性能位运算加速库。

- **Header-only** — 零编译依赖，直接 `#include` 即可使用
- **SIMD 加速** — 自动选择 SSE2 / AVX2 / NEON 最优指令集
- **编译期优化** — C++17 模板 + `if constexpr`，零运行时开销
- **跨平台** — Linux / Windows / macOS，x86 / ARM

## 当前版本

**v2.1.0** (2026-02-27)

## 快速开始

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;       // AND
    auto d = a | b;       // OR
    auto e = a ^ b;       // XOR
    auto f = a.andnot(b); // a & ~b

    a <<= 10;
    uint64_t ones = a.popcount();
    return 0;
}
```

## 目录

请通过左侧导航栏浏览完整文档。
