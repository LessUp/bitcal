# BitCal Project Status and Handoff

## Current posture

BitCal 正在从“功能仍可扩展的库仓库”收敛为“稳定、可归档、可交接”的终局状态。当前策略不是继续扩张功能面，而是降低维护面、统一规范源、减少工程噪音，并确保对外声明与实际验证路径一致。

## Single sources of truth

- 活动规范：`openspec/`
- legacy 规范路径：`specs/README.md`（迁移壳层）
- AI 执行规则：`AGENTS.md`
- Claude 会话补充：`CLAUDE.md`
- Copilot 项目级说明：`.github/copilot-instructions.md`
- 版本源：`include/bitcal/config.hpp`
- 文档策略：`docs/README.md`

## GLM handoff reading order

1. `AGENTS.md`
2. `CLAUDE.md`
3. `openspec/changes/archive/2026-04-26-finalize-bitcal/proposal.md`
4. `openspec/changes/archive/2026-04-26-finalize-bitcal/design.md`
5. `openspec/changes/archive/2026-04-26-finalize-bitcal/tasks.md`
6. `docs/README.md`
7. `CMakeLists.txt`
8. `tests/test_bitcal.cpp`

## MCP vs CLI Skills decision

### Keep as MCP / external-state tools

- GitHub repository metadata and release operations
- GitHub Actions inspection
- Web lookups that require fresh external state

### Keep as CLI skills / local workflow discipline

- Brainstorming, planning, OpenSpec propose/apply, debugging, TDD, review flow
- Project-local reasoning over code, docs, and tasks

### Explicitly avoid

- 为了“看起来更智能”而引入额外上下文开销大的集成
- 把内部仓库推理问题交给外部系统

## Copilot plugin evaluation

当前结论：**不引入额外项目级 Copilot Plugin**。

原因：

- 当前仓库已经有 `AGENTS.md`、`CLAUDE.md`、`.github/copilot-instructions.md` 三层约束
- BitCal 的核心问题是规范与资产收口，不是缺少新的工具入口
- 继续增加插件只会扩大维护面和上下文噪音

## AI asset policy

- `.claude/`：保留现有高价值 commands/skills 入口，不进行盲删
- `_bmad/`：不作为主流程依赖
- 新增 AI 资产必须先证明能减少维护成本，而不是增加入口数量

## Archive readiness criteria

The following gates must be satisfied before BitCal reaches full archive-ready status:

### Structural gates (completed)

- [x] `openspec/` is the only active specification source
- [x] `specs/` retained as migration shell layer only
- [x] Documentation and Pages no longer maintain large-scale HTML/Markdown mirrors
- [x] GitHub Actions converged to minimal trusted set
- [x] Release strategy aligns with header-only truth
- [x] GitHub about/homepage/topics aligned with project positioning
- [x] `AGENTS.md` / `CLAUDE.md` / `.github/copilot-instructions.md` aligned
- [x] Version source unified to `include/bitcal/config.hpp`
- [x] Clean CMake build and test passes

### Remaining contract stabilization phases (in progress)

Phase 1-4 tasks are currently active in the `contract-public-surface` branch to:
- Contract the public API surface and remove deprecated features
- Align documentation landing pages with current repository truth
- Consolidate version semantics and release workflows
- Remove low-value automation sprawl

**Current status:** Repository is in active stabilization. The structural foundations are complete, but contract finalization and documentation alignment phases remain in progress before full archive-ready state is reached.
