# CLAUDE.md

BitCal 当前处于“终局收口”阶段。对本仓库工作的默认行为如下：

## 会话规则

- 与维护者交互统一使用中文
- 长会话串行推进，**不要使用 `/fleet`**
- 默认在 `.worktrees/` 隔离工作树中实施改动
- 对大改造优先做结构收口、规范收口、文档减法，再做美化或扩展

## OpenSpec 规则

- 先看 `openspec/changes/` 当前变更，再动实现
- `openspec/specs/` 是唯一活动规范源
- `specs/` 只是迁移壳层，禁止继续写入
- 一旦实现与设计发生冲突，先更新 OpenSpec，再继续实现

## 文档规则

- 不新增重复 Markdown/HTML 镜像
- `docs/` 的内容必须服务于最终 Pages 架构，而不是复制 README
- 双语范围要克制：核心入口可双语，深度参考按维护成本决定

## 工程规则

- BitCal 是 header-only 库；workflow、release、artifact 设计要围绕这一事实
- 保留最小可信 CI/CD，优先删除低价值自动化
- 任何平台/性能/API 声明都必须有相应验证路径

## AI 工具链规则

- `AGENTS.md` 是跨代理共享规则；本文件是 Claude 会话补充
- 仅保留真正高价值的 `.claude/` 入口
- MCP、CLI Skills、Copilot 插件都要先证明“减少上下文成本”，否则不引入

## 审查规则

- 合并价值较大的阶段性工作前，优先触发 `/review`
- 如果发现项目正在继续漂移，优先停下来收口，不要叠加新层
