# 性能

BitCal 把性能当成证据纪律，而不是营销背景音。本节刻意把当前 baseline 快照与生成、解释这些数字的方法学拆开讲。

## 基线快照

<EvidenceStrip
  :items="[
    { label: '当前证据目标', value: 'AVX2 on x86-64', tone: 'accent' },
    { label: '数字状态', value: 'Baseline checkpoint' },
    { label: '当前稳定内容', value: 'Method + command path' },
    { label: '明确不承诺', value: '普遍性胜利', note: '本地数字不等于无条件产品承诺。' }
  ]"
/>

当前保留的 baseline 主要比较 BitCal 与 `std::bitset` 在若干固定宽度操作上的表现。它的价值，不在于制造“全面领先”的印象，而在于告诉读者：哪里已经出现明确信号，哪里仍然不成熟。

<PerformanceTable
  title="256 位操作"
  caption="当前活跃 AVX2 路径上的本地 baseline"
  :rows="[
    { operation: 'and<256>', bitcal: '1.34', stdBitset: '1.22', ratio: '0.91' },
    { operation: 'or<256>', bitcal: '1.02', stdBitset: '1.04', ratio: '1.02' },
    { operation: 'xor<256>', bitcal: '1.02', stdBitset: '1.02', ratio: '1.00' },
    { operation: 'popcount<256>', bitcal: '5.06', stdBitset: '1.90', ratio: '0.38' },
    { operation: 'shift_left<256>', bitcal: '1.68', stdBitset: '2.61', ratio: '1.56', highlight: true },
    { operation: 'shift_right<256>', bitcal: '1.27', stdBitset: '2.56', ratio: '2.01', highlight: true }
  ]"
  :highlightBest="true"
/>

<PerformanceTable
  title="512 位操作"
  caption="位宽增大后，部分 SIMD 优势会更明显"
  :rows="[
    { operation: 'and<512>', bitcal: '1.76', stdBitset: '4.27', ratio: '2.43', highlight: true },
    { operation: 'shift_right<512>', bitcal: '3.13', stdBitset: '11.78', ratio: '3.76', highlight: true }
  ]"
  :highlightBest="true"
/>

<PerformanceTable
  title="1024 位操作"
  caption="当前 benchmark 集中保留的一项更大位宽检查点"
  :rows="[
    { operation: 'popcount<1024>', bitcal: '8.10', stdBitset: '11.75', ratio: '1.45', highlight: true }
  ]"
  :highlightBest="true"
/>

这些 baseline 真正表达的是：

- shift 类路径已经在当前 x86-64 主路线上显示出实质性上行空间；
- 位宽会改变结论，因为部分优势只有在摊平 dispatch/setup 成本后才会明显；
- counting primitives 仍需继续审视，不能因为项目强调性能就被提前写成“已解决”。

## 测量方法学

性能主张必须始终附着在可复现命令路径和解释规则上。

### 复现命令

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --target benchmark_compare -j"$(nproc)"
./build-test/benchmarks/benchmark_compare
```

### benchmark 二进制分工

本页的性能证据来自 `benchmark_compare`，而不是导读里那个更轻量的 `bitcal_benchmark` 烟雾可执行文件。

| 二进制 | 在文档体系中的角色 | 为什么要拆开 |
| --- | --- | --- |
| `benchmark_compare` | 产出本页 BitCal 对 `std::bitset` 的对比表格。 | 基线发布需要明确的对照程序、方法学和解释护栏。 |
| `bitcal_benchmark` | 保留在 [验证路径](/zh/guide/verification) 中，作为烟雾级 benchmark 可执行文件。 | 验证链需要更轻量的执行检查，不应与对外发布的对比实验混成同一条链路。 |

### 方法规则

| 规则 | 设立原因 |
| --- | --- |
| 把当前数字视为保留的 **baseline checkpoint** | 防止一次本地 benchmark 输出被写成永久营销文案。 |
| 始终带上活跃 backend 与平台语境 | 没有 ISA / 平台语境的速度提升没有意义。 |
| benchmark 叙事必须回到公开算法形状 | 性能数字应该能映射回文档中的算法，而不是映射到匿名 kernel 小技巧。 |
| 把测量与解释分开 | 复现命令是证据，结论只是可以被质疑的论证。 |

### 解释护栏

- synthetic loop 很有用，但它不代表所有真实工作负载。
- x86-64-first 是支持姿态，不是“其他平台同样成熟”的证据。
- 某一类算法上的 benchmark 优势，不能自动外推成更宽泛的 API 或平台承诺。

## 宣称边界

本节刻意拒绝说出超出证据范围的话。

**今天可以安全说的话**

- BitCal 仍保有一条可复现的 x86-64 benchmark 路径；
- 当前本地 baseline 中，部分固定宽度操作已经优于 `std::bitset`；
- 性能讨论仍然绑定在具名算法、具名位宽与明确 backend 语境之上。

**仍然不能说的话**

- 对标准库或专业 bitmap 实现具有普遍性优势；
- ARM64、macOS 或未来 x86 backend 已具备同等成熟度；
- 没有真实负载或场景测量支撑的 workload-level 承诺。

## 性能工作接下来该往哪里走

继续扩展时，优先补方法学深度，而不是堆 headline：

- aligned 与 unaligned 对比；
- owner 与 borrowed view 工作负载差异；
- counting / scan 类原语更完整的覆盖；
- 能与 synthetic baseline 互补的 workload traces。

如果你需要设计背景，请回到 [白皮书](/zh/whitepaper/)；如果你需要契约语言，请继续阅读 [Reference](/zh/reference/)；如果你想看外部对照，请进入 [Research](/zh/research/)。
