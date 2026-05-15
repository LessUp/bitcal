# 构建选项

本页收集 BitCal vNext 作为 **header-only C++23** 库时最关键的构建开关。

## 面向使用者的默认行为

`bitcal` 这个 CMake 目标是一个 `INTERFACE` 库，有两个关键行为：

1. 暴露 BitCal 的 include 目录
2. 通过 `target_compile_features(... cxx_std_23)` 要求 **C++23**

它**不会**把 `-march=native`、`/arch:AVX2` 或其他 CPU 定向标志强制传递给下游项目。

## 仓库 CMake 选项

| 选项 | 默认值 | 作用 |
| --- | --- | --- |
| `BITCAL_BUILD_TESTS` | `ON` | 构建保留的 smoke test 可执行文件 |
| `BITCAL_BUILD_EXAMPLES` | `ON` | 构建仓库内示例程序 |
| `BITCAL_BUILD_BENCHMARKS` | `OFF` | 构建保留的 benchmark 基线 |
| `BITCAL_NATIVE_ARCH` | `ON` | 为仓库内测试/示例/benchmark 添加本机 CPU 标志 |
| `BITCAL_ENABLE_LTO` | `ON` | 在支持时启用链接时优化 |
| `BITCAL_ENABLE_HARDENING` | `OFF` | 在支持的工具链上按需增加 hardening 标志 |

## 常见构建配置

### 完整本机验证

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

### 更保守的可移植正确性验证

```bash
cmake -S . -B build-generic -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=OFF -DBITCAL_NATIVE_ARCH=OFF
cmake --build build-generic --config Release -j"$(nproc)"
ctest --test-dir build-generic --output-on-failure -C Release
```

### 安装/导出包构建

```bash
cmake -S . -B build-install -DCMAKE_BUILD_TYPE=Release
cmake --build build-install --target install
```

## 编译器标志建议

| 工具链 | 常用标志 |
| --- | --- |
| GCC / Clang | 本机验证常用 `-std=c++23 -O3 -march=native`；显式 AVX2 构建可用 `-std=c++23 -O3 -mavx2` |
| Apple Clang | `-std=c++23 -O3`，按需要补充明确架构目标 |
| MSVC | 验证面向 AVX2 的路径时使用 `/std:c++23 /O2 /arch:AVX2` |

## 有用的编译期诊断信息

记录构建或 benchmark 运行时，下列公开宏与辅助接口很有用：

- `BITCAL_ARCH_X86`
- `BITCAL_HAS_SSE2`
- `BITCAL_HAS_AVX2`
- `BITCAL_HAS_AVX512`
- `BITCAL_VERSION_MAJOR`、`BITCAL_VERSION_MINOR`、`BITCAL_VERSION_PATCH`
- `bitcal::default_backend()`

把它们视为诊断与验证辅助工具，而不是把应用代码拆成“后端特定公开类型”的理由。

## 经验法则

- 需要在单机上测量或验证时，用 **native** 标志
- 需要更保守的可移植验证时，关闭 **native** 标志
- 始终保持公开 include 入口稳定：`#include <bitcal/bitcal.hpp>`

## 下一步

- [安装](./installation.md)
- [快速开始](./quickstart.md)
- [SIMD 分发](../architecture/simd-dispatch.md)
