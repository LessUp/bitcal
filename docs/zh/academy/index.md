# BitCal 学院

学院模块提供从零到一的学习路径，帮助你建立理解 BitCal 所需的心智模型。

## 学习路径

<ReadingPathGrid
  :items="[
    {
      title: '为什么选择 BitCal',
      href: '/zh/academy/why-bitcal',
      summary: '理解 BitCal 在 C++ 位操作生态中的定位与价值。',
      detail: 'BitCal 选择了标准库忽略的中间地带：固定位宽 + SIMD + 语义丰富。'
    },
    {
      title: '位运算心智模型',
      href: '/zh/academy/bit-mental-model',
      summary: '建立正确的位运算和内存布局直觉。',
      detail: '理解字、对齐、小端序和三角色模型是使用 BitCal 的基础。'
    },
    {
      title: 'SIMD 入门',
      href: '/zh/academy/simd-primer',
      summary: '了解 SIMD 优化的原理和 BitCal 的分发策略。',
      detail: '从 SSE2 到 AVX2，理解编译时分发如何实现零开销抽象。'
    },
    {
      title: '术语表',
      href: '/zh/academy/terminology',
      summary: '中英对照术语定义，确保理解一致。',
      detail: '从核心类型到 SIMD 术语，所有关键概念的定义。'
    }
  ]"
/>

## 前置知识

阅读学院内容需要以下基础：

| 知识领域 | 最低要求 | 推荐水平 |
|---------|---------|---------|
| C++ | 了解模板和 constexpr | 熟悉 C++17/23 特性 |
| 位运算 | 理解 AND/OR/XOR | 理解位级优化 |
| 计算机体系结构 | 了解内存对齐 | 理解 SIMD 和缓存行 |

---

> 学完基础后，进入 [白皮书](../whitepaper/index.md) 深入了解 BitCal 的设计架构。
