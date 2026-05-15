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

### 2. Task 1 先冻结 Pages contract，后续任务只在该 contract 内实现
- **Decision**：Task 1 不仅冻结信息架构分层、双语结构镜像、canonical source/changelog policy，也明确冻结本次 overhaul 的站点实现基座：继续使用现有 VitePress 渲染管线，并继续以 GitHub Pages 作为发布目标；后续任务只负责在这个基座上实现导航、主题组件和页面迁移。
- **Why**：Task 1 的作用是先建立 contract，避免后续导航和内容重写在没有单一真相的情况下继续漂移，也避免把“是否换 SSG / 是否换部署平台”重新打开成隐性范围。
- **Consequence**：本变更后续任务可以调整 VitePress 配置、主题和内容树，但不能把迁移到其他文档框架或其他托管平台视为 Task 1 留下的待定事项。

### 3. Theme-component layer 是本变更的既定实现方向
- **Decision**：后续站点实现必须增加一个小型、可复用的 VitePress theme-component layer，用于承载共享的 hero、callout、figure chrome 等展示骨架；这是一项已锁定的设计决定，不是可选增强，也不是要演化为重型自定义前端应用。
- **Why**：proposal 与 tasks 已经把导航重组、页面重写和图示治理绑定在同一轮 whitepaper overhaul 中；如果不在 Task 1 明确 theme-component layer，后续实现容易退回到零散 Markdown 特例或失控地扩展为新的站点框架层。
- **Consequence**：后续任务需要在 VitePress theme 范围内落地薄而稳定的共享组件层，并以它作为白皮书页面与图示呈现复用的唯一承载面。

### 4. 图示策略固定为 theme-aware figures / SVG
- **Decision**：本变更明确采用 theme-aware figure strategy，并以 SVG 作为白皮书图示的首选格式；图示必须通过主题 token 或样式机制适配 light/dark，而不是默认依赖位图截图、手工维护两套不联动插图，或把配色兼容留作后续再议。
- **Why**：proposal 已把“theme-aware figure and SVG policy for light/dark readability”列为本次 overhaul 的组成部分；若 Task 1 不先锁定该方向，后续页面重写会重新引入不可维护的图示分叉。
- **Consequence**：后续任务中的 figure 重建、组件封装和页面迁移都必须围绕 theme-aware SVG 路线执行，并把 light/dark 可读性视为默认验收条件。

### 5. 根 changelog 保持 canonical，Pages release-notes 仅作 derived entry pages
- **Decision**：`CHANGELOG.md` / `CHANGELOG.zh-CN.md` 继续作为唯一 canonical changelog source；`docs/*/release-notes/changelog.md` 若保留，只能作为面向 Pages 导航的派生入口页。
- **Why**：这同时满足仓库“单一真相”原则与 Pages 站点对 release-notes 导航的需要，避免手工双轨维护。
- **Consequence**：`docs/README.md` 需要明确区分 canonical source 与 Pages-derived entry pages，清理规则也应只删除手工镜像或失去同步依据的版本。

### 6. 深层技术内容继续以仓库 Markdown 作为 canonical source
- **Decision**：Whitepaper、guides、reference、research 等深层技术内容继续以仓库内 Markdown 文件作为唯一 canonical source；VitePress theme、导航和最终 Pages HTML 只负责渲染、组织与呈现，不引入独立的 HTML/组件副本作为另一套长期维护真相。
- **Why**：Task 1 的目标是先冻结文档系统 contract，而仓库文档规则已经要求深内容留在 canonical repository Markdown 中；如果这里不把它写成显式设计决定，后续白皮书改造仍可能在主题组件、定制页面或手工页面里重新制造第二份技术内容真相。
- **Consequence**：后续任务可以重组导航、重写入口页、增加展示组件和图示包装，但深层技术正文必须仍由仓库 Markdown 承载，`docs/README.md` 也需要把“deep technical content stays canonical in Markdown”写成明确 policy。

### 7. 双语镜像要求绑定主受众导航结构
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
