# 性能

性能章节的价值，在于让项目的主张更小，但更可信。

<EvidenceStrip
  :items="[
    { label: '当前状态', value: '可复现 baseline' },
    { label: '活跃后端', value: 'AVX2' },
    { label: '优化中心', value: 'x86-64 优先' }
  ]"
/>

## BitCal vs std::bitset 性能对比

以下数据来自本地 benchmark 测试，使用 AVX2 后端。

<PerformanceTable
  title="256 位操作"
  caption="BitCal 使用 AVX2 后端，std::bitset 使用标准库实现"
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
  caption="更大的位宽下，BitCal 的 SIMD 优势更加明显"
  :rows="[
    { operation: 'and<512>', bitcal: '1.76', stdBitset: '4.27', ratio: '2.43', highlight: true },
    { operation: 'shift_right<512>', bitcal: '3.13', stdBitset: '11.78', ratio: '3.76', highlight: true }
  ]"
  :highlightBest="true"
/>

<PerformanceTable
  title="1024 位操作"
  :rows="[
    { operation: 'popcount<1024>', bitcal: '8.10', stdBitset: '11.75', ratio: '1.45', highlight: true }
  ]"
  :highlightBest="true"
/>

## 关键发现

1. **移位操作优势明显**：BitCal 在 `shift_right<512>` 上快 **3.76x**
2. **大位宽优势更大**：512 位操作比 256 位有更显著的加速
3. **popcount 需要优化**：std::bitset 在 popcount 上更快（可能使用了更好的内置函数）

## 测量方法

### 复现命令

```bash
cmake -B build -DBITCAL_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --target benchmark_compare
./build/benchmarks/benchmark_compare
```

### 测量规则

| 规则 | 作用 |
| --- | --- |
| 把当前结果视为 baseline checkpoint | 避免把 smoke 数字包装成成品承诺 |
| 呈现数字时同步说明活跃 backend | 没有目标上下文的性能数字毫无意义 |
| 保留可复现的本地命令路径 | 读者需要一条持续可挑战的验证路径 |

### 解释护栏

1. 本地测量只是本地证据
2. x86-64 优先优化，不等于所有目标同样成熟
3. 小型 synthetic loop 有用，但不能代表全部工作负载

## 仍待成熟的部分

- 对齐与非对齐对比
- owner 与 view 工作负载差异
- 更完整的 kernel family 覆盖
- 与 synthetic loop 互补的真实工作负载轨迹

<CitationList
  :items="[
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: '评估指令级权衡，以及为什么不同微架构会讲出不同性能故事时很有帮助。'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      note: '提醒我们位级微基准同样值得严肃工程化，而不是随手展示。'
    }
  ]"
/>
