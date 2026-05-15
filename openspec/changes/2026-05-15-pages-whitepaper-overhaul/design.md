## Context

本变更在 Task 1 先建立 Pages whitepaper overhaul 的规范与文档治理边界，后续任务再实现导航、主题组件与页面重写。当前变更包缺少仓库约定的 OpenSpec 目录形态，`design.md` 也没有把 Task 1 需要先冻结的文档契约讲清楚，导致后续站点改造容易再次漂移。

## Goals / Non-Goals

**Goals**
- 让本变更包符合仓库既有的 OpenSpec change layout：`.openspec.yaml`、`proposal.md`、`design.md`、`tasks.md`、`specs/<capability>/spec.md`。
- 固定 Task 1 要表达的文档系统契约：Pages IA 要分层、双语主路径要镜像维护、`docs/README.md` 要写清 canonical source policy。
- 先把后续白皮书站点改造所依赖的“单一真相”写清楚，避免实现阶段再靠口头约定补规则。

**Non-Goals**
- 本设计文档不展开 VitePress 导航、首页、白皮书正文或双语页面内容的具体实现方案。
- 本设计文档不新增构建脚本、校验器或新的文档生成流程。
- 本设计文档不处理 Task 1 之外的 changelog 页面漂移，只定义其 canonical/derived 关系。

## Decisions

### 1. 变更包采用仓库既有 capability layout
- **Decision**：将文档站点增量放在 `specs/project-documentation-site/spec.md`，并补齐 `.openspec.yaml`。
- **Why**：仓库中已有 change package 都按 `specs/<capability>/spec.md` 组织；继续使用 `specs/project/project-documentation-site.md` 会让本变更成为异常结构。
- **Consequence**：后续 review、archive 与 OpenSpec 工具都可以按统一结构读取该变更。

### 2. Task 1 先冻结 Pages contract，后续任务再实现站点改造
- **Decision**：本设计文档只细化 Task 1 需要先冻结的信息架构分层、双语结构镜像、canonical source/changelog policy 三类约束；导航重写、主题组件和页面迁移仍按 proposal/tasks 在后续任务落实。
- **Why**：Task 1 的作用是先建立 contract，避免后续导航和内容重写在没有单一真相的情况下继续漂移。
- **Consequence**：本变更仍保留后续站点重构任务，但这些任务必须受 Task 1 先冻结的 contract 约束。

### 3. 根 changelog 保持 canonical，Pages release-notes 仅作 derived entry pages
- **Decision**：`CHANGELOG.md` / `CHANGELOG.zh-CN.md` 继续作为唯一 canonical changelog source；`docs/*/release-notes/changelog.md` 若保留，只能作为面向 Pages 导航的派生入口页。
- **Why**：这同时满足仓库“单一真相”原则与 Pages 站点对 release-notes 导航的需要，避免手工双轨维护。
- **Consequence**：`docs/README.md` 需要明确区分 canonical source 与 Pages-derived entry pages，清理规则也应只删除手工镜像或失去同步依据的版本。

### 4. 双语镜像要求绑定主受众导航结构
- **Decision**：对 Home / Academy / Whitepaper / Guides / Reference / Research / Project Status 这类主阅读路径，英文与中文结构必须同步增删改。
- **Why**：如果只声明“支持双语”而不约束结构同步，后续站点会再次出现一个语言有入口、另一个语言缺位的漂移问题。
- **Consequence**：任何主受众 section 的新增、删除、重命名，都需要同步更新另一语言导航，或者先在 policy 中明确例外。

## Validation Implications

- Task 1 的验证重点是文本契约而非页面渲染：变更包结构、设计文档章节完整性、spec delta 内容以及 `docs/README.md` 的 policy 一致性必须先成立。
- 最低验证应覆盖 `git diff --check`，以及对 `openspec/changes/2026-05-15-pages-whitepaper-overhaul` 和 `docs/README.md` 的占位标记扫描，确保没有遗留未完成占位符。
- 后续任务才需要继续承担 VitePress build、路由、主题与内容镜像的实现验证。

## Risks / Mitigations

- **风险：** Task 1 若把 changelog policy 写得过于绝对，会与现有 Pages release-notes 入口冲突。
  **Mitigation：** 明确根 changelog 是 canonical source，docs release-notes 页面只是 derived/synced entry pages，不再宣称为独立真相。

- **风险：** 双语“镜像”若未限定到 primary audience-facing sections，可能把所有深层技术页都变成强制双语负担。
  **Mitigation：** 只把镜像约束绑定到主阅读路径，并允许 policy 预先声明受控例外。

- **风险：** 继续使用非常规 change layout 会让后续 archive 和审查成本上升。
  **Mitigation：** 现在就对齐仓库中已有的 capability-based OpenSpec 结构。
