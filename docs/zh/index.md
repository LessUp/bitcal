---
layout: home
hero:
  name: BitCal
  text: C++17 头文件式 SIMD 位操作库
  tagline: 零构建步骤。零依赖。极致性能。
  actions:
    - theme: brand
      text: 开始使用
      link: /zh/getting-started/installation
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/bitcal
features:
  - icon: ⚡
    title: 头文件式
    details: 单一头文件入口。无需构建系统。只需复制头文件并编译。
  - icon: 🚀
    title: SIMD 优化
    details: 自动分发到 x86-64 上的 SSE2/AVX/AVX2 和 ARM64 上的 NEON。
  - icon: 🎯
    title: 固定位宽
    details: bit64、bit128、bit256、bit512、bit1024，以及 64 位的自定义倍数。
---

## 快速开始

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

## 平台支持

| 平台 | 编译器 | 状态 |
|------|--------|------|
| Linux x64 | GCC 7+ | 已测试 |
| Linux x64 | Clang 6+ | 已测试 |
| Windows x64 | MSVC 2017+ | 已测试 |
| macOS ARM64 | AppleClang | 已测试 |
| Linux ARM64 | 交叉编译 | 已验证 |
