# 验证路径

BitCal 虽然是 header-only 库，但它的可信度仍然依赖一条保留下来的验证路径。当前本地基线为：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## 为什么重要

这里追求的不是自动化数量，而是保留一条能同时覆盖正确性、示例构建和 benchmark checkpoint 的可信路径。
