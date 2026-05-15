---
layout: home
hero:
  name: BitCal vNext
  text: 头文件式 C++23 位库
  tagline: 正在围绕 block/view 分离、x86-64 优先验证与可观察的算法契约重建。
  actions:
    - theme: brand
      text: 阅读白皮书
      link: ./architecture/vnext-whitepaper
    - theme: alt
      text: 项目状态
      link: ./project-status/
    - theme: alt
      text: English
      link: ../en/
---

<script setup>
import { withBase } from 'vitepress'
</script>

## 从适合你的入口开始

<ReadingPathGrid
  :items="[
    {
      title: '面试官导读',
      href: './architecture/overview',
      badge: '快速总览',
      summary: '先看保留后的架构概览，快速建立 BitCal 过去与 vNext 方向之间的对照，再进入白皮书与项目状态页。'
    },
    {
      title: '高级开发者导读',
      href: './academy/overview',
      badge: '概念模型',
      summary: '从学院入口理解 block/view 分离、分发边界，以及性能结论如何保持为可观察的公开契约。'
    },
    {
      title: '使用者导读',
      href: './getting-started/installation',
      badge: '采用路径',
      summary: '如果你要落地使用，请直接进入安装入口，查看 include seam、迁移姿态与当前仍被维护的上手路径。'
    }
  ]"
/>

## 证据快照

<FigureFrame
  eyebrow="架构证据"
  title="首页现在直接对应 BitCal vNext 的分层论点。"
  caption="BitCal vNext 把 block 与 view 职责拆开，把算法保留在可观察的契约层，并把性能宣称绑定回 x86-64 优先的验证路径。"
>

<img :src="withBase('/figures/bitcal-vnext-layers.svg')" alt="BitCal vNext 分层示意图">

</FigureFrame>

## 为什么要有这个站点

这个站点是 BitCal vNext 的保留阅读层：它负责解释重设计论点、指出证据所在，并把读者路由到仍被维护的章节，而不是继续停留在混杂的新旧时代页面之间。

- [学院](./academy/overview) 负责建立概念模型。
- [白皮书](./architecture/vnext-whitepaper) 负责陈述架构论点与验证姿态。
- [指南](./getting-started/installation) 负责安装、快速开始与迁移路径。
- [参考](./api/overview) 负责定义公开表面与契约。
- [研究](./research/overview) 负责收口比较材料、引用与方法说明。
- [项目状态](./project-status/) 负责说明支持边界与维护现实。
