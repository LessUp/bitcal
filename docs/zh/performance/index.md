# 性能

<script setup>
import { performanceBaseline, tableRows } from '../../.vitepress/theme/data/performanceBaseline'
</script>

BitCal 把性能当成证据纪律，而不是营销背景音。本节现在直接读取 `benchmarks/results/retained/` 里的提交结果，因此公开表格和仓库中的保留证据会一起演进，而不是分开手工维护。

## 基线快照

<EvidenceStrip
  :items="[
    { label: '当前证据目标', value: `${performanceBaseline.backend} on x86-64`, tone: 'accent' },
    { label: '数字状态', value: 'Retained checkpoint' },
    { label: '发布统计量', value: 'median ns/op', note: '每一行都来自提交进仓库的 summary artifact，而不是手工挑选的一次跑分。' },
    { label: '当前提交基线', value: performanceBaseline.commit },
    { label: 'ARM 状态', value: 'ARM 数据先留空', note: '当前还没有保留下来的 ARM benchmark 路径。' },
    { label: '明确不承诺', value: '普遍性胜利', note: '提交到仓库的本地数字只是检查点，不是无条件产品承诺。' }
  ]"
/>

当前保留的 baseline 已经跟随当前 shipped vNext 公共表面：`bit_and`、`bit_or`、`bit_xor`、`bit_andnot`、`popcount`、`equals`、`is_zero`、`shift_left` 和 `shift_right`。

ARM 数据先留空，直到项目拥有同等级、可复现、可提交的 ARM benchmark 证据路径。

本页报告的是面向编译目标的 CPU SIMD 证据，不报告 CUDA、GPU、NPU、ARM SVE/SVE2 或国产 CPU SIMD 加速。

<PerformanceTable
  title="128 位保留操作"
  :caption="`当前 ${performanceBaseline.backend} retained baseline（${performanceBaseline.commit}）`"
  :rows="tableRows('128')"
  :highlightBest="true"
/>

<PerformanceTable
  title="192 位保留操作"
  caption="保留在主证据链里的 custom-width 检查点"
  :rows="tableRows('192')"
  :highlightBest="true"
/>

<PerformanceTable
  title="256 位保留操作"
  caption="当前 x86-64 主验证面上的代表性固定宽度检查点"
  :rows="tableRows('256')"
  :highlightBest="true"
/>

<PerformanceTable
  title="512 位保留操作"
  caption="更大位宽上的当前公开算法表现"
  :rows="tableRows('512')"
  :highlightBest="true"
/>

这些 retained baseline 真正表达的是：

- 当前结论比旧的手写表格更克制，也更诚实；
- 128 位和 192 位上的大多数公开操作仍明显落后于 `std::bitset`，尤其是变换类操作；
- 256 位与 512 位已经出现少量局部胜利、若干持平和若干失利并存的混合局面；
- `equals` 在保留位宽上基本处于持平，这说明证据链覆盖的是完整公共表面，而不是只挑最戏剧化的结果。

## 测量方法学

性能主张必须始终附着在可复现命令路径和解释规则上。

### 复现命令

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --target benchmark_compare -j"$(nproc)"
./build-test/benchmarks/benchmark_compare --json-out benchmarks/results/retained/baseline-x86_64-avx2.json
node benchmarks/scripts/generate-performance-summary.mjs \
  benchmarks/results/retained/baseline-x86_64-avx2.json \
  benchmarks/results/retained/baseline-x86_64-avx2.summary.json
```

### benchmark 二进制分工

本页的性能证据来自 `benchmark_compare`，而不是导读里那个更轻量的 `bitcal_benchmark` 烟雾可执行文件。

| 二进制 | 在文档体系中的角色 | 为什么要拆开 |
| --- | --- | --- |
| `benchmark_compare` | 发布当前 shipped vNext 公共算法的 retained baseline，并写出原始 JSON 结果。 | 主基线需要明确的对照程序、结构化结果和严格的宣称边界。 |
| `bitcal_benchmark` | 保留在 [验证路径](/zh/guide/verification) 中，作为烟雾级 benchmark 可执行文件。 | 验证链需要更轻量的执行检查，不应与对外发布的对比实验混成同一条链路。 |

### 方法规则

| 规则 | 设立原因 |
| --- | --- |
| 把当前数字视为保留的 **baseline checkpoint** | 防止一次本地 benchmark 输出被写成永久营销文案。 |
| 表格统一发布每个场景的 median | 相比单次最好值或临时均值，median 对偶发调度噪声更稳健。 |
| 始终带上活跃 backend、CPU 和 commit 语境 | 没有 ISA、机器和修订语境的速度提升没有意义。 |
| benchmark 叙事必须回到公开算法形状 | 性能数字应该能映射回文档中的公开算法，而不是匿名 kernel 小技巧。 |
| 让 retained evidence 只绑定当前 shipped 公共表面 | 对外发布的对照路径只应衡量今天真正交付的 API，而不是已经删掉的兼容层。 |

### 解释护栏

- synthetic loop 很有用，但它不代表所有真实工作负载。
- x86-64-first 是支持姿态，不是“其他平台同样成熟”的证据。
- AVX2 或 AVX-512 行描述的是二进制的编译目标，不是运行时 CPU feature dispatch。
- 某一类算法上的 benchmark 优势，不能自动外推成更宽泛的 API 或平台承诺。
- benchmark 失利同样是重要证据；保留基线的目标是诚实，而不是表演。

## 宣称边界

本节刻意拒绝说出超出证据范围的话。

**今天可以安全说的话**

- BitCal 仍保有一条可复现的 x86-64 benchmark 路径，并把 raw/summary artifacts 提交进了仓库；
- 当前 retained baseline 始终绑定在具名公开算法、具名位宽以及明确 backend/commit 语境之上；
- BitCal 目前并没有对 `std::bitset` 形成普遍性优势，而保留证据链会把这一点公开写出来。

**仍然不能说的话**

- 对标准库或专业 bitmap 实现具有普遍性优势；
- ARM64、macOS 或未来 x86 backend 已具备同等成熟度；
- retained core library 已具备 CUDA/GPU/NPU 加速；
- 没有真实负载或场景测量支撑的 workload-level 承诺。

## 性能工作接下来该往哪里走

继续扩展时，优先补方法学深度，而不是堆 headline：

- aligned 与 unaligned 对比；
- owner 与 borrowed view 工作负载差异；
- 如果未来引入外部 comparator，也要始终保持它们处于明确的 opt-in 研究层；
- 能与 synthetic retained baseline 互补的 workload traces。

如果你需要设计背景，请回到 [白皮书](/zh/whitepaper/)；如果你需要契约语言，请继续阅读 [参考](/zh/reference/)；如果你想看外部对照，请进入 [研究](/zh/research/)。
