# BitCal 文档中心

<p align="center">
  <strong>现代化、跨平台、基于 SIMD 指令集加速的高性能 C++17 位运算库</strong>
</p>

<p align="center">
  <a href="getting-started/installation.html">📦 安装指南</a> •
  <a href="getting-started/quickstart.html">🚀 快速开始</a> •
  <a href="api/types.html">📚 API 参考</a> •
  <a href="architecture/overview.html">🏗️ 架构设计</a> •
  <a href="../../specs/product/bit-manipulation-library.html">📋 产品规格</a>
</p>

---

## 概述

**BitCal** 是一个高性能的 C++17 纯头文件位运算库，提供全面的位操作功能并支持自动 SIMD 加速。通过编译期分派和现代 C++ 特性，BitCal 相比标量实现可提供高达 **6 倍的性能提升**。

### 核心设计理念

- **零运行时开销**：所有 SIMD 后端选择均在编译期通过 `if constexpr` 完成
- **透明加速**：无需修改代码即可自动利用 SSE2/AVX2 (x86) 或 NEON (ARM)
- **优雅降级**：在不支持的平台上自动回退到标量实现
- **类型安全**：编译期位宽保证，支持 `constexpr` 验证

## 快速示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // 创建两个 256 位位数组
    bitcal::bit256 a(0xDEADBEEFCAFEBABE);
    bitcal::bit256 b(0x123456789ABCDEF0);
    
    // SIMD 加速的位运算
    auto c = a & b;           // 与运算: ~2.1ns (AVX2)
    auto d = a | b;           // 或运算: ~2.1ns (AVX2)
    auto e = a ^ b;           // 异或运算: ~2.1ns (AVX2)
    
    // 位计数（硬件加速）
    auto popcount = c.popcount();
    auto leading_zeros = c.count_leading_zeros();
    
    std::cout << "1 的个数: " << popcount << std::endl;
    std::cout << "前导零: " << leading_zeros << std::endl;
    
    return 0;
}
```

**编译与运行：**
```bash
g++ -std=c++17 -O3 -march=native example.cpp -o example && ./example
```

---

## 文档索引

### 🚀 入门指南

| 文档 | 描述 |
|------|------|
| [安装指南](getting-started/installation.html) | 系统要求、安装方法、验证步骤 |
| [快速开始](getting-started/quickstart.html) | 第一个 BitCal 程序、核心概念和常见用法 |
| [构建选项](getting-started/build-options.html) | 编译器标志、CMake 集成和优化建议 |

### 📚 API 参考

| 文档 | 描述 |
|------|------|
| [类型](api/types.html) | `bitarray` 模板、类型别名、内存布局 |
| [核心运算](api/core-operations.html) | 与、或、异或、非、ANDNOT、比较、状态检测 |
| [位移操作](api/shift-operations.html) | 左移、右移、跨字进位处理 |
| [位计数](api/bit-counting.html) | `popcount()`、前导零、尾部零，支持硬件加速 |
| [位操作](api/bit-manipulation.html) | 获取/设置/翻转位、`reverse()`、`clear()` |
| [SIMD 后端](api/simd-backend.html) | 后端选择、平台检测、强制指定后端 |
| [Ops 命名空间](api/ops-namespace.html) | 面向原始指针的低级函数式 API |

### 🏗️ 架构设计

| 文档 | 描述 |
|------|------|
| [概述](architecture/overview.html) | 设计原则、文件结构、性能特性 |
| [SIMD 分派](architecture/simd-dispatch.html) | 编译期分派机制、后端选择逻辑 |
| [平台支持](architecture/platform-support.html) | 支持的平台、编译器和架构 |

### 📝 版本发布说明

| 版本 | 发布日期 |
|------|----------|
| [v2.1.0](changelog/v2.1.0.html) | 2026-04-16 |
| [v2.0.0](changelog/v2.0.0.html) | 2026-01-08 |

---

## 规范文档

本项目采用**规范驱动开发（SDD）**方法。所有规范文档位于 `/specs` 目录：

| 目录 | 用途 |
|------|------|
| [产品规格](../../specs/product/bit-manipulation-library.html) | 功能需求和验收标准 |
| [架构 RFC](../../specs/rfc/) | 技术设计文档 |
| [API 规范](../../specs/api/bitcal-public-api.html) | 公共接口规范 |
| [测试规范](../../specs/testing/bitcal-testing-spec.html) | 测试覆盖要求 |

---

## 核心类型

| 类型 | 位宽 | 字数 | 最佳用途 |
|------|------|------|----------|
| `bitcal::bit64` | 64 位 | 1 | 机器字 |
| `bitcal::bit128` | 128 位 | 2 | SSE2/NEON 原生位宽 |
| `bitcal::bit256` | 256 位 | 4 | AVX2 原生位宽 |
| `bitcal::bit512` | 512 位 | 8 | 大位宽并行运算 |
| `bitcal::bit1024` | 1024 位 | 16 | 超大位宽运算 |

## SIMD 性能

| 平台 | 指令集 | 加速比 |
|------|--------|--------|
| x86-64 | AVX2 | 5-6× |
| ARM | NEON | 2.5× |

---

## 多语言支持

本文档提供多种语言版本：

- 🇨🇳 **中文**（当前）
- 🇺🇸 **[English](../en/README.html)** — English Version

---

## 获取帮助

- 📖 **完整文档**: [GitHub Pages](https://lessup.github.io/bitcal/)
- 🐛 **问题追踪**: [GitHub Issues](https://github.com/LessUp/bitcal/issues)
- 💬 **讨论区**: [GitHub Discussions](https://github.com/LessUp/bitcal/discussions)
- 📋 **更新日志**: [查看所有版本](../../CHANGELOG.html)

## 参与贡献

欢迎贡献！请参阅我们的[贡献指南](../../CONTRIBUTING.html)了解详情。

---

<p align="center">
  <sub>由 BitCal 团队和贡献者 ❤️ 精心打造</sub>
</p>
