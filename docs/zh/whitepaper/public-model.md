# 公开模型

只有当读者不用翻内部头文件，也能分清 ownership、borrowing 与 behavior，BitCal 的公开叙事才算成立。

## owner / view / algorithm 模型

| 角色 | 主要表面 | 公共职责 |
| --- | --- | --- |
| Owner | `bit_block<Bits>` | 持有固定宽度存储、位宽与布局相关责任。 |
| Mutable borrower | `bit_view` | 对现有存储提供非拥有、可变访问。 |
| Read-only borrower | `const_bit_view` | 对现有存储提供非拥有、只读访问。 |
| Behavioral layer | 自由算法 | 用操作语义承载行为，而不是把一切塞进单个大类。 |

这个模型刻意追求显式：读者应该一眼看出谁拥有字节、谁只是借用、语义又写在什么地方。

## 稳定 include seam

`<bitcal/bitcal.hpp>` 仍然是唯一支持的公开 include seam。

这项决定同时维持三件事情：

- header-only 的接入方式保持简单；
- 内部头文件布局在重设计期间仍可演化；
- Reference 可以始终指向一个稳定入口，而不是暗示 detail 头文件天然也是对外契约。

## 契约边界

公开模型承诺的是角色清晰，而不是 backend 永久不变。

读者可以依赖：

- owner 与 view 是一等公民的公开概念；
- 算法围绕这些角色组织，而不是藏进单个状态大类；
- 稳定 include seam 仍是 `<bitcal/bitcal.hpp>`。

读者不应推断：

- 当前内部头文件名就是额外稳定入口；
- 某个 dispatch topology 或 intrinsic 序列本身就是公开 API；
- 旧 `bitarray` 命名仍是架构中心。

## 为什么旧中心必须让位

一个单体公共中心，看上去方便，代价却很高：

- 存储策略与行为语义会继续纠缠；
- borrowing 很容易退化成次级适配层；
- backend 话题不断渗进 Reference；
- 后续算法增长会表现为 method 膨胀，而不是清晰组合。

新模型选择显式角色，是因为这更容易测试、文档化、benchmark，也更容易在后端层自由迭代。
