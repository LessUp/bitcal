## Why

BitCal 刚完成 3.0.0 的 public-surface contraction，但用户目标已经从“稳定收口”转向下一代重设计：使用 C++23、优先追求 x86-64 极限性能、允许彻底重做公开 API，并且不再保留旧 API 兼容层。

继续在 3.0.0 的 `bitarray` 契约上做渐进修补，会同时拖累 API 设计、benchmark 重建和内核优化：

- 当前公开模型以 `bitarray` 成员函数为中心，不利于抽离 owning storage、non-owning view 和 free algorithm 三层边界。
- 当前产品与测试规范仍以 C++17、跨平台收口和 archive-ready 叙事为主，不适合作为下一代高性能重构的唯一规范源。
- 当前 benchmark 资产已经失真，后续性能结论需要建立在新的公共契约和验证边界之上。

因此需要先建立一个新的活动变更，作为 BitCal vNext 的唯一设计入口。

## What Changes

- 创建变更 `2026-05-14-vnext-cpp23-x64-redesign`，作为 BitCal 下一代重设计的唯一活动入口。
- **BREAKING**：将语言基线从 C++17 提升到 **C++23**。
- **BREAKING**：将公开模型从 `bitarray` 主导的 retained contract 转向 `bit_block`、`bit_view` / `const_bit_view` 与 free algorithms 的新契约。
- **BREAKING**：支持姿态从“稳定收口的跨平台 retained matrix”改为 **x86-64 first**；Linux / Windows x86-64 成为主验证面，ARM 与 macOS 降为次级目标。
- **BREAKING**：不提供旧 API 的兼容别名或适配层；迁移通过版本语义和文档明确说明，而非代码级兼容。
- 明确新的头文件分层、后端分派边界和后续 benchmark / correctness / Pages 白皮书工作的前置条件。

## Impact

- 影响 OpenSpec：`openspec/specs/product/bit-manipulation-library.md`、`openspec/specs/api/bitcal-public-api.md`、`openspec/specs/testing/bitcal-testing-spec.md`、`openspec/specs/rfc/0001-core-architecture.md`
- 影响实现入口：`include/bitcal/bitcal.hpp` 仍保留为唯一稳定 include seam，但其公开内容将重构为新的 C++23 契约。
- 影响测试与 benchmark：旧 `bitarray` retained contract 不再作为下一代实现目标；新的 correctness / benchmark 基线必须围绕新公共模型重建。
- 版本语义：该重设计计划目标为 **v4.0.0** 级别的 breaking release。
