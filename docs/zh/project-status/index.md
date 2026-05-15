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

## 详细状态页

这个本地化页面是新的主导航入口。完整的支持与交接细节仍保留在 [/project-status](/project-status) 页面中，可作为详细状态台账查阅。
