# BitCal vNext 性能基线

## 范围

本页记录的是当前 vNext benchmark 目标的**可复现基线**，不是最终性能宣传页。

它主要用于：

- 验证 benchmark 目标已迁移到新公开模型
- 为后续 AVX2 / AVX-512 / 标量对比提供同机基线
- 给 Pages 保留一个可引用、可复现的性能入口

## 当前样例结果

当前开发机上的一次样例输出：

| Benchmark | 结果 |
| --- | ---: |
| `bit_and<128>` | `9.3252 ns/op` |
| `bit_and<256>` | `1.4631 ns/op` |
| `bit_and<512>` | `3.2167 ns/op` |
| 默认后端 | `avx2` |

## 如何理解这些数字

- 这些数字是**本地 smoke baseline**
- 它们适合用于同环境下比较未来内核改动
- 目前还不适合作为跨平台或正式宣传指标
- 当前最重要的结论是：benchmark 已经能在 vNext 公共模型上稳定运行

## 复现命令

```bash
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=OFF -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-bench --target bitcal_benchmark -j"$(nproc)"
./build-bench/benchmarks/bitcal_benchmark
```
