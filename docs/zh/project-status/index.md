# 项目状态

BitCal 当前处于明确的 vNext 重设计阶段，并面向一个允许破坏性变更的 v4.0.0 发布线，而不是“功能完整且稳定的库”阶段。

## 本节定义

- 支持姿态
- 维护边界
- 迁移预期
- 版本语义与破坏性变更策略

## 当前姿态

- C++23 是当前公开设计基线
- 优化与验证默认采用 x86-64 优先姿态
- 新的公开模型围绕 `bit_block`、`bit_view` 和自由算法展开
- vNext 不承诺为保留的 `bitarray` API 提供代码级兼容层

## 支持与维护边界

### 主支持范围

- Linux x86-64（GCC / Clang）
- Windows x86-64（MSVC）
- 新公开表面的正确性与 benchmark 可信度

### 次级范围

- Linux ARM64
- macOS ARM64
- 后续 Pages 白皮书润色与发布型性能看板

次级目标不应获得强于保留验证路径所能支撑的支持或性能承诺。

## 持续有效的仓库规则

- 活动需求以 `openspec/` 为准
- `<bitcal/bitcal.hpp>` 仍然是唯一稳定公开 include seam
- 文档、规范、代码、benchmark 与版本语义必须一起移动
- benchmark 可信度优先于过早的平台或性能宣称

## 单一真相入口

- 活动需求：`openspec/`
- 文档策略：`docs/README.md`
- 稳定公开 include seam：`<bitcal/bitcal.hpp>`
- 版本来源：`include/bitcal/config.hpp`

## 当前执行顺序

1. 先在 OpenSpec 中冻结 vNext 契约
2. 落地可编译的 C++23 公开骨架
3. 围绕新公开表面重建正确性与 benchmark 可信度
4. 在重建后的基线上优化 x86-64 内核
5. 仅在保留验证证据存在后再发布白皮书与指标

## 下一步

- 如果你正从保留的 `bitarray` 时代迁移，请先看[迁移指南](/zh/getting-started/migration)
- 如需理解目标公开模型，请看 [vNext 白皮书](/zh/architecture/vnext-whitepaper)
- 如需先建立概念路线，请看[学院](/zh/academy/)
