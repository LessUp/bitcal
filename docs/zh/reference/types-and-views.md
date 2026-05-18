# 类型与视图

公开参考必须先从角色讲起，因为一旦 ownership 与 borrowing 模糊，后面的契约阅读就会失焦。

## 公共类型角色

| 表面 | 拥有关系 | 契约阅读方式 |
| --- | --- | --- |
| `bit_block<Bits>` | 拥有 | 固定宽度存储对象，负责位宽、布局与 word 级身份。 |
| `bit_view` | 借用可变 | 对现有存储提供非拥有、可变窗口；不暗示 ownership 转移。 |
| `const_bit_view` | 借用只读 | 对现有存储提供非拥有、只读窗口，用于观察、比较与算法输入。 |
| `<bitcal/bitcal.hpp>` | 稳定入口 | 公开表面的唯一支持 include seam。 |

## ownership 与 borrowing 契约

读者应该无需猜测就能得到这些结论：

- ownership 属于 `bit_block<Bits>`；
- borrowing 属于 `bit_view` 与 `const_bit_view`；
- borrowed view 是算法的正常输入，而不是低一级的适配器；
- include 建议应保持简单：除非你在维护内部实现，否则就使用 `<bitcal/bitcal.hpp>`。

## 值得长期保留的契约说明

- 位宽是公开叙事的一部分，因为 BitCal 当前明确采用 fixed-width 取向。
- borrowing 需要被写到足够清楚，确保 aliasing 与 mutability 问题能在 Reference 层被回答。
- detail 头文件可以存在，但并不会自动成为对外承诺。

## 非承诺项

本页并不承诺：

- 当前内部存储布局构成稳定 ABI；
- 每个中间辅助类型都值得进入公开文档；
- 旧 `bitarray` 命名仍是推荐心智模型。
