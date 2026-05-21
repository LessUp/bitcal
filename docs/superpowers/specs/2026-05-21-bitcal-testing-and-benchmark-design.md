# BitCal 测试与 Benchmark 体系设计

## Summary

BitCal 当前已经有一条**可运行的最小验证路径**，但还没有形成一套能被称为“严肃完整”的 correctness 与 performance evidence 系统。

这份设计把目标定为：

1. 让测试从“vNext 冒烟检查”升级为**可分层、可扩展、可解释**的 correctness 框架。
2. 让 benchmark 从“本地打印几行 ns/op”升级为**可重复、可对比、可提交、可喂给白皮书**的数据管线。
3. 保持 BitCal 一贯的轻量仓库姿态，不把项目变成依赖沉重、CI 复杂、维护负担过高的性能工程平台。

推荐方案是：建立一个**双轨证据系统**——

- 一条是必须长期保留的 **retained baseline**，只覆盖公开契约、核心操作和 `std::bitset` 基准对比；
- 另一条是可选的 **research lanes**，用于额外比较第三方库、探索更复杂 workload、分析回归和瓶颈。

## Assumptions

因为用户当前离线，本文锁定以下假设，以便自治推进：

1. 项目的首要目标是**方法严肃**，不是追求排行榜式性能。
2. 默认必须保留的同类对比对象只有 `std::bitset`。
3. 第三方同类库比较应设计成**可插拔扩展层**，而不是主验证链的硬依赖。
4. benchmark 结果需要作为仓库资产提交，但**不能把共享 CI 机器上的噪声结果当成白皮书真相**。
5. 现有轻量测试风格、header-only 姿态、最小 GitHub Actions 原则都应继续保留。

## Problem Statement

当前仓库已经具备一个“最低可信回路”，但这套回路不足以支撑更强的工程结论：

1. **测试覆盖过浅**：目前更像 API smoke tests，尚未形成系统性的边界、差分、随机、回归、平台姿态验证。
2. **benchmark 方法学过弱**：目前只有简单循环计时，没有结构化结果、环境指纹、批次统计、数据留存和自动生成文档。
3. **性能文档缺乏单一事实源**：白皮书/性能页中的数字是手写内容，不是从仓库中保留的 raw results 生成。
4. **对比策略不分层**：当前只有 `std::bitset` 对比，且没有明确区分“保留对比对象”和“研究性对比对象”。
5. **证据与宣称耦合不严**：现在还不能清晰回答“这条性能结论对应哪份结果、哪台机器、哪次提交、哪组编译选项”。

## Goals

1. 为 vNext 公共契约建立**分层 correctness 模型**。
2. 为核心操作建立**可重复、可结构化导出的 benchmark harness**。
3. 把性能结果变成**可提交的仓库资产**，而不是终端临时输出。
4. 让 `docs/en/performance/index.md` 与 `docs/zh/performance/index.md` 从**数据源生成**，不再手写表格。
5. 明确“可安全宣称什么、不能宣称什么”的证据边界。
6. 在不引入沉重测试框架的前提下，把当前轻量宏测试风格提升到更严肃的工程水平。

## Non-Goals

1. 不把 BitCal 变成大型 benchmark 基础设施项目。
2. 不把所有第三方位图库都变成强依赖。
3. 不在共享 CI 上自动发布噪声较大的性能数字。
4. 不用 benchmark 结果直接驱动营销式“全面领先”叙事。
5. 不为了先进而引入与仓库体量不匹配的 fuzz infra、数据库或复杂服务。

## Current-State Findings

### Strengths

- 现有 CMake 路径已经能同时构建 tests、examples、benchmarks。
- 当前仓库已经保留了两个 benchmark 角色：
  - `bitcal_benchmark`：轻量 smoke benchmark
  - `benchmark_compare`：与 `std::bitset` 的对比 benchmark
- 当前 docs 已经有“性能是证据纪律”的叙事基础。

### Weaknesses

1. `tests/test_bitcal.cpp` 只有少量 public smoke cases；`tests/test_bitcal_detail.cpp` 只有少量 detail cases。
2. 当前 `ctest` 只跑两个测试可执行文件，逻辑覆盖仍然非常薄。
3. benchmark 仍使用简单定时循环，缺少：
   - 批次采样
   - 统计摘要
   - 结构化输出
   - 环境指纹
   - 结果 schema
   - 提交到仓库的 retained artifacts
4. `docs/en/performance/index.md` 与 `docs/zh/performance/index.md` 中的表格不是由 raw benchmark 结果自动生成，存在漂移风险。
5. 当前没有明确区分：
   - correctness 最低必保层
   - performance 最低必保层
   - 研究性扩展层

### Baseline reality check

本次设计前的本地基线显示：

- 构建、`ctest`、`bitcal_benchmark` 与 `benchmark_compare` 当前都可运行；
- 但测试总量和 benchmark 方法学仍不足以支撑“严肃完整”的结论；
- 当前性能页表格虽然有方法学说明，但其数据流并未真正闭环到仓库中的 retained result artifacts。

换句话说：**现在的仓库有“起点”，但还没有“体系”。**

## Approaches Considered

### Approach A — 在现有文件上继续加更多 case 和更多 benchmark 输出

沿用当前宏测试和简单 timer，只做增量扩充。

**Pros**

- 改动最少
- 上手最快
- 学习成本最低

**Cons**

- 还是没有证据链
- 结果无法稳定提交和回放
- 文档与 benchmark 仍容易漂移
- 长期会再次沦为“很多 case，但没有体系”

### Approach B — 构建轻量但严肃的“证据管线”体系（推荐）

保留当前轻量风格和 CMake 姿态，但引入明确分层：

- correctness layers
- benchmark harness
- retained results
- docs data source
- optional comparator adapters

**Pros**

- 最符合 BitCal 当前体量和仓库姿态
- 能真正支撑性能白皮书
- 不要求重型依赖或复杂平台
- 便于以后逐步扩展

**Cons**

- 需要一次性重构测试与 benchmark 组织方式
- 需要新增结果 schema、生成脚本和数据约束

### Approach C — 直接引入 Google Benchmark + 多第三方库 + 更重的性能平台

把 benchmark 主体完全建立在外部框架与更多 comparator 之上。

**Pros**

- 工具能力强
- 对研究性实验友好

**Cons**

- 增加依赖和维护成本
- 与“练手项目但要严肃”的目标不完全匹配
- 容易让主验证链被可选依赖绑架

### Recommendation

选择 **Approach B**。

它最适合 BitCal：既足够严肃，能支持白皮书和对比研究；又不会把仓库拖进过度复杂的基础设施里。

## Design Overview

目标架构分为五层：

```text
Public contract
└── Correctness layers
    ├── compile/contract coverage
    ├── deterministic behavior coverage
    ├── differential randomized coverage
    └── platform / build posture checks

Benchmark evidence
└── benchmark harness
    ├── scenarios
    ├── comparators
    ├── environment fingerprint
    ├── statistical summaries
    └── structured outputs

Retained assets
└── committed result artifacts
    ├── raw JSON
    ├── normalized summary JSON
    └── generated docs tables
```

关键原则：

1. **测试与 benchmark 分层，而不是混成一锅**
2. **原始结果保留，文档从结果生成**
3. **默认比较对象最少，但架构允许扩展**
4. **本地探索可以自由，白皮书证据必须克制**

## Correctness Framework

### Layer 1 — Public contract tests

这层验证 vNext 公共契约，是“必须长期保留”的 correctness floor。

覆盖对象：

- `<bitcal/bitcal.hpp>` 唯一稳定 include seam
- `bit_block<Bits>`
- `bit_view` / `const_bit_view`
- free algorithms
- `backend_kind`

覆盖轴：

1. **类型/构造**
   - 零初始化
   - `bits` / `word_count`
   - view 类型与 const/mutable 语义
2. **位运算正确性**
   - `bit_and` / `bit_or` / `bit_xor` / `bit_andnot`
3. **查询**
   - `is_zero`
   - `equals`
   - `popcount`
4. **位移**
   - 0
   - 1
   - word crossing
   - width-clearing
   - 全宽度边界
5. **宽度维度**
   - 128
   - 256
   - 512
   - 至少一个 custom width

### Layer 2 — Deterministic regression matrix

这一层不再只是几个手写 smoke case，而是把典型输入模式组织成矩阵：

- 全 0
- 全 1
- 单 bit
- 双 bit
- 边界 bit
- 交错位型
- 稀疏/稠密模式
- 跨 word 边界模式
- 高位清零与尾部 padding 敏感模式

这层的目标不是“随机”，而是让**每种典型失败模式都有固定样例**。

### Layer 3 — Differential randomized testing

这层是 serious correctness 的关键升级。

对每个算法，使用固定 seed 的随机输入，把 BitCal 输出与**reference model** 对齐比较。

参考模型策略：

- 对固定宽度主路径，优先使用 `std::array<std::uint64_t, N>` + 纯标量 reference functions
- 对可自然映射的场景，补充 `std::bitset<Bits>` 作为差分参照

原因：

1. `std::bitset` 并不是所有操作的理想 gold model；
2. 标量 reference model 更能表达公开语义，而不是依赖另一库的实现习惯；
3. `std::bitset` 可作为第二参考层，帮助发现语义偏差。

这层要覆盖：

- 多个固定 seeds
- 多个 bit density
- 多个 shift amount
- 多个 width
- 多轮批量随机样本

### Layer 4 — Build and posture validation

这层不是再加功能测试，而是确保仓库声明与验证路径一致。

包括：

- CMake clean configure/build
- `ctest`
- benchmark targets compile
- x86-64 primary posture 验证路径
- 文档中列出的命令实际可运行

### Layer 5 — Optional heavy lanes

这一层不是默认 gate，而是保留为研究性扩展：

- sanitizer builds
- more aggressive UB checks
- future fuzz-style probes
- optional large-width stress loops

对 BitCal 来说，这些可以存在，但**不能喧宾夺主**。

## Test Organization

### Recommended structure

为尊重当前仓库约束，主测试风格仍使用现有 lightweight macro style，但要增加组织能力。

建议结构：

```text
tests/
├── test_bitcal.cpp              # public contract + deterministic matrix main suite
├── test_bitcal_detail.cpp       # internal detail and implementation-aware checks
├── support/
│   ├── test_macros.hpp
│   ├── case_generators.hpp
│   ├── reference_model.hpp
│   ├── deterministic_cases.hpp
│   └── random_cases.hpp
```

原则：

- **public contract** 继续集中在 `tests/test_bitcal.cpp`
- 共享逻辑下沉到 `tests/support/`
- 不引入新的测试框架
- 让“案例生成”和“断言工具”成为复用资产

## Benchmark Framework

## Roles

benchmark 体系拆成三类角色：

1. **Smoke benchmark**
   - 继续由 `bitcal_benchmark` 承担
   - 目标：证明 benchmark target 能构建、运行、报告 backend
2. **Retained comparison benchmark**
   - 由 `benchmark_compare` 升级承接
   - 目标：产出可提交的 baseline data
3. **Research benchmark lanes**
   - 用于额外 comparator、不同 workload、探索性实验
   - 不进入最小验证回路

### Benchmark harness responsibilities

新的 benchmark harness 必须负责：

1. 场景注册
2. comparator 适配
3. 输入数据准备
4. 热身
5. 批次采样
6. 统计汇总
7. 环境元数据采集
8. 结构化输出

### Scenario model

每个 benchmark case 由以下维度定义：

- operation
- width
- data pattern / density
- alignment mode
- comparator
- backend
- compiler / flags profile

建议至少保留以下 operation families：

- `and`
- `or`
- `xor`
- `not`
- `andnot`
- `popcount`
- `is_zero`
- `shift_left`
- `shift_right`

后续可扩展到：

- owner vs view
- aligned vs unaligned
- in-place vs out-of-place
- hot-cache vs cold-ish replay

### Statistical model

为了保持轻量但严肃，推荐的统计输出是：

- `samples`
- `iterations_per_sample`
- `median_ns`
- `min_ns`
- `max_ns`
- `mean_ns`
- `stdev_ns`
- `cv`

理由：

- `median` 适合做白皮书主展示值；
- `cv` 可快速判断噪声是否过高；
- `min/max` 能帮助发现异常批次；
- 这些统计可以用标准库自行实现，不必强依赖外部 benchmark 框架。

### Environment fingerprint

每次 retained baseline 结果必须记录：

- commit SHA
- benchmark timestamp
- CPU model
- core/thread metadata
- OS
- compiler and version
- CMake build type
- native arch flags
- active backend

这一步非常关键：**没有环境指纹，就没有可信白皮书证据。**

## Comparator Strategy

### Tier 1 — Retained comparator

默认长期保留：

- `std::bitset`

原因：

- 零额外依赖
- 易理解
- 与当前 docs 叙事一致
- 适合作为“公开、保守、可长期维护”的对比对象

### Tier 2 — Optional research comparators

设计成可插拔 adapter，不进入主验证链：

- Boost `dynamic_bitset`
- CRoaring（只在适用 workload 下比较，不能强行对齐所有 fixed-width 语义）
- `libpopcnt`（仅用于 counting primitive 局部研究，不作为全栈对比对象）

### Comparator guardrails

必须明确：

1. 不同存储模型不能被包装成“绝对公平”的单一排行榜。
2. 第三方 comparator 只在**语义可对齐**的场景下比较。
3. 白皮书主页面只展示 retained baseline；研究性对比应进入 research/performance appendix。

## Result Artifacts

这是整套设计最重要的一点：**结果要成为代码库资产。**

### Canonical result layout

建议新增：

```text
benchmarks/
├── results/
│   ├── retained/
│   │   ├── baseline-x86_64-avx2.json
│   │   └── baseline-x86_64-avx2.summary.json
│   └── research/
│       └── *.json
```

其中：

- `retained/` 是白皮书和 docs 唯一可信性能数据源
- `research/` 是探索性实验，不自动成为公开宣称

### Result schema

`baseline-x86_64-avx2.json` 应包含：

- metadata
- toolchain
- environment
- scenario list
- per-scenario raw sample summaries
- comparator metrics

`baseline-x86_64-avx2.summary.json` 则是面向 docs 的标准化摘要层。

### Source of truth rule

性能 docs 不再手写数字。

应改为：

1. benchmark binary 写出 raw JSON
2. 一个仓库内脚本把 raw JSON 归一化为 summary JSON
3. `docs/en/performance/index.md` 与 `docs/zh/performance/index.md` 读取 summary 数据生成表格

这样可以保证：

- benchmark 结果与文档同步
- 结果可以被审查
- whitepaper 中的数字有来源

### CI policy

CI 不负责生成或发布 retained benchmark 数字。

CI 只负责验证：

- benchmark target 仍可编译
- result schema 合法
- summary generation 可重复
- docs 没有重新退回手写数字

真正用于白皮书的 retained baseline 应由明确命名的本地/专用机器 profile 产出后再提交。

## Whitepaper Evidence Policy

### Safe claims

有了新体系后，BitCal 可以安全地宣称：

- 公开 benchmark 路径可复现
- 结果可追溯到提交、机器、编译器和 backend
- 某些固定宽度场景在 retained baseline 中优于 `std::bitset`

### Unsafe claims

即使框架建好，也仍然不应宣称：

- 对所有位图库都更快
- 所有平台都成熟
- 任意 workload 都更强
- 某次本地跑分等于产品通用结论

### Publication rule

白皮书主页面只允许引用：

1. `benchmarks/results/retained/` 中的数据
2. 与当前文档同步的 summary 数据
3. 有明确环境指纹的 baseline run

## Documentation Integration

### Docs changes implied by this design

1. `docs/en/performance/index.md`
2. `docs/zh/performance/index.md`
3. `docs/en/guide/verification.md`
4. `docs/zh/guide/verification.md`
5. `docs/en/research/evolution-and-comparisons.md`
6. `docs/zh/research/evolution-and-comparisons.md`

### Documentation contract

文档必须清楚区分：

- verification path
- retained baseline
- research comparisons
- claim boundary

换句话说，**文档不再“展示数字”，而是“解释证据系统”。**

## Implementation Phases

### Phase 1 — Honest baseline and test refactor

1. 把现有测试宏和公共断言整理为共享 support helpers
2. 扩充 public contract deterministic matrix
3. 增加 reference model 与固定 seed 随机差分测试
4. 保持现有 `ctest` 命令路径不变

### Phase 2 — Benchmark harness upgrade

1. 为 benchmark 引入统一 harness
2. 给 `benchmark_compare` 增加 scenario registration、采样统计和 JSON 输出
3. 保留 `bitcal_benchmark` 作为 smoke executable

### Phase 3 — Result artifact and docs data pipeline

1. 新增 retained result artifact 目录
2. 新增 summary generation step
3. 让 docs 从 summary 数据读表格
4. 删除 performance docs 中的手写数字

### Phase 4 — Optional research lanes

1. 增加可选 comparator adapters
2. 增加更复杂 workload 与分析页
3. 保持它们不进入默认 gate

## Risks and Mitigations

### Risk 1 — 结果提交后很快过期

**Mitigation**

- 只提交明确命名的 retained baseline
- 强制记录环境元数据
- 让文档把它表述为 checkpoint，不表述为 timeless truth

### Risk 2 — benchmark 过于复杂，压垮小项目维护成本

**Mitigation**

- retained lane 只保留 `std::bitset`
- 研究性 comparator 全部 optional
- 不把重 benchmark 强行塞进 CI

### Risk 3 — 测试数量变多，但结构仍然混乱

**Mitigation**

- 把 case generators、reference model、deterministic matrix 抽成共享 support 层
- 用分层命名和分层职责组织测试

### Risk 4 — docs 又重新和 benchmark 漂移

**Mitigation**

- 结果 schema 成为单一事实源
- docs 直接消费 summary data
- 对手写数字建立内容契约校验，阻止回退

## Success Criteria

完成后，仓库应能回答下面这些问题，而且答案都在代码里：

1. **这个操作的正确性由哪些 deterministic cases 和 random differential cases 覆盖？**
2. **这张性能表是从哪次 benchmark 结果生成的？**
3. **这些数字在哪台机器、什么编译器、什么 backend 下得到？**
4. **哪些比较对象是保留基线，哪些只是研究扩展？**
5. **哪些性能结论可以安全写进白皮书，哪些不可以？**

如果这些问题都能被清楚回答，BitCal 就从“有一些测试和 benchmark”进入了“有一套严肃证据体系”的阶段。
