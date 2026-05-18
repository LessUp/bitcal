# 参考

Reference 的任务，是在白皮书已经说明设计逻辑之后，用契约语言描述公开表面。它不是源码目录的镜像。

## 契约地图

<ReadingPathGrid
  :items="[
    {
      title: '类型与视图',
      href: '/zh/reference/types-and-views',
      badge: '角色层',
      summary: '先理解 owner 与 borrowing 模型，再读算法细节。',
      detail: '这页会定义每个公共类型负责什么，以及明确不负责什么。'
    },
    {
      title: '算法契约',
      href: '/zh/reference/algorithms',
      badge: '行为层',
      summary: '确认算法应如何被命名、解释和约束。',
      detail: 'Reference 关注可观察语义、操作数与非目标，不把优化技巧当契约。'
    },
    {
      title: '研究',
      href: '/zh/research/index',
      badge: '对照层',
      summary: '需要把 BitCal 放到外部语境中时，再去研究相关系统。',
      detail: 'Research 负责扩大比较背景；Reference 只定义 BitCal 自己的契约语言。'
    }
  ]"
/>

## Reference 在这里冻结什么

在逐页细读之前，请先把下面三个公开锚点固定下来：

- 稳定 include seam 是 `<bitcal/bitcal.hpp>`；
- owner / view / algorithm 拆分先定义公共角色，再决定语法外形；
- 这里关注的是算法表面而不是内部 kernel 布局，因为可观察契约比实现细枝末节更重要。

## 如何正确阅读 Reference

阅读本节时，请始终记住三条规则：

1. **先角色，后语法** —— 先分清 owner、borrower 与 algorithm 的职责，再看函数外形。
2. **先语义，后实现** —— 只有当 dispatch 或 kernel 会改变可见行为或证据范围时，它们才应进入 Reference。
3. **先契约，后便利性** —— 页面描述的是项目愿意维护的内容，而不是实现阶段顺手长出来的所有 helper。

如果你还需要架构理由，请回到 [白皮书](/zh/whitepaper/index)；如果你想理解这些契约选择背后的外部对照，请继续阅读 [Research](/zh/research/index)。
