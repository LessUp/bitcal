# 研究

Research 为 BitCal 提供真正有工程价值的上下文层：它收录外部参考、相关系统与演进思考，目的是帮助工程判断，而不是营造装饰性的学术气氛。

## 研究地图

本节主要回答三类问题：

- 哪些论文、手册与参考资料支撑了算法与性能叙事；
- 哪些相邻库或系统，是阅读 BitCal 时最值得放在一起比较的对象；
- 当 BitCal 选择 C++23、x86-64-first 与 owner/view/algorithm 模型时，具体是在接受哪些取舍。

## 参考资料集

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      meta: '指令参考',
      note: '讨论 x86 SIMD 能力与指令限制时，应该优先回到这里。'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: '优化手册',
      note: '用于理解 dispatch 成本、延迟、吞吐与 x86-64 上的性能解释。'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'Broadword 论文',
      note: '理解 word-parallel 位运算技术路线时，这是非常关键的背景材料。'
    }
  ]"
/>

## 相关系统

更具体的对照请继续阅读 [演进与比较](/zh/research/evolution-and-comparisons)。当前最有价值的三个参考点是：

- **Boost dynamic_bitset**：C++ 生态中动态 bitset 设计的常见基线；
- **CRoaring**：以不同存储模型切入，但高度重视 bitmap 性能工程；
- **libpopcnt**：展示单个位级原语也值得被做到多么深入。

## 这个部分在阅读链中的位置

一般建议在读完 [参考](/zh/reference/) 之后再进入研究，因为只有先搞清 BitCal 自己的契约，外部比较才有意义。最后再回到 [状态](/zh/status/)，把视野重新收束到仓库今天真正支持的范围。
