# 导读

这不是给新手的“从零到入门”。导读层的任务，是让高级读者先判断 BitCal 这次重设计是否值得继续往下读。

## 读者与范围

本节默认服务以下问题：

- **C++23 baseline** 是否已经是明确项目前提；
- **x86-64-first** 姿态是否写得足够诚实，边界是否收紧；
- 从 `bitarray` 中心转向新的公开模型，是否真的让契约更清晰；
- `<bitcal/bitcal.hpp>` 是否仍被认真维护为稳定 include seam。

导读不会替代白皮书或 Reference。它负责把你送到正确的阅读轨道上。

## 阅读链路

标准阅读顺序是 **Guide → Whitepaper → Performance → Reference → Research → Status**。

<ReadingPathGrid
  :items="[
    {
      title: 'Verification path',
      href: '/zh/guide/verification',
      badge: '执行面',
      summary: '先看保留下来的构建、测试、示例与 benchmark 命令路径。',
      detail: 'header-only 也必须有可执行的本地验证闭环。'
    },
    {
      title: 'Migration posture',
      href: '/zh/guide/migration-posture',
      badge: '方向感',
      summary: '理解 BitCal 为什么愿意打破旧 bitarray 中心模型。',
      detail: '迁移不是靠模糊兼容话术，而是靠明确设计取舍来解释。'
    },
    {
      title: 'Whitepaper',
      href: '/zh/whitepaper/index',
      badge: '架构层',
      summary: '执行姿态明确之后，再进入系统架构主线。',
      detail: 'owner / view / algorithm 模型和 dispatch 边界会在这里被正式定义。'
    },
    {
      title: 'Performance',
      href: '/zh/performance/index',
      badge: '证据层',
      summary: '把 baseline 快照与 methodology 当作两件事情分开理解。',
      detail: '数字的意义取决于复现路径、目标 ISA 和解释护栏。'
    },
    {
      title: 'Reference',
      href: '/zh/reference/index',
      badge: '契约层',
      summary: '在架构论点清楚后，再阅读类型、视图与算法契约。',
      detail: 'Reference 读的是稳定角色，不是今天源码目录里有多少文件。'
    },
    {
      title: 'Research 与 Status',
      href: '/zh/research/index',
      badge: '上下文',
      summary: 'Research 提供外部对照，Status 重新收紧最终结论。',
      detail: 'Research 负责扩展判断背景，Status 负责收束现在时边界。'
    }
  ]"
/>

## 在继续之前应先建立的预期

- BitCal **不是**一个已经完全定型、只做小修小补的通用 bitset 库。
- 当前重设计是 **v4.0.0 级别** 的改造，明确允许 breaking change。
- Pages 的任务是帮助你评估架构和证据，不是放大“功能数很多”的印象。

如果你想先把文档锚定到可执行证据上，请继续看 [验证路径](/zh/guide/verification)；如果你已经熟悉仓库背景，可以直接进入 [白皮书](/zh/whitepaper/index)。
