# 2026-06-22 Clean Code 优化：shift DRY + 双 op 注释

## 目标

按《代码整洁之道》DRY 原则消除 `shift_left` / `shift_right` 重复，并文档化 `compose_binary_block` 双 op 参数的设计意图。

## 根因

`shift_left` / `shift_right` 两个函数 30 行结构完全相同，仅 `shift_left_array` vs `shift_right_array` 一行差异。重复骨架增加维护成本：任何对 copy + dispatch 流程的修改都要在两处同步。

`compose_binary_block` 接收 `BinaryOp` + `WordOp` 两个参数表达同一逻辑操作，调用方需手动保持同步。该设计为性能必需（小位宽快速路径需 word 级 lambda 以利内联与零初始化消除），但缺乏注释说明，读者只能猜测意图。

## 代码改动

### `include/bitcal/algorithms.hpp`

- 新增 `detail::compose_shifted_block<Bits>(value, count, shift_op)` 模板
  - 封装 copy + in-place shift 骨架
  - `shift_op` 为 lambda，路由到 `shift_left_array` 或 `shift_right_array`
- `shift_left` / `shift_right` 改为单行调用 `compose_shifted_block`，仅传入对应 shift lambda
- `compose_binary_block` 上方新增注释块，说明双 op 参数的语义与同步约束

## 收益

- shift 路径代码量 30 行 → 12 行
- 单一修改点：copy + dispatch 骨架只在 `compose_shifted_block` 一处
- 双 op 设计意图显式化，降低未来误用风险

## 验证

- `ctest` 全部通过
- `benchmark_compare` 确认 shift 路径无性能回归（lambda 正常内联）
  - `shift_left<128>` 0.27ns（持平 std::bitset）
  - `shift_right<128>` 0.53ns
  - 256/512 位 shift 与基线一致

## 后续

Clean Code 评估识别的其余优化项（dispatch DRY、view 统一、测试清理）待后续批次推进。
