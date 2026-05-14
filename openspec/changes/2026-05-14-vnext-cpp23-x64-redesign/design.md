## Context

本变更定义 BitCal vNext 的设计边界，不在本任务内完成全部内核优化。目标是在进入 correctness、benchmark 和 x86-64 极限优化之前，先固定下一代公共契约、头文件分层、支持矩阵和迁移姿态。

## Goals / Non-Goals

**Goals**
- 固定 BitCal vNext 的最小语言基线为 C++23。
- 固定新的公开模型：owning `bit_block<Bits>`、non-owning `bit_view` / `const_bit_view`、以及 free algorithm surface。
- 固定 x86-64 first 的支持姿态与内部后端分派边界。
- 固定“不保留旧 API 兼容层”的迁移策略。
- 为后续 correctness / benchmark / Pages 白皮书工作建立单一规范入口。

**Non-Goals**
- 本变更不完成最终 SIMD 内核优化。
- 本变更不建立最终 benchmark 指标页。
- 本变更不承诺立即删除全部旧实现；它先冻结 vNext 契约与架构方向。

## Decisions

### 1. 继续保留 `<bitcal/bitcal.hpp>` 作为唯一稳定公开 include seam
- **Decision**：即使 vNext 重做公开 API，`<bitcal/bitcal.hpp>` 仍是唯一稳定 include seam。
- **Why**：这样可以保留 header-only 接入体验，同时允许内部目录结构继续演化。
- **Consequence**：`bit_block.hpp`、`bit_view.hpp`、`algorithms.hpp` 是公开定义所在文件，但不额外承诺为独立稳定入口。

### 2. 新公开模型改为 owning block + view + free algorithms
- **Decision**：vNext 公开契约围绕 `bit_block<Bits>`、`bit_view` / `const_bit_view`、以及 free algorithms 展开，而不是继续以 `bitarray` 成员函数为中心。
- **Why**：这让 owning storage、non-owning access 和 algorithm composition 的边界更清晰，更适合 C++23 的 `std::span` / concepts 风格。
- **Consequence**：旧 `bitarray` 契约不再是架构中心；后续实现允许彻底替换对象模型。

### 3. 支持矩阵改为 x86-64 first
- **Decision**：主支持与主优化面为 Linux / Windows x86-64（GCC / Clang / MSVC），ARM 与 macOS 退为次级目标。
- **Why**：用户目标是极限性能，必须优先把验证、benchmark 与内核优化资源集中到最重要的平台。
- **Consequence**：vNext 的平台与性能声明必须以 x86-64 retained validation path 为中心，其他平台不能继续获得更强承诺。

### 4. 不提供旧 API 兼容层
- **Decision**：vNext 不提供 `bitarray` 兼容别名、旧成员函数兼容包装或迁移适配层。
- **Why**：兼容层会抬高实现复杂度、污染 benchmark 与内核设计，并削弱“最佳设计优先”的目标。
- **Consequence**：迁移只能通过文档、版本语义和示例完成，而不是代码级双轨维护。

### 5. 公共算法与后端分派分层明确隔离
- **Decision**：公开算法层与内部后端分派层分离；后端分派集中到 `include/bitcal/detail/`，不成为公共契约。
- **Why**：这样可以让 correctness 与 benchmark 针对 observable behavior，而不把内部 dispatch topology 变成兼容负担。
- **Consequence**：后续实现可在不改变公共模型的前提下重写 x86-64 dispatch 和 SIMD kernels。

## Target Layering

```text
Stable public include
└── include/bitcal/bitcal.hpp
    ├── include/bitcal/bit_block.hpp      # owning fixed-width storage
    ├── include/bitcal/bit_view.hpp       # non-owning word views
    ├── include/bitcal/algorithms.hpp     # public free algorithms
    └── include/bitcal/detail/
        ├── backend.hpp                   # backend tags / shared contracts
        └── x64_dispatch.hpp              # x86-64-first dispatch layer
```

约束：
- `bitcal/bitcal.hpp` 是唯一稳定 include seam。
- `bit_block.hpp`、`bit_view.hpp`、`algorithms.hpp` 共同构成 vNext 的公开定义层。
- `detail/backend.hpp` 与 `detail/x64_dispatch.hpp` 是内部实现层，不构成额外的稳定入口。

## Validation Implications

- Product spec 必须从“archive-ready C++17 retained library”改写为“C++23 x86-64-first vNext redesign in progress”。
- API spec 必须定义 `bit_block` / `bit_view` / public algorithms，而不是继续以 `bitarray` retained contract 为中心。
- Testing spec 必须把 vNext contract smoke、observable view semantics、x86-64-first validation policy 写成新的测试边界。
- RFC 0001 必须改写为新的 layering，而不是继续描述 3.0 contraction。

## Risks / Mitigations

- **风险：** vNext 规范与当前实现短期强烈背离。  
  **Mitigation：** 在 proposal、design 和 tasks 中明确这是先行设计变更，后续 skeleton / correctness / optimization 按阶段对齐。

- **风险：** 放弃兼容层会提高迁移成本。  
  **Mitigation：** 用明确版本语义、示例和文档说明替代模糊兼容承诺。

- **风险：** x86-64 first 容易让其他平台文档继续漂移。  
  **Mitigation：** 在 product/testing specs 中同步收紧支持和验证措辞。
