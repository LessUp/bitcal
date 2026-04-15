# BitCal 重构说明

## 概述

BitCal 2.x 是一次完全重构，从 OOP 继承设计转变为现代 C++17 模板设计。

## 设计对比

### 旧设计 (v1.x)

```cpp
// 基于继承和虚函数
class BitCalBase { virtual uint64_t bitAnd64(...) = 0; };
class BitCalGpr : public BitCalBase { ... };
class BitCalXmm : public BitCalBase { ... };

// 运行时分发 + 单例
BitCal::getInstance().bitAnd64(a, b);  // 虚函数开销
```

### 新设计 (v2.x)

```cpp
// 编译期模板特化
template<size_t Bits, simd_backend Backend>
class bitarray { ... };

// 零开销抽象
bitcal::bit256 a, b;
auto c = a & b;  // 直接内联到最优指令
```

## 主要变更

| 变更项 | 旧设计 | 新设计 |
|--------|--------|--------|
| **架构** | 继承 + 虚函数 | 模板 + if constexpr |
| **分发** | 运行时 | 编译期 |
| **依赖** | 需要编译 | Header-only |
| **API** | 方法调用 | 运算符重载 |
| **SIMD** | 手动选择 | 自动选择 |

## 文件命名规范

统一使用小写字母 + 下划线：

```
include/bitcal/
├── bitcal.hpp       # 主入口
├── config.hpp       # 平台配置
├── simd_traits.hpp  # SIMD 特征
├── scalar_ops.hpp   # 标量实现
├── sse_ops.hpp      # SSE2 实现
├── avx_ops.hpp      # AVX2 实现
└── neon_ops.hpp     # NEON 实现
```

## 依赖管理

- ❌ 移除 spdlog 依赖
- ✅ 使用标准库 `<iostream>`, `<stdexcept>`
- ✅ 零外部依赖

## 符合最佳实践

新设计符合现代 C++ 库的标准：

1. ✅ **Header-only** — 易于集成
2. ✅ **编译期分发** — 零运行时开销
3. ✅ **模板化接口** — 类型安全
4. ✅ **零依赖** — 仅需标准库

## 参考项目

| 库 | 特点 |
|----|------|
| Google Highway | 现代 SIMD 抽象 |
| xsimd | C++17 模板设计 |
| simde | SIMD 模拟器 |

## 迁移建议

详见 [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)

### 快速迁移

```cpp
// 旧代码
uint64_t data[4];
BitCal::getInstance().bitAnd256(data, other, result);

// 新代码
bitcal::bit256 data, other;
auto result = data & other;
```

## 总结

BitCal 2.x 是一次成功的现代化重构，实现了：

- 🚀 更快的性能（编译期优化）
- 📦 更简单的集成（Header-only）
- 🔒 更安全的类型（模板类型检查）
- 🌍 更广的平台（x86 + ARM）
