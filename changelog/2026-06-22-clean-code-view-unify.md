# 2026-06-22 Clean Code 优化：bit_view / const_bit_view 统一

## 目标

按《代码整洁之道》消除 `bit_view` 与 `const_bit_view` 之间的接口与成员重复，并消除 `const_bit_view` 中误导性的 `protected` 成员访问（暗示未使用的继承设计）。

## 根因

两个类镜像彼此的接口：
- `word_count()`、`word()`、默认构造、`data_`/`word_count_` 成员在两类中各定义一次
- `const_bit_view` 用 `protected` 成员暗示继承意图，但 `bit_view` 未继承，形成"死设计"
- `bit_view` 需显式定义 `operator const_bit_view` 实现向上转型，本应是语言原生能力

## 代码改动

### `include/bitcal/bit_view.hpp`

- `bit_view` 改为公有继承 `const_bit_view`
- `const_bit_view` 成员从 `protected` 改为 `private`（消除误导性访问级别）
- `bit_view` 构造函数委托基类构造（`std::uint64_t*` 隐式转为 `const std::uint64_t*`）
- `bit_view::data()` 用 `const_cast` 还原可变指针，安全因 `bit_view` 仅从非 const 指针构造
- 删除 `bit_view` 的 `word_count()`、`word()` 重复声明（继承自基类）
- 删除 `bit_view` 的 `data_`/`word_count_` 成员重复（继承自基类）
- 删除显式 `operator const_bit_view`（现为公有基类向上转型，语言原生支持）
- 新增注释块说明继承关系与 `const_cast` 安全性约束

## 收益

- 消除 `word_count()`/`word()`/成员/默认构造的重复定义
- 消除误导性 `protected` 访问级别（死设计标记）
- 消除显式 `operator const_bit_view`（语言原生向上转型替代）
- `const_bit_view` 作为只读接口的单一事实源，`bit_view` 仅扩展可变访问
- 文件从 50 行 → 53 行，但消除 ~15 行重复，净减重复量

## 验证

- `ctest` 全部通过（含 `decltype(block.view())` 类型断言、`mutable_view.data()` vs `const_view.data()` 重载解析）
- `benchmark_compare` 确认无性能回归：
  - 128/192 位小位宽快速路径：`bit_and<128>` ~0.7ns，`bit_and<192>` ~1.4ns（持平）
  - 256/512 位 AVX2 路径：`bit_and<256>` ~0.73ns，`bit_and<512>` ~1.49ns（持平）

## 后续

Clean Code 评估识别的其余优化项（copy_words_helper #8、测试清理 #9/#11）待后续批次推进。
