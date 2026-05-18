# 分发与内核

BitCal 记录 dispatch，是为了定义支持边界，而不是为了给项目增加“很底层”的人格设定。读者需要知道实现自由从哪里开始，因为这条线决定了哪些变化属于架构，哪些只是 backend 工程。

## 分发边界

<FigureFrame title="公开契约与实现自由" caption="契约线把用户可见角色与语义留在上层，把 dispatch policy 与 kernel 替换留在下层。">
  <svg viewBox="0 0 760 330" role="img" aria-label="分发边界图">
    <rect x="40" y="28" width="680" height="64" rx="18" data-fill="accent" />
    <text x="70" y="68" fill="currentColor" font-size="26" font-weight="700">公开契约</text>
    <text x="280" y="68" fill="currentColor" font-size="16">稳定入口头文件 · 所有权/借用角色 · 算法语义</text>
    <path d="M60 132 L700 132" stroke-width="3" fill="none" data-stroke="primary" />
    <text x="70" y="122" fill="currentColor" font-size="14">分发边界</text>
    <rect x="40" y="164" width="210" height="96" rx="18" data-fill="surface" />
    <text x="70" y="202" fill="currentColor" font-size="22" font-weight="680">分发策略</text>
    <text x="70" y="230" fill="currentColor" font-size="14">特征检测、路由、回退选择</text>
    <rect x="276" y="164" width="210" height="96" rx="18" data-fill="surface" />
    <text x="308" y="202" fill="currentColor" font-size="22" font-weight="680">内核族</text>
    <text x="308" y="230" fill="currentColor" font-size="14">AVX2 主路径，标量可移植底线</text>
    <rect x="512" y="164" width="210" height="96" rx="18" data-fill="surface" />
    <text x="546" y="202" fill="currentColor" font-size="22" font-weight="680">微调层</text>
    <text x="546" y="230" fill="currentColor" font-size="14">指令选择、展开、对齐策略</text>
  </svg>
</FigureFrame>

## 内核族

| 内核族 | 当前文档姿态 | 为什么要单独分层 |
| --- | --- | --- |
| Scalar floor | 保留的可移植基线 | 即使没有宽向量路径，也要有 correctness floor。 |
| AVX2 path | 当前主优化与主证据目标 | 现阶段 benchmark 叙事主要集中在这里。 |
| Future x86 variants | 只能作为设计方向提及 | 在没有保留证据前，不应把它们写成现时主张。 |

## 支持边界

dispatch 与 kernel 必须服从当前公开支持姿态：

- **Linux / Windows x86-64** 是主要优化与验证目标；
- **次级目标** 可以继续保持可构建，但不能自动继承更强性能或成熟度承诺；
- **公开文档** 只有在 backend 细节会改变可见语义、benchmark 语境或支持边界时，才应主动讨论它。

## 读者应带走什么结论

只要公开角色模型、算法语义和 include seam 仍然一致，dispatch 层就可以被积极重写。这也是为什么下一站应该是 [Performance](/zh/performance/)：benchmark claim 属于这条边界线以下，而不是属于它以上。
