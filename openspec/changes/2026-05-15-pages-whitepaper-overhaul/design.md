## Context

本变更需要把活动 OpenSpec、`docs/README.md` 与实际 VitePress 站点收敛到同一套主站模型。当前漂移主要集中在：主导航仍残留 retired academy naming、状态 section 仍混用 `project-status`、性能路径仍挂在旧白皮书命名下，以及多个入口页继续把读者指向旧 architecture routes。

## Goals / Non-Goals

**Goals**
- 把本次 Pages overhaul 的主站 contract 冻结为 `Home / Docs Landing / Guide / Whitepaper / Performance / Reference / Research / Status`。
- 让 OpenSpec、`docs/README.md`、VitePress nav/sidebar 与关键入口页都围绕同一 IA 工作。
- 删除 compatibility-only 页面与旧路由，避免它们继续成为维护噪音或事实竞争源。

**Non-Goals**
- 本设计不展开视觉系统重做或整站内容重写。
- 本设计不把深层技术正文从 Markdown 迁移到别的 canonical source。
- 本设计不把文档站点迁移到新的 SSG 或新的发布平台。

## Decisions

### 1. 主站 contract 统一为六个 audience-facing sections
- **Decision**：英文与中文主导航、侧边栏、入口页统一围绕 `Guide / Whitepaper / Performance / Reference / Research / Status`，并由 Home / Docs Landing 承担语言分流与总入口。
- **Why**：这是当前 `docs/README.md` 已经声明的最终 IA，也是本轮 Pages 收口需要对齐的唯一主站模型。
- **Consequence**：任何仍依赖 retired academy naming、旧 architecture 路径或 `project-status` 命名的主路径都必须被替换或降级为兼容 alias。

### 2. Performance 必须成为独立 section
- **Decision**：性能内容以 `/performance/` 作为主路径，不再让主导航和主阅读路径继续依赖旧白皮书路径。
- **Why**：性能证据、方法学和宣称边界需要独立入口，才能与 Whitepaper 的设计论证、Reference 的契约说明形成清晰分层。
- **Consequence**：现有性能内容可以保留 Markdown canonical source，但最终 IA、sidebar 和入口页必须把 `/performance/` 视为主阅读路径。

### 3. Status 是唯一主路径命名
- **Decision**：主站统一使用 `Status` / `状态`，旧的 `project-status` 仅保留为兼容 alias。
- **Why**：同一 section 不应继续在 policy、config、入口页和 legacy alias 中混用两种主路径命名。
- **Consequence**：OpenSpec、`docs/README.md`、VitePress config 和关键页面都要改写为 `Status` 命名；如需保留旧链接，只能通过兼容 alias 或跳转页承接。

### 4. 兼容 alias 不再保留
- **Decision**：兼容 alias 页、旧 route 页面与 docs 内 changelog 镜像不再保留，直接从维护树中删除。
- **Why**：只要这些页面继续存在，维护者就仍需为“哪一层才是真相”付出额外判断成本。
- **Consequence**：结构校验必须显式检查这些旧路径不存在，而不是继续维护一套 alias bookkeeping。

### 5. 结构校验成为本轮收口的最低自动化保护
- **Decision**：新增一个可执行的 IA 结构校验，直接验证 section 命名、关键路径、禁用旧一级结构，以及 OpenSpec / docs policy 的关键一致性。
- **Why**：当前构建可以通过，但无法证明 IA 没有漂移；必须增加针对结构的自动化约束。
- **Consequence**：后续任何 IA 变更都需要先更新校验，再更新实现。

## Validation Implications

- 必须先让 IA 结构校验在旧实现上失败，再开始修改生产文件。
- 完成修改后，IA 结构校验、`cd docs && npm run build` 与 `git diff --check` 都必须通过。
- 兼容 alias 是否已经删除，不靠口头约定判断，而靠结构校验确保这些旧路径彻底不存在。

## Risks / Mitigations

- **风险：** 只改 config，不改入口页和 policy，会留下“看起来对齐、实际仍漂移”的假一致性。
  **Mitigation：** 本任务同时约束 OpenSpec、`docs/README.md`、config 和关键入口页。

- **风险：** 删除旧路由后，历史外链会失效。
  **Mitigation：** 当前阶段优先减少维护噪音；如后续确实需要恢复跳转，应以单独变更显式引入。

- **风险：** 性能 section 仍借用旧路径，导致 IA 无法自证收口。
  **Mitigation：** 为 `/performance/` 建立明确入口，并把主导航、首页 CTA 与 changelog 链接统一改到该路径。
