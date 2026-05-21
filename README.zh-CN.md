# BitCal

<p align="center">
  <strong>纯头文件 C++23 位运算库，正在进行 vNext 重设计</strong>
</p>

<p align="center">
  <a href="https://github.com/LessUp/bitcal/actions/workflows/ci.yml"><img src="https://github.com/LessUp/bitcal/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://lessup.github.io/bitcal/"><img src="https://img.shields.io/badge/docs-GitHub_Pages-blue.svg" alt="文档"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="许可证"></a>
  <a href="https://en.cppreference.com/w/cpp/23"><img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23"></a>
  <a href="#状态"><img src="https://img.shields.io/badge/status-vNext-redesign-orange.svg" alt="状态"></a>
</p>

<p align="center">
  <a href="README.md">English</a> | <strong>简体中文</strong>
</p>

---

## 概述

**BitCal** 是一个纯头文件位运算库，当前正朝 **C++23** 与 **x86-64 优先** 的方向重设计。

新的公开模型围绕以下三层展开：
- `bit_block<Bits>`：拥有固定宽度位存储
- `bit_view` / `const_bit_view`：非拥有视图
- `bit_and<Bits>()` 这类自由算法

`<bitcal/bitcal.hpp>` 仍然是唯一稳定的公开 include seam。

## 状态

BitCal 当前处于 **vNext / 4.0.0** 的活动重设计阶段。

- **破坏性变更**：旧的 `bitarray` 中心公开模型正在被替换
- **破坏性变更**：最小语言基线提升到 C++23
- **破坏性变更**：不计划保留旧 API 的代码级兼容层
- **重点**：Linux / Windows x86-64 是主要优化与验证目标

## 快速开始

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

## 构建

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
```

## 文档

- 活动需求的规范真相：`openspec/`
- 文档架构策略：`docs/README.md`
- 项目状态说明：`docs/en/status/index.md` 与 `docs/zh/status/index.md`
- 版本历史只保留在 `CHANGELOG.md` 与 `CHANGELOG.zh-CN.md`

## 迁移说明

旧的 `bitarray` API 已不再属于当前 shipped vNext 公共表面。BitCal 4.x 期望使用者迁移到 `bit_block`、`bit_view` / `const_bit_view` 与通过 `<bitcal/bitcal.hpp>` 提供的自由算法模型。
