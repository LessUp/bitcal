# Verification Path

BitCal is a header-only library, but its credibility still depends on a retained verification path. The current local baseline is:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## Why it matters

The point is not to maximize automation count. The point is to keep a credible path that validates correctness, builds the public examples, and preserves a reproducible benchmark checkpoint.
