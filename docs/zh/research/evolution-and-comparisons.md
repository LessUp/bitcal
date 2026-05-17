# 演进与比较

当 BitCal 明确说出“自己是什么、又不是什么”时，它才更值得信任。

## 比较轴

| 维度 | 旧倾向 | vNext 倾向 |
| --- | --- | --- |
| 公共中心 | 单体类型 | owner、view、algorithm 拆分 |
| backend 叙事 | 公开 API 身份 | 实现边界 |
| 性能叙事 | feature 语言 | retained evidence 优先 |
| 支持姿态 | 宽泛兼容表述 | x86-64 优先的精确表达 |

## 相邻参考点

| 项目或资料 | 为什么重要 |
| --- | --- |
| Boost dynamic_bitset | 为 C++ 生态中的灵活 bitset 设计提供一个基线 |
| CRoaring | 展示 bitmap 工程可以如何严肃对待性能与 benchmark |
| libpopcnt | 说明哪怕一个局部位原语也值得高度工程化 |
| Intel Intrinsics Guide | 为 x86 实现讨论提供权威指令参考 |
| Agner Fog manuals | 为底层设计权衡补齐延迟与吞吐语境 |
| Vigna 的 broadword 论文 | 为 word-parallel bit 技术提供理论背景 |

<CitationList
  :items="[
    {
      title: 'Boost dynamic_bitset',
      href: 'https://www.boost.org/doc/libs/release/libs/dynamic_bitset/',
      note: '适合用于对比“定位宽公开模型”和“更灵活但目标不同的设计”。'
    },
    {
      title: 'CRoaring',
      href: 'https://github.com/RoaringBitmap/CRoaring',
      note: '是一个范围明确、性能导向很强的 bitmap 工程实例。'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      note: '说明位级基础设施完全值得被当成严肃工程问题看待。'
    },
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      note: '为 x86 SIMD 讨论提供权威底座。'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: '为性能解读补充微架构语境。'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      note: '是把 bit-level 推理与 word-parallel 实现连接起来的实用背景资料。'
    }
  ]"
/>
