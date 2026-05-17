# 公开模型

BitCal vNext 正在收敛成三个明确角色组成的公开模型：

| 角色 | 公开表面 | 作用 |
| --- | --- | --- |
| 拥有型存储 | `bit_block<Bits>` | 显式承载定位宽、可检查的存储 |
| 借用型访问 | `bit_view`、`const_bit_view` | 让算法能直接操作已有存储，而不是强迫拷贝 |
| 行为层 | 自由算法 | 让公共契约围绕可观察结果，而不是围绕一个 member-heavy 类型 |

## 为什么这层拆分重要

这不是审美选择，而是在保护四条容易被混淆的边界：

1. 存储与借用
2. 稳定用户契约与可替换后端细节
3. 算法语义与实现策略
4. 迁移成本与便利性怀旧

<FigureFrame title="owner、view、algorithm 拆分" caption="BitCal 希望存储、借用与行为都能被分别理解。">
  <svg viewBox="0 0 760 300" role="img" aria-label="owner、view、algorithm 拆分">
    <rect x="28" y="36" width="214" height="108" rx="18" data-fill="accent" />
    <text x="58" y="82" fill="currentColor" font-size="28" font-weight="700">bit_block</text>
    <text x="58" y="112" fill="currentColor" font-size="15">拥有定位宽字存储与对齐约束</text>

    <rect x="272" y="36" width="214" height="108" rx="18" data-fill="surface" />
    <text x="304" y="82" fill="currentColor" font-size="28" font-weight="700">bit_view</text>
    <text x="304" y="112" fill="currentColor" font-size="15">借用可变或只读存储</text>

    <rect x="516" y="36" width="214" height="108" rx="18" data-fill="surface" />
    <text x="548" y="82" fill="currentColor" font-size="28" font-weight="700">algorithms</text>
    <text x="548" y="112" fill="currentColor" font-size="15">表达公共行为与快路径</text>

    <path d="M242 90 L272 90" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M486 90 L516 90" stroke-width="2.5" fill="none" data-stroke="primary" />

    <rect x="112" y="188" width="536" height="70" rx="18" data-fill="surface" />
    <text x="146" y="228" fill="currentColor" font-size="24" font-weight="680">稳定 include seam</text>
    <text x="430" y="228" fill="currentColor" font-size="18">&lt;bitcal/bitcal.hpp&gt;</text>
  </svg>
</FigureFrame>

## 契约后果

- `<bitcal/bitcal.hpp>` 仍然是唯一稳定 include seam
- backend 选择不再是用户契约中心
- 用户应在不阅读内部头文件的情况下理解拥有与借用
- 文档应该先教角色，再列操作

## 为什么不继续维持一个中心大类

单体公共中心看起来方便，但会让未来每次改动都更贵：

- 行为和存储被迫一起演进
- 借用型接口始终变成附属
- 快路径讨论会泄漏进 API 语言
- 文档会退化成“大类型导览”，而不是契约叙事

BitCal 选择的是更锋利的边界，而不是延续这种旧形状。
