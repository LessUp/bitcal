# BitCal Agent Operating Manual

本文件是 BitCal 仓库内 AI/自动化协作者的**执行手册**，不是项目宣传文档。

## 1. 项目定位

BitCal 是一个 C++17、header-only、SIMD 加速的位操作库。当前项目目标不是继续扩张功能面，而是把仓库收敛到**可归档、可交接、可稳定维护**的终局状态。

对本仓库的默认判断：

- 优先做减法，而不是叠加新层
- 允许必要的破坏性重构，但必须同步更新 OpenSpec、文档与版本语义
- 对外宣称的能力必须能被保留下来的测试、工作流或文档验证路径支撑

## 2. 仓库真相

### 2.1 规范真相

- **唯一活动规范源：** `openspec/`
- `openspec/specs/`：持久规范
- `openspec/changes/`：活动变更与归档
- `specs/`：仅保留迁移壳层，**禁止继续写入**

任何新需求、破坏性改动、流程调整、测试策略变化，都必须先落到 `openspec/`，再做实现。

### 2.2 文档真相

- `README.md` / `README.zh-CN.md`：项目入口与快速理解
- `docs/`：对外文档与 Pages 站点来源
- 不允许继续长期维护“同一内容的 Markdown + 手写 HTML 镜像”双轨
- 删除文档前先确定保留矩阵与双语策略

### 2.3 代码真相

- 核心库：`include/bitcal/*.hpp`
- 测试：`tests/test_bitcal.cpp`
- 示例：`examples/*.cpp`
- 基准：`benchmarks/*.cpp`
- 构建：`CMakeLists.txt` 与 `cmake/`

## 3. 必须遵循的工作流

### 3.1 OpenSpec-first

标准顺序：

1. 在 `openspec/changes/<change>/` 建立或更新 proposal/design/tasks/spec deltas
2. 读取变更上下文
3. 逐项实现 `tasks.md`
4. 完成后验证并归档

不要绕过 OpenSpec 直接“先改代码再补文档”。

### 3.2 单一活动工作树

- 项目内 worktree 目录固定为：`.worktrees/`
- `.worktrees/` 必须保持在 `.gitignore` 中
- 默认在隔离 worktree 上工作，不直接在 `master` 上做大改

### 3.3 轻量开发流

- 避免堆积多个长期未合并分支
- 避免为了“并行”引入大量上下文分裂
- 大改拆成小批次提交，每批次都能解释其范围与回滚边界
- 需要人工审查的阶段，优先通过 `/review` 收敛，而不是继续盲扩实现

## 4. BitCal 特有的实现约束

### 4.1 API 与行为

- `bitarray` 是核心抽象；所有公开契约变化都要同步更新 API 规范与用户文档
- 对外 API、平台支持、性能结论必须一致；文档不能超前于实现和验证
- 若发生 breaking change，必须同步写清迁移影响

### 4.2 测试

- 测试框架是 `tests/test_bitcal.cpp` 中的轻量宏框架，不引入新的测试框架
- 新增或修复行为时，优先扩充现有测试文件与现有断言模式
- 只有仍被项目保留为“受支持行为”的内容，才值得保留在自动化测试矩阵中

### 4.3 CMake 与发布

- BitCal 是 header-only；发布策略、artifact 设计、工作流矩阵都应体现这一点
- 版本号应有单一事实来源，不允许 README、changelog、spec、CMake 长期漂移
- 仅保留对项目稳定性有明确贡献的 CI/CD

## 5. 文档与 Pages 约束

- 首页负责价值主张、定位和导航，不负责承载完整 README 搬运
- API 参考、架构说明、快速开始、维护状态应分层清晰
- 中英双语范围必须明确，不做无边界镜像翻译
- 低价值 changelog 镜像、重复 API 页、手工同步页面应优先清理

## 6. AI 工具链约束

- `AGENTS.md`：跨代理共享的仓库执行规则
- `CLAUDE.md`：Claude/兼容代理的会话级执行偏好
- `copilot-instructions.md`：Copilot 项目级指令

这些文件都必须是 **BitCal 专用规则**，禁止套用泛化 boilerplate。

对 AI 资产的态度：

- `.claude/`：只保留高价值命令、skills 与项目工作流入口
- `_bmad/`：不应成为主流程依赖
- MCP / CLI Skills / 插件：按“是否真的减少上下文成本与重复劳动”来决定去留

### 6.1 MCP 与 CLI Skills 的边界

- **MCP / 外部集成**：仅用于 GitHub 元信息、Actions 状态、发布操作、外部资料检索等“需要最新外部状态”的任务
- **CLI Skills / 本地工作流**：用于 OpenSpec、计划、调试、TDD、审查和本地仓库推理
- 只要仓库内能完成的分析，就不要额外引入 MCP

### 6.2 Copilot Plugin 结论

- 当前不引入额外项目级 Copilot Plugin
- 项目级行为约束已经由 `AGENTS.md`、`CLAUDE.md`、`.github/copilot-instructions.md` 覆盖
- 任何新增插件都必须先证明能减少维护成本，而不是增加入口数量

## 7. 已验证的本地基线命令

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build --config Release -j"$(nproc)"
ctest --test-dir build --output-on-failure -C Release
```

## 8. 禁止事项

- 不要再向 `specs/` 写新规范
- 不要继续制造文档镜像
- 不要保留没有明确职责的 workflow
- 不要把 AI 控制文档写成泛泛而谈的模板
- 不要为 header-only 库引入与体量不匹配的复杂基础设施

## 9. 完成标准

当你完成一批工作时，检查这几件事：

- 是否让仓库更接近“单一真相”，而不是更分裂
- 是否减少了重复资产和维护负担
- 是否同步更新了 OpenSpec、文档、测试与版本语义
- 是否保留了清晰的回滚边界与交接上下文

如果答案不是“是”，说明这批改动还不够好。
