# BitCal vNext 技术白皮书

## 摘要

BitCal vNext 将库重设计为三个清晰层次：

1. `bit_block<Bits>`：拥有型存储
2. `bit_view` / `const_bit_view`：非拥有视图
3. `bit_and<Bits>()`、`and_into()` 等自由算法

`<bitcal/bitcal.hpp>` 仍然是唯一稳定公开入口，后端选择和 SIMD 分发则被收敛到内部实现层。

## 设计目标

- 用更现代的 C++23 方式表达位块与算法
- 保持 header-only 接入体验
- 优先把正确性与性能资源集中在 x86-64
- 不继续维护旧 `bitarray` API 的代码级兼容层
- 用可复现 benchmark 重建性能叙事

## 当前架构

```text
bitcal/bitcal.hpp
├── bit_block.hpp
├── bit_view.hpp
├── algorithms.hpp
└── detail/
    ├── backend.hpp
    └── x64_dispatch.hpp
```

## 当前内核姿态

- 标量回退路径始终存在
- `and_into()` 已引入 x86-64 / AVX2 快路径
- x86-64 下 owning block 当前保证 **32 字节对齐**

## 正确性与 benchmark 姿态

当前自动化覆盖已经包含：

- block/view 基础契约
- `bit_and`
- `and_into`
- `is_zero`
- `popcount`
- x86-64 对齐约束
- span 导入/导出互操作

性能页当前提供的是 **baseline**，不是最终宣传数字。它的作用是为后续内核优化保留一个可信比较点。

## 复现

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```
