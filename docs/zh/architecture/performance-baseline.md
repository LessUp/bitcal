# 性能基线

本页定义 BitCal vNext 当前应该如何谈论性能。

## 基线的用途

当前保留的 benchmark 面向的是**可复现检查点**，不是永久有效的产品宣传结论。

它主要回答四个问题：

1. benchmark 目标是否仍能在 vNext 公开模型上构建？
2. 这次构建最终选择了哪个后端？
3. 同一台机器上的后续改动是提升了还是退化了？
4. 当前保留的工作负载是否被优化尝试真正改善？

## 当前保留的 benchmark 目标

仓库里的 benchmark 可执行文件目前聚焦于：

- `bit_and<128>`
- `bit_and<256>`
- `bit_and<512>`
- 同时打印 `default_backend()`，把它作为编译期目标/配置摘要的诊断标签

这些工作负载是刻意收窄的。它们验证当前 block / view / algorithm 叙事已经打通，同时为后续扩展留下空间。

真正被测的保留写路径仍然是 `bit_and<Bits>()` → `and_into()` → `detail::and_words()` → `detail::x64_dispatch.hpp`。`default_backend()` 只是打印在旁边的摘要信息。

## 如何复现这条基线

```bash
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=OFF -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-bench --target bitcal_benchmark -j"$(nproc)"
./build-bench/benchmarks/bitcal_benchmark
```

如果你需要完整开发验证路径，而不是只跑 benchmark，请使用：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## 如何解读一次结果

一条有价值的基线记录至少应包含：

- 编译器及版本
- CPU / 机器名称
- 生效的构建标志
- 打印出来的 `default_backend()`
- 各保留工作负载的 ns/op 输出

这样的记录只适合做**同环境对比**，不足以支撑脱离上下文的广泛宣传结论。

## 不应如何使用这些数字

不要把某次本地运行直接包装成以下结论：

- 每个后端都已同等优化
- 每个操作系统都有相同吞吐
- 每种工作负载都会以相同方式受益于一次内核修改

只有当性能证据始终附着在生成它的构建设置与硬件上下文上时，它才值得信任。

## 后续 benchmark 工作的推荐方向

当保留的内核表面继续扩大时，可以继续把 benchmark 扩展到：

- 同机的 scalar / AVX2 / AVX-512 对比
- 对齐敏感场景
- 避免 owning 分配的 view-oriented 工作负载
- 除合成循环之外的代表性应用轨迹

当前执行故事请看 [SIMD 分发](./simd-dispatch.md)，支持边界请看[平台支持](./platform-support.md)。
