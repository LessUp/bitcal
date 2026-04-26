## Context

BitCal 的核心实现集中在 `include/bitcal/`，功能面相对稳定，但项目外围已经出现结构性漂移：`openspec/specs/` 与 `specs/` 双轨并存；`docs/` 同时维护 Markdown 与预生成 HTML，且中英文大面积镜像；GitHub Actions 为 header-only 库维持了多条长期常驻工作流；AI 协作文档和工具资产分散在 `AGENTS.md`、`.claude/`、`_bmad/` 与仓库根目录。这个变更是一次跨目录、跨文档、跨自动化、跨协作面的收口，而不是单点修复。

### Repository audit matrix

| Asset class | Current evidence | Final disposition |
| --- | --- | --- |
| `openspec/specs/` | 现有 authoritative specs 仍在使用 | **Keep**，并继续作为唯一规范源 |
| legacy `specs/` | 5 个文件，与 `openspec/specs/` 当前无差异 | **Migrate**：删除重复内容，仅保留迁移壳层 README |
| `docs/` | 68 个文件，`en/` 与 `zh/` 下 Markdown/HTML/changelog 大面积镜像 | **Migrate/Remove**：保留高价值 canonical docs，移除重复镜像 |
| 根目录文档/站点文件 | README、CHANGELOG、`index.html`、`404.html` 并存 | **Keep/Migrate**：README 与核心说明保留，站点入口重做并与 docs 真相对齐 |
| `.github/workflows/` | 6 条 workflow，存在职责重叠 | **Migrate/Remove**：保留必要流程，删除或合并冗余流程 |
| `.claude/` | 262 个 tracked files，已形成大型 AI 资产树 | **Migrate/Remove**：保留高价值入口，裁剪模板与重复能力 |
| `_bmad/` | 已忽略且未被跟踪 | **Remove from active flow**：不纳入最终主流程依赖 |
| `build/`, `build_arm64/`, `.worktrees/` | 均非 tracked 产物目录 | **Keep ignored**：继续视为本地/临时目录，不进入版本控制 |

## Goals / Non-Goals

**Goals:**
- 建立单一规范源、单一目录真相、单一版本源和单一项目治理入口。
- 将文档站、GitHub Pages、README、changelog、workflow、release 和 AI 协作规则统一到一致的终局策略。
- 允许必要的破坏性重构，但要求每一类破坏都具备规范、迁移说明和验证路径。
- 为后续执行与 GLM handoff 产出稳定、可追踪、可归档的任务边界。

**Non-Goals:**
- 不在本变更中引入新的业务能力或新的 SIMD 后端。
- 不为了“现代化”而引入与项目体量不匹配的新平台、新基础设施或重型服务。
- 不保留已经被证实只会增加维护成本的双份文档、双份规范或多余自动化。

## Decisions

### 1. 以 OpenSpec 主变更作为唯一治理入口
- **Decision**：所有终局治理动作都挂在 `openspec/changes/finalize-bitcal/` 下推进，并以这里的 proposal/design/specs/tasks 作为执行依据。
- **Why**：项目已经声明采用 OpenSpec，但长期没有活跃 change；本次收尾必须先恢复变更治理，否则后续删改没有统一约束。
- **Alternative considered**：直接在仓库里边改边删，不建立主变更。该方案会继续放大漂移，因此拒绝。

### 2. 用“单一真相 + 派生产物”替代镜像式维护
- **Decision**：后续文档与站点只允许存在一个主维护源；任何 HTML、索引、站点资产都应从主源派生，而不是与 Markdown 双份人工维护。
- **Why**：当前 `docs/` 的 md/html 双轨和中英镜像导致维护成本指数上升，且质量无法同步。
- **Alternative considered**：保留现状，只做局部删减。该方案无法阻止重复文档再次生成，因此拒绝。

### 3. 将 CI/CD 收敛为最小可信组合
- **Decision**：保留真正能证明库稳定性的工作流；对高成本、低收益或重复职责的流程执行合并、路径过滤或手动触发化。
- **Why**：当前工作流数量和触发面明显高于项目体量，既浪费 CI 资源，也增加误报噪音。
- **Alternative considered**：继续维持六条工作流，只调小矩阵。该方案仍保留职责重叠，收益有限。

### 4. AI 工具链采用“角色文档 + 最少接入点”策略
- **Decision**：`AGENTS.md`、`CLAUDE.md`、`copilot-instructions.md` 只保留对 BitCal 高价值的行为与约束；对 `.claude/`、`_bmad/`、CLI Skills、MCP 做去冗余定责。
- **Why**：现有 AI 资产数量已经足以产生认知噪音；终局状态需要的是少量强约束，而不是大量模板入口。
- **Alternative considered**：继续堆叠更多插件或多套说明文档。该方案只会扩大上下文负担。

### 5. 变更采用“先规范/文档，后代码/自动化，再 AI/交接”的顺序
- **Decision**：执行顺序固定为：规范与目录真相 → 文档与站点 → 代码与测试 → workflow/release/GitHub → AI 工具链与 handoff。
- **Why**：如果先改代码或先改 AI 文档，容易在目标结构尚未确定前反复返工。
- **Alternative considered**：并行推进全部 Phase。由于强依赖多、用户明确要求避免 `/fleet`，因此拒绝。

## Risks / Trade-offs

- **[结构兼容风险]** 现有 persistent specs 仍是旧式单文件，而 change delta specs 期望能力目录 → **Mitigation**：本变更内先按 schema 生成能力级 delta specs，并把 persistent specs 的收口纳入 Phase 1 明确迁移动作。
- **[破坏性变更风险]** 文档、API、目录与发布策略可能发生一次性变化 → **Mitigation**：所有破坏点都要求同步更新规范、版本与迁移说明。
- **[外部权限风险]** GitHub Pages、repo metadata、secrets 或 third-party 服务需要仓库权限 → **Mitigation**：将这类动作保留在主计划中，但执行前先验证 `gh auth` 和仓库配置能力。
- **[减法过猛风险]** 文档/工作流删除过快可能丢失有效信息 → **Mitigation**：先建立保留矩阵，再执行删除，并保留迁移说明。

## Migration Plan

1. 创建主变更并完成 proposal/design/specs/tasks。
2. 先收口规范与 legacy specs，再做 `docs/` 保留矩阵与站点信息架构重建。
3. 在文档真相确定后，修复核心库和测试中的终局稳定性问题。
4. 再重构 GitHub workflows、release、repo metadata 与 Pages 发布路径。
5. 最后重写 AI 协作控制面，并生成 GLM 交接包与 archive readiness gate。

回滚策略：若某一子阶段失败，优先回滚该子阶段对应提交与变更文件，不回滚已确认的上游规范收口结果。

## Open Questions

- legacy `specs/` 采用迁移壳层策略：删除重复规范文件，仅保留指向 `openspec/specs/` 的说明入口。
- 文档双语策略采用“核心入口双语 + 深度参考按需提供中文”，不再维护全量镜像。
- release 是否只保留 header/source 级 artifact，还是需要极少量示例/测试打包作为附属产物。
