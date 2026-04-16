# BitCal 文档中心

<p align="center">
  <strong>现代化、跨平台、基于 SIMD 指令集加速的高性能 C++17 位运算库</strong>
</p>

<p align="center">
  <a href="../setup/installation.md">📦 安装指南</a> •
  <a href="../setup/quickstart.md">🚀 快速开始</a> •
  <a href="../api/types.md">📚 API 参考</a> •
  <a href="../architecture/overview.md">🏗️ 架构设计</a> •
  <a href="../../specs/product/bit-manipulation-library.md">📋 产品规格</a>
</p>

---

## 文档结构重整

BitCal 现在采用**规范驱动开发（SDD）**工作流。文档已重新组织：

| 目录 | 用途 |
|------|------|
| `/docs/setup/` | 安装指南、快速入门、构建选项、迁移指南 |
| `/docs/api/` | API 参考文档 |
| `/docs/architecture/` | 架构说明和设计决策 |
| `/docs/internal/` | 内部开发笔记（重构、发布详情） |
| `/specs/product/` | 产品需求和验收标准 |
| `/specs/rfc/` | 技术设计文档（RFC） |
| `/specs/api/` | 公共 API 规范 |
| `/specs/testing/` | 测试覆盖要求 |

➡️ [查看完整文档索引](../README.md)

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

## 文档结构

### 🚀 入门指南
| 文档 | 描述 | 难度 |
|------|------|------|
| [安装指南](getting-started/installation.md) | 系统要求、安装方法和验证步骤 | ⭐ 入门 |
| [快速开始](getting-started/quickstart.md) | 第一个 BitCal 程序、核心概念和常见用法 | ⭐ 入门 |
| [构建选项](getting-started/build-options.md) | 编译器标志、CMake 集成和优化建议 | ⭐⭐ 进阶 |

### 📚 API 参考
| 文档 | 描述 | 状态 |
|------|------|------|
| [类型](api/types.md) | `bitarray` 模板、类型别名、内存布局 | 完整 |
| [核心运算](api/core-operations.md) | 与、或、异或、非、ANDNOT、比较、状态检测 | 完整 |
| [位移操作](api/shift-operations.md) | 左移、右移、跨字进位处理 | 完整 |
| [位计数](api/bit-counting.md) | `popcount()`、前导零、尾部零，支持硬件加速 | 完整 |
| [位操作](api/bit-manipulation.md) | 获取/设置/翻转位、`reverse()`、`clear()` | 完整 |
| [SIMD 后端](api/simd-backend.md) | 后端选择、平台检测、强制指定后端 | 完整 |
| [Ops 命名空间](api/ops-namespace.md) | 面向原始指针的低级函数式 API | 完整 |

### 🏗️ 架构设计
| 文档 | 描述 | 受众 |
|------|------|------|
| [概述](architecture/overview.md) | 设计原则、文件结构、性能特性 | 所有用户 |
| [SIMD 分派](architecture/simd-dispatch.md) | 编译期分派机制、后端选择逻辑 | 高级用户 |
| [平台支持](architecture/platform-support.md) | 支持的平台、编译器和架构 | 所有用户 |

### 📝 版本发布说明
| 版本 | 发布日期 | 亮点 |
|------|----------|------|
| [v2.1.0](changelog/v2.1.0.md) | 2026-02-27 | ANDNOT 运算、统一分派、性能提升 |
| [v2.0.0](changelog/v2.0.0.md) | 2026-01-08 | 完全重写：纯头文件、基于模板、编译期分派 |

## 多语言支持

本文档提供多种语言版本：

- 🇨🇳 **[中文](README.md)**（当前）
- 🇺🇸 **[English](../../en/README.md)** — English Version

## 版本信息

| 属性 | 值 |
|------|-----|
| **当前版本** | v2.1.0 |
| **发布日期** | 2026年2月27日 |
| **最低 C++ 标准** | C++17 |
| **许可证** | MIT |
| **依赖** | 无（纯头文件） |

## 获取帮助

- 📖 **完整文档**: [GitHub Pages](https://lessup.github.io/bitcal/)
- 🐛 **问题追踪**: [GitHub Issues](https://github.com/LessUp/bitcal/issues)
- 💬 **讨论区**: [GitHub Discussions](https://github.com/LessUp/bitcal/discussions)
- 📋 **更新日志**: [查看所有版本](../../CHANGELOG.md)

## 参与贡献

欢迎贡献！请参阅我们的[贡献指南](../../CONTRIBUTING.md)了解：
- 代码风格和规范
- 提交拉取请求流程
- 报告问题
- 功能需求

---

<p align="center">
  <sub>由 BitCal 团队和贡献者 ❤️ 精心打造</sub>
</p>
