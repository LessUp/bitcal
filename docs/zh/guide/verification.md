# 验证路径

BitCal 虽然是 header-only，但依然需要一条可执行路径，把公开表面、示例和当前 benchmark baseline 串起来。

## 保留的命令路径

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## 每一步分别证明什么

| 步骤 | 验证内容 | 为什么重要 |
| --- | --- | --- |
| Configure | 仓库仍以 CMake 方式维持 header-only 包的当前构建姿态。 | `<bitcal/bitcal.hpp>` 的接入体验本身也是契约的一部分。 |
| Build | 测试、示例和 benchmark 目标还能一起通过编译。 | 防止文档先行、集成现实滞后。 |
| `ctest` | 当前保留的 correctness surface 仍通过现有测试二进制执行。 | 对外行为的主张不能只靠编译通过。 |
| benchmark 可执行文件 | 性能章节仍有可以被重跑的 baseline 路径。 | 没有复现路线的性能数字不构成证据。 |

## 如何解读失败

这条路径的意义，不是证明所有未来优化都已经完成，而是在重设计期间保留最低可信验证闭环。

- **构建失败** 说明文档姿态与仓库实现已经漂移。
- **测试失败** 说明可观察行为底线不稳定，Reference 需要被谨慎阅读。
- **benchmark 失败** 说明当前性能证据链断裂，即使 API 还在编译。

## 证据交接点

一旦命令路径跑通，建议按以下顺序继续：

1. [迁移姿态](/zh/guide/migration-posture) —— 先理解重设计意图；
2. [白皮书](/zh/whitepaper/index) —— 再进入系统架构主线；
3. [性能](/zh/performance/index) —— 最后把 baseline 与 methodology 放到一起看。

BitCal 把验证放在阅读开头，是为了防止后续所有叙事脱离可执行证据。
