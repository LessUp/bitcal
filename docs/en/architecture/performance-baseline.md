# Performance Baseline

This page defines how BitCal vNext talks about performance today.

## What the baseline is for

The retained benchmark surface is a **reproducible checkpoint**, not a permanent product claim.

It exists to answer four questions:

1. does the benchmark target still build on the vNext public model?
2. which compile-target/config summary label did this build report?
3. how do current runs compare on the same machine after a change?
4. did an optimization attempt improve or regress the retained workloads?

## Current retained benchmark target

The repository benchmark executable currently focuses on:

- `bit_and<128>`
- `bit_and<256>`
- `bit_and<512>`
- when using the fallback non-GBench runner, printing `default_backend()` alongside the run as a diagnostic label for the compile-time target/config summary

Those workloads are intentionally narrow. They verify the current block/view/algorithm story and leave room for future benchmark expansion.

The actual retained write-path benchmark still runs through `bit_and<Bits>()` → `and_into()` → `detail::and_words()` → `detail::x64_dispatch.hpp`. In the fallback non-GBench path, `default_backend()` is only the summary label printed next to that run, not a promise that every step maps 1:1 to the named kernel.

## How to reproduce the baseline

```bash
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=OFF -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-bench --config Release --target bitcal_benchmark --parallel
```

Then run the generated `bitcal_benchmark` executable from the `build-bench` tree. For multi-config generators, use the `Release` output location.

If you want the full development validation path instead of a benchmark-only run, use:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --parallel
ctest --test-dir build-test --output-on-failure -C Release
```

Then run the generated `bitcal_benchmark` executable from the `build-test` tree. For multi-config generators, use the `Release` output location.

## How to interpret a result

A useful baseline record includes:

- compiler and version
- CPU / machine name
- active build flags
- if present, the printed `default_backend()` summary label
- ns/op output for each retained workload

That record is meaningful for **same-environment comparison**. It is not enough to support broad marketing claims across every platform.

## What not to do

Do not present a single local run as proof that:

- every backend is equally optimized
- every operating system has identical throughput
- every workload benefits the same way from a kernel change

Performance evidence only remains trustworthy when it stays attached to the build settings and hardware that produced it.

## Recommended next steps for future benchmark work

When the retained kernel surface grows, extend the benchmark story with:

- scalar vs AVX2 vs AVX-512 comparisons on the same host
- alignment-sensitive runs
- view-oriented workloads that avoid owning allocations
- representative application traces alongside synthetic loops

For the current execution story, read [SIMD Dispatch](./simd-dispatch.md). For support boundaries, read [Platform Support](./platform-support.md).
