# 平台支持

本页定义 BitCal vNext 当前保留的平台支持边界。

## 契约摘要

| 领域 | 状态 | 含义 |
| --- | --- | --- |
| 语言基线 | **必需** | 公开目标与开发目标统一假定 **C++23** |
| 交付模型 | **稳定** | BitCal 继续保持 **header-only** |
| 主要优化目标 | **主要** | x86-64，且以编译期目标/配置驱动的分发为中心 |
| 可移植性下限 | **保留** | 当未启用 SIMD 路径时，仍保留标量执行 |

## 设计层面的支持矩阵

| 目标 | 状态 | 当前承诺 |
| --- | --- | --- |
| Linux x86-64 | **主要验证路径** | GCC/Clang 构建、编译期后端选择、保留的正确性与基准验证路径 |
| Windows x86-64 | **主要验证目标** | MSVC CI 会以原生 build/test 路径验证同一公开模型；需要 AVX2 开发目标时使用 `/arch:AVX2` |
| macOS ARM64 | **次要验证路径** | Apple Clang CI 会在 Apple Silicon runner 上验证公开模型；这是保留的集成覆盖，不是 x86-64 优化中心 |
| Linux ARM64 | **保留的次级 build-only 路径** | 保留支持矩阵仅通过 ARM64 交叉编译构建覆盖来维持可见性；它不是保留的原生测试或 benchmark 可信路径 |
| 非 x86 目标 | **仅保留可移植下限** | 公开模型仍可能通过标量路径构建，但这些目标不定义 BitCal 的优化契约 |

## 后端边界在实践中的含义

公开模型不会把后端编码进存储类型本身。调用方只面对：

- `bit_block<Bits>`
- `bit_view` / `const_bit_view`
- 自由算法
- 可选诊断接口，例如 `backend_kind` 与 `default_backend()`

当前后端枚举为：

```cpp
enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512,
};
```

`default_backend()` 反映的是当前编译目标/配置所对应的后端标签。就今天而言，非 x86 构建会显示为 `scalar`；应把它理解成构建摘要，而不是每一次调用都必然精确映射到同名运行时内核路径的保证。

对于当前保留的写路径，真正的算法流是 `bit_and<Bits>()` / `and_into()` → `detail::and_words()` → `detail::x64_dispatch.hpp`，而是否编译进 x86-64 AVX2 循环由构建目标决定。`default_backend()` 更适合被理解成这套构建状态的诊断摘要。

## 编译器预期

| 工具链 | 推荐基线 |
| --- | --- |
| GCC / Clang / Apple Clang | `-std=c++23 -O3`，并按需要显式指定 `-march=native` 或 `-mavx2` |
| MSVC | `/std:c++23 /O2`，需要 AVX2 开发目标时再加 `/arch:AVX2` |
| CMake 使用者 | 链接 `bitcal` 接口库，并由使用者自己的构建系统决定 CPU 标志 |

仓库内的开发目标会通过 `BITCAL_NATIVE_ARCH` 为测试、示例和 benchmark 添加本机 SIMD 标志；`bitcal` 接口库本身**不会**把这些标志强加给下游项目。

## 刻意不纳入契约的内容

BitCal vNext 目前**不**承诺：

- 运行时 CPU 分发
- 所有 ISA 都有同等深度的优化
- 为每个后端分别暴露公开类型家族
- 脱离保留构建/基准路径单独宣称性能结论

## 验证路径

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --parallel
ctest --test-dir build-test --output-on-failure -C Release
```

然后从 `build-test` 构建树中运行生成的 `bitcal_benchmark` 可执行文件；如果使用 multi-config generator，请使用 `Release` 配置对应的输出位置。
