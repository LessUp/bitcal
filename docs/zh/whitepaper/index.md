# 白皮书

白皮书层的任务，是把 BitCal 的重设计论点讲到足够清楚，清楚到可以被挑战。它围绕一条系统架构主线展开，而不是围绕功能列表堆字数。

## 系统架构主线

```mermaid
flowchart LR
  A[稳定 include seam<br/><bitcal/bitcal.hpp>] --> B[公开角色模型<br/>bit_block · bit_view · const_bit_view]
  B --> C[自由算法层]
  C --> D[dispatch 边界]
  D --> E[x86-64 kernel + scalar floor]
  E --> F[baseline 证据与支持状态]
```

这条主线依次回答三个问题：公开模型是什么、算法如何组织、契约在何处停止，从而允许 dispatch 与 kernel 在其下自由演化。

## 公开模型

先读 [公开模型](/zh/whitepaper/public-model)。这页解释为什么 BitCal 要把 owner、view 与 algorithm 写成独立公共角色，而不是继续挂在一个中心对象上。

## 算法组织

接着阅读 [算法设计](/zh/whitepaper/algorithm-design)。这一页会说明为什么自由算法是行为中心、算法族如何组织，以及为什么必须先定义可观察语义，再讨论 backend。

## 分发与支持边界

最后阅读 [分发与内核](/zh/whitepaper/dispatch-and-kernels)。它说明哪些内容属于契约线以上，哪些仍是实现自由，以及 x86-64-first 支持姿态会如何约束公开叙事。

<ReadingPathGrid
  :items="[
    {
      title: 'Public Model',
      href: '/zh/whitepaper/public-model',
      badge: '角色模型',
      tone: 'whitepaper',
      summary: 'owner、view、algorithm 与稳定 include seam。',
      detail: '如果你想先搞清公开词汇表是什么，就从这里开始。'
    },
    {
      title: 'Algorithm Design',
      href: '/zh/whitepaper/algorithm-design',
      badge: '行为层',
      tone: 'whitepaper',
      summary: '解释自由算法如何组织契约表面。',
      detail: '这一页会把算法族、语义优先写法和 borrowing 为什么必须是一等公民讲清楚。'
    },
    {
      title: 'Dispatch and Kernels',
      href: '/zh/whitepaper/dispatch-and-kernels',
      badge: '边界层',
      tone: 'whitepaper',
      summary: '说明实现自由从哪里开始，支持边界从哪里收紧。',
      detail: '在阅读性能数据之前，必须先理解这条边界线。'
    },
    {
      title: 'Performance',
      href: '/zh/performance/',
      badge: '证据交接',
      summary: '离开论点层，进入 baseline 与 methodology。',
      detail: '白皮书刻意把 benchmark 证据交给性能章节处理，而不是塞回架构叙事里；ARM 数据先留空，直到存在保留下来的 benchmark 证据。'
    }
  ]"
/>
