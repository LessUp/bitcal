# 算法设计

算法层体现了 BitCal 最强的一条架构选择：行为应当围绕 block 和 view 上的操作来描述，而不是围绕一个包揽所有责任的中心对象。

## 责任流

```mermaid
flowchart LR
  A[bit_block] --> B[bit_view 或 const_bit_view]
  B --> C[自由算法]
  C --> D[detail dispatch]
  D --> E[x86-64 或 scalar kernel]
  E --> F[可观察结果]
```

## 为什么自由算法成为中心

### 1. 它让契约保持行为导向

读者会先问“这个算法保证什么结果”，而不是“哪个类拥有这段实现细节”。

### 2. 它让借用访问变成常态

只要算法天然能操作 view，借用型访问就不会再退化成附属能力。

### 3. 它把 backend 话题继续留在幕后

BitCal 仍然要保留实现自由，自由算法正好允许库在不改公共命名面的前提下持续演进 dispatch 与 kernel policy。

## 这会带来的设计压力

- 操作需要明确 ownership 与 aliasing 预期
- 文档必须先讲行为，再讲优化动机
- benchmark 页必须说明自己测的是哪一类算法形状

## 读者结论

只要理解 owner/view 拆分和算法表面，就已经抓住了 BitCal 的公共逻辑。剩下的部分属于实现自由，而不是公共身份。
