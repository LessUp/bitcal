# BitCal OpenSpec 集成方案

## 概述

本文档描述如何将 BitCal 项目从现有的 `specs/` 目录结构迁移到 OpenSpec 框架管理。

## OpenSpec 核心概念

### 目录结构

OpenSpec 使用 `.openspec/` 目录存放所有规格驱动开发相关的文件：

```
.openspec/
├── config.yaml           # 项目配置
├── specs/                # 规格文档（持久化）
│   ├── product/          # 产品需求规格
│   ├── api/              # API 规格
│   ├── rfc/              # 技术设计文档
│   └── testing/          # 测试规格
├── changes/              # 进行中的变更提案
│   └── <change-name>/    # 每个变更一个目录
│       ├── proposal.md   # 变更原因与内容
│       ├── specs/        # 需求和场景（增量）
│       ├── design.md     # 技术方案
│       └── tasks.md      # 实现清单
└── changes/
    └── archive/          # 已完成的变更归档
        └── YYYY-MM-DD-<name>/
```

### 核心工作流

1. **Propose 阶段**: 使用 `/opsx:propose "<idea>"` 创建新变更提案
   - 创建变更目录
   - 生成 proposal.md（为什么做、改什么）
   - 生成 specs/（需求和场景）
   - 生成 design.md（技术方案）
   - 生成 tasks.md（实现清单）

2. **Apply 阶段**: 使用 `/opsx:apply` 执行任务实现
   - 读取 tasks.md 中的任务清单
   - 逐个实现任务
   - 更新任务状态

3. **Archive 阶段**: 使用 `/opsx:archive` 归档完成的变更
   - 将变更移动到 archive 目录
   - 更新持久化 specs

## 迁移策略

### 阶段 1: 初始化 OpenSpec

```bash
# 安装 OpenSpec CLI
npm install -g @fission-ai/openspec@latest

# 在项目根目录初始化
openspec init
```

这将创建 `.openspec/` 目录结构。

### 阶段 2: 迁移现有规格文件

| 原位置 | 新位置 | 说明 |
|--------|--------|------|
| `specs/product/` | `.openspec/specs/product/` | 产品需求 |
| `specs/api/` | `.openspec/specs/api/` | API 规格 |
| `specs/rfc/` | `.openspec/specs/rfc/` | 技术设计文档 |
| `specs/testing/` | `.openspec/specs/testing/` | 测试规格 |

### 阶段 3: 创建配置文件

创建 `.openspec/config.yaml`:

```yaml
schema: core
profile: core
delivery: both
workflows:
  - propose
  - apply
  - verify
  - archive
```

### 阶段 4: 更新项目配置

1. 更新 `AGENTS.md` 以反映 OpenSpec 工作流
2. 添加 Claude Code skill 文件（可选）
3. 保留原有 `specs/` 作为历史参考，或删除

## 文件格式规范

### proposal.md 格式

```markdown
# Proposal: <变更名称>

## Why

<!-- 变更原因：为什么需要这个变更 -->

## What

<!-- 变更内容：具体改变什么 -->

## Impact

<!-- 影响范围：影响哪些模块、API、用户 -->

## Related

<!-- 相关文档链接 -->
```

### design.md 格式

```markdown
# Design: <变更名称>

## Context

<!-- 技术背景 -->

## Decision

<!-- 技术决策 -->

## Implementation

<!-- 实现方案 -->

### Architecture

<!-- 架构图或描述 -->

### API Changes

<!-- API 变更详情 -->

### Database Changes

<!-- 数据库变更（如适用） -->

## Alternatives Considered

<!-- 考虑过的替代方案 -->

## Risks

<!-- 风险评估 -->
```

### tasks.md 格式

```markdown
# Tasks: <变更名称>

## Implementation Checklist

- [ ] 任务1: 描述
- [ ] 任务2: 描述
  - [ ] 子任务2.1
  - [ ] 子任务2.2
- [ ] 任务3: 描述

## Testing

- [ ] 单元测试
- [ ] 集成测试
- [ ] 性能测试

## Documentation

- [ ] API 文档更新
- [ ] 用户文档更新
```

### specs/ 增量规格格式

每个变更可以在 `specs/` 子目录中定义增量规格：

```markdown
# Spec: <规格名称>

## Overview

<!-- 规格概述 -->

## Requirements

### REQ-001: <需求标题>

<!-- 需求描述 -->

**Priority:** Must/Should/May
**Status:** Proposed/Accepted/Implemented

## Scenarios

### Scenario 1: <场景标题>

**Given:** 前置条件
**When:** 触发动作
**Then:** 预期结果
```

## 与 Claude Code 集成

### Skill 文件

创建 `.claude/skills/openspec.md`:

```markdown
# OpenSpec Skill

## Usage

Use this skill for spec-driven development workflows.

### Commands

- `/opsx:propose "<idea>"` - 创建新变更提案
- `/opsx:apply` - 执行任务实现
- `/opsx:verify` - 验证实现完整性
- `/opsx:archive` - 归档完成的变更

### Workflow

1. **Propose**: 先创建提案，定义 why/what/how
2. **Apply**: 按任务清单实现
3. **Verify**: 验证实现符合规格
4. **Archive**: 归档并更新持久化规格
```

## 旧规格迁移计划

### 已有规格文件处理

1. `specs/product/bit-manipulation-library.md` → `.openspec/specs/product/bit-manipulation-library.md`
2. `specs/api/bitcal-public-api.md` → `.openspec/specs/api/bitcal-public-api.md`
3. `specs/rfc/0001-core-architecture.md` → `.openspec/specs/rfc/0001-core-architecture.md`
4. `specs/rfc/0002-simd-optimizations.md` → `.openspec/specs/rfc/0002-simd-optimizations.md`
5. `specs/testing/bitcal-testing-spec.md` → `.openspec/specs/testing/bitcal-testing-spec.md`

### 历史变更归档

对于已完成的功能（如 RFC 0001, 0002），可以创建归档条目：

```
.openspec/changes/archive/
├── 2024-04-16-core-architecture/
│   ├── proposal.md
│   ├── design.md
│   └── tasks.md
└── 2024-04-17-simd-optimizations/
    ├── proposal.md
    ├── design.md
    └── tasks.md
```

## 工作流示例

### 示例：添加新功能

1. 创建提案：
   ```
   /opsx:propose "添加 AVX-512 原生支持"
   ```

2. OpenSpec 自动创建：
   ```
   .openspec/changes/add-avx512-support/
   ├── proposal.md   # 为什么需要 AVX-512，改变什么
   ├── specs/
   │   └── avx512-requirements.md  # AVX-512 相关需求
   ├── design.md     # AVX-512 实现方案
   └── tasks.md      # 实现清单
   ```

3. 实现变更：
   ```
   /opsx:apply
   ```

4. 验证实现：
   ```
   /opsx:verify
   ```

5. 归档：
   ```
   /opsx:archive
   ```

## 优势

1. **结构化变更管理**: 每个变更都有完整的提案、设计、任务记录
2. **可追溯性**: 归档系统保留所有决策历史
3. **增量规格**: 变更可以定义增量需求，归档后合并到主规格
4. **工作流一致性**: 标准化的提案→实现→验证→归档流程
5. **AI 集成**: 原生支持 AI Agent 工作流

## 注意事项

1. Node.js 版本要求: >= 20.19.0
2. 迁移后原 `specs/` 目录可以保留作为历史参考
3. 建议团队成员都安装 OpenSpec CLI
4. CI/CD 可能需要调整以支持新目录结构
