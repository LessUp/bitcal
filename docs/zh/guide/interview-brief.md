# 面试介绍

这页是给维护者和面试场景准备的短版材料：用一页文档把 **项目定位、设计主线、当前已交付能力、benchmark 设计与结果边界** 说清楚。它不是新的 canonical whitepaper，也不是新的性能真相源；详细架构请回到 [白皮书](/zh/whitepaper/)，完整证据请回到 [性能](/zh/performance/)。

## 一句话介绍

**BitCal** 是一个 **C++23、header-only、x86-64-first** 的位运算库，目前处在 **vNext / 4.0.0 重设计** 阶段。它的核心变化不是“再加一些 bit API”，而是把旧的 `bitarray` 中心公开模型，收敛为 **`bit_block<Bits>` + `bit_view` / `const_bit_view` + free algorithms** 的新公共契约。

## 我的角色

如果把这页直接转成面试口径，可以这样概括：

- 我是 BitCal 的核心设计与实现者；
- 我主导了 vNext 的公开模型重构、x86-64-first 支持姿态收紧，以及 benchmark / 文档 / OpenSpec 的契约对齐；
- 我的重点不是“堆功能”，而是把 **公共 API、内存布局、dispatch 边界、性能证据链** 一起收口。

## 为什么要自研

BitCal 不是为了重复 `<bit>`。标准库 `<bit>` 的中心是 **标量值上的标准化位操作**；`std::bitset` 的中心是 **固定大小容器语义**。而 BitCal 想解决的是另一层问题：

- 位数据如何以对 SIMD 友好的方式组织；
- 如何把 owning storage、borrowed view 和 algorithm layer 明确拆开；
- 如何在不污染公共契约的前提下继续重写后端 dispatch 与 kernel。

换句话说，`<bit>` 更偏 **值语义工具集**，BitCal 更偏 **数据布局 + 批量算法执行模型**。

## 当前实际交付了什么

当前 shipped vNext 公共表面已经落到以下能力：

- `bit_block<Bits>`：固定宽度 owning storage；
- `bit_view` / `const_bit_view`：非 owning 的连续 `uint64_t` word 视图；
- 公开自由算法：`bit_and`、`bit_or`、`bit_xor`、`bit_andnot`、`equals`、`is_zero`、`popcount`、`shift_left`、`shift_right`；
- `<bitcal/bitcal.hpp>` 作为唯一稳定 include seam；
- x86-64-first 的后端姿态与 AVX2 主路径。

同时也要如实说明当前边界：

- **动态位集合** 还没有进入当前 shipped vNext 契约；
- **压缩位图 / 自适应压缩** 还没有作为公开功能交付；
- **序列化 / 反序列化** 没有成为当前公共表面；
- **SSE2 / AVX-512 / NEON** 有能力探测与底层预留，但公共算法主路径目前主要接入的是 **AVX2 + scalar fallback**。

这不是缺项列表，而是一个刻意收紧的工程策略：先稳定 block / view / algorithm 契约，再扩张压缩层、动态层和更多 backend。

## benchmark 是怎么设计的

当前公开性能页读取的是保留在仓库里的 retained baseline artifact；面试时可以把 benchmark 方法讲成下面这套纪律：

| 设计项 | 当前做法 |
| --- | --- |
| 对照对象 | `std::bitset` |
| 覆盖位宽 | 128 / 192 / 256 / 512 |
| 覆盖算法 | `bit_and`、`bit_or`、`bit_xor`、`bit_andnot`、`popcount`、`equals`、`is_zero`、`shift_left`、`shift_right` |
| 统计口径 | `median ns/op` |
| 执行参数 | warmup 100、samples 25、iterations/sample 5000 |
| 输出形式 | raw JSON + summary JSON 一起提交进仓库 |
| 上下文绑定 | 始终记录 backend、CPU、commit |

设计重点不是“挑一次最快结果”，而是把 benchmark 当作 **retained checkpoint**：

1. 只测当前 shipped 的公开算法，而不是已经废弃的兼容层；
2. 公开数字必须能映射回文档中的具体 API；
3. 结果必须带上 ISA、机器和 commit 语境；
4. 失利结果同样保留，不允许只展示漂亮数据。

## 当前 benchmark 结果该怎么说

当前保留下来的摘要文件来自：

- backend：**AVX2**
- CPU：**12th Gen Intel(R) Core(TM) i7-12700**
- profile：`retained-vnext-avx2`

面试里不建议背整张表，而是讲清这几个结论：

| 位宽 | 当前结论 | 可以引用的例子 |
| --- | --- | --- |
| 128 位 | 多数公开操作明显落后于 `std::bitset` | `bit_and<128>` 约 **10.59ns vs 0.53ns** |
| 192 位 | 仍以失利为主，但已经覆盖 custom-width 检查点 | `bit_xor<192>` 约 **10.03ns vs 1.52ns** |
| 256 位 | 进入持平与局部胜利并存阶段 | `bit_or<256>` 约 **1.01ns vs 1.01ns**，`is_zero<256>` 约 **0.95ns vs 0.96ns** |
| 512 位 | 大位宽上出现持平和混合结果 | `bit_and<512>` 约 **2.01ns vs 2.01ns**，`shift_right<512>` 约 **11.05ns vs 10.04ns** |

最重要的表述方式是：**BitCal 当前还没有对 `std::bitset` 形成普遍性优势。** 这不是问题，而是项目状态的真实描述。当前阶段更重要的是 **公共契约稳定、correctness 明确、dispatch 边界清楚、证据链可复现**。

## 面试里最值得强调的技术点

如果需要把项目讲到“有技术深度但不过度吹”，建议抓这几个点：

1. **公开模型重构**：把单体 `bitarray` 模型改成 owner / view / algorithm 三层；
2. **header-only 但不放弃边界治理**：稳定入口只保留 `<bitcal/bitcal.hpp>`；
3. **按位宽选择对齐策略**：为 128 / 256 / 512 位路径保留自然的 SIMD 对齐；
4. **后端自由留在 `detail/`**：不把 dispatch topology 变成兼容负担；
5. **性能证据纪律**：benchmark 不做 marketing table，而是做 retained baseline。

## 追问到“未来怎么扩展”时的安全说法

下面这些方向可以说成 **设计预留**，而不是“已经做完”：

- 把 **动态位集合** 作为 block / view 之上的上层容器；
- 把 **压缩位图** 做成独立层，优先研究 Roaring container 分层或 WAH/RLE 类编码；
- 继续把 **AVX-512 / NEON** 从底层预留推进到真正可发布的 dispatch 路径；
- 在 retained baseline 之外，增加 aligned / unaligned、owner / borrowed view 的工作负载分层。

如果你要继续看架构论点，请回到 [白皮书](/zh/whitepaper/)；如果你要核对完整 benchmark 证据、方法学和宣称边界，请直接进入 [性能](/zh/performance/)。
