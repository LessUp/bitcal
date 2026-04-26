## Why

BitCal 的核心库已经具备可用性，但项目外围资产长期漂移：OpenSpec 与 legacy specs 双轨并存、文档站与 README/HTML 镜像重复维护、CI/CD 对一个 header-only 库存在明显过度设计、AI 协作文档与工具链配置也未形成统一约束。继续在这种状态下叠加修补，只会放大维护成本并削弱项目可归档性。

这个变更用于把项目收敛到“最终完结状态”：允许必要的破坏性重构，一次性清理规范、文档、站点、工作流、AI 工具链与版本治理的历史包袱，让后续实现与归档都基于单一真相推进。

## What Changes

- 建立一个 OpenSpec 驱动的终局治理主变更，统一后续目录、规范、文档与工程化改造入口。
- **BREAKING**：结束 `openspec/specs/` 与 `specs/` 的双写状态，收敛到单一规范源。
- **BREAKING**：重构文档信息架构与 GitHub Pages 首页，删除低价值、过时和重复的文档资产。
- **BREAKING**：精简 GitHub Actions 工作流、重置 release 策略，并收敛版本与依赖锚点。
- **BREAKING**：重写项目级 AI 协作文档与工具链配置，明确 Copilot / Claude / OpenSpec / CLI Skills / MCP 的边界。
- 审查核心库、测试与示例，修复影响终局稳定性的 bug，并同步更新测试与规范。

## Capabilities

### New Capabilities
- `project-governance`: 规定 BitCal 的最终目录真相、规范优先级、归档标准与轻量开发流程。
- `project-documentation-site`: 规定对外文档、双语策略与 GitHub Pages 的最终信息架构和内容边界。
- `project-engineering-pipeline`: 规定 CI/CD、release、版本锚点、GitHub 仓库元信息与自动化边界。
- `project-ai-collaboration`: 规定 AGENTS/CLAUDE/Copilot 指令、LSP、CLI Skills 与 MCP 取舍。

### Modified Capabilities
- `bit-manipulation-library`: 调整产品层要求，使项目从“持续扩展型库”切换为“可归档的稳定终局型库”。
- `bitcal-public-api`: 若核心库在收敛过程中发生必要的 API/行为破坏，需要同步更新公开 API 要求。
- `bitcal-testing-spec`: 根据 bug 清理、目录收口与工作流精简，更新最终测试覆盖与验证要求。

## Impact

- 影响目录：`openspec/`、`specs/`、`docs/`、`.github/workflows/`、根站点文件、AI 控制文档。
- 影响代码：`include/bitcal/`、`tests/`、`examples/`、`benchmarks/`、`CMakeLists.txt`、版本与依赖配置。
- 影响外部系统：GitHub Pages、GitHub repository metadata、release artifacts、可能的 Codecov/MCP/插件配置。
- 相关现有规范：`openspec/specs/product/bit-manipulation-library.md`、`openspec/specs/api/bitcal-public-api.md`、`openspec/specs/testing/bitcal-testing-spec.md`。
