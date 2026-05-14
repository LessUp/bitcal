# BitCal vNext Performance Baseline

## Scope

This page records the **current reproducible baseline** for the vNext benchmark target.

It is not a final performance claim. It is a checkpoint for:

- benchmark build correctness
- public API compatibility with the benchmark target
- future scalar / AVX2 / AVX-512 comparisons

## Benchmark target

Current benchmark focus:

- `bit_and<128>`
- `bit_and<256>`
- `bit_and<512>`

Current backend printout:

- `default_backend()`

## Current sample baseline

Measured in the vNext worktree on the current development machine:

| Benchmark | Result |
| --- | ---: |
| `bit_and<128>` | `9.3252 ns/op` |
| `bit_and<256>` | `1.4631 ns/op` |
| `bit_and<512>` | `3.2167 ns/op` |
| Default backend | `avx2` |

## Interpretation

- These numbers are a **local smoke baseline**
- They are useful for comparing future kernel changes in the same environment
- They should not yet be presented as cross-platform product claims
- The important result today is that the benchmark target has been rebuilt on top of the new public model

## Methodology notes

Current methodology is intentionally simple:

- deterministic RNG seed
- warmup iterations before timing
- fixed iteration counts
- a dedicated benchmark executable

This should evolve toward a richer methodology that includes:

- backend-specific comparisons
- explicit alignment scenarios
- cold/warm path separation
- real workload traces
- hardware counters when available

## Reproduce

```bash
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=OFF -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-bench --target bitcal_benchmark -j"$(nproc)"
./build-bench/benchmarks/bitcal_benchmark
```
