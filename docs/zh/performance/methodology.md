# 方法学

Benchmark 的可信度首先来自测量设计。

## 当前测量规则

| 规则 | 作用 |
| --- | --- |
| 把当前结果视为 baseline checkpoint | 避免把 smoke 数字包装成成品承诺 |
| 呈现数字时同步说明活跃 backend | 没有目标上下文的性能数字毫无意义 |
| 保留可复现的本地命令路径 | 读者需要一条持续可挑战的验证路径 |

## 解释护栏

1. 本地测量只是本地证据
2. x86-64 优先优化，不等于所有目标同样成熟
3. 小型 synthetic loop 有用，但不能代表全部工作负载

## 仍待成熟的部分

- 对齐与非对齐对比
- owner 与 view 工作负载差异
- 更完整的 kernel family 覆盖
- 与 synthetic loop 互补的真实工作负载轨迹

<CitationList
  :items="[
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: '评估指令级权衡，以及为什么不同微架构会讲出不同性能故事时很有帮助。'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      note: '提醒我们位级微基准同样值得严肃工程化，而不是随手展示。'
    }
  ]"
/>
