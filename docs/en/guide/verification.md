# Verification Path

BitCal is header-only, but it still needs an executable path that proves the public surface, the examples, and the current benchmark baseline are all wired together.

## Retained command path

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## What each step proves

| Step | What it validates | Why it matters |
| --- | --- | --- |
| Configure | The repository still builds as a CMake-based header-only package with the current feature toggles. | Build posture is part of the contract around `<bitcal/bitcal.hpp>`. |
| Build | Tests, examples, and benchmark targets still compile together. | A clean compile path guards against documentation outrunning integration reality. |
| `ctest` | The retained correctness surface still runs through the existing test binary. | Claims about public behavior need more than type-checking. |
| Benchmark binary | The performance section retains an executable baseline path. | Performance numbers without a reproduction route are just decoration. |

## Benchmark binaries and responsibilities

BitCal intentionally keeps two benchmark executables, and this page only treats one of them as part of the minimum verification loop.

| Binary | Primary role | Read it here or elsewhere? |
| --- | --- | --- |
| `bitcal_benchmark` | Smoke-level benchmark executable that proves the benchmark target still builds, runs, and reports a backend path. | This page keeps it in the retained verification loop. |
| `benchmark_compare` | BitCal-vs-`std::bitset` comparison harness used to publish the baseline tables. | Use the [Performance](/en/performance/index) section for that evidence chain and its interpretation rules. |

## Failure interpretation

The purpose of this path is not to prove that every future optimization is already complete. It is to keep a minimum credible loop alive while the redesign evolves.

- A **build failure** means the documented public posture and the repository have drifted apart.
- A **test failure** means the observable behavior floor is unstable and the reference pages should be treated cautiously.
- A **benchmark failure** means the current performance evidence chain is broken, even if the API still compiles.

## Evidence hand-off

Once the command path succeeds, keep reading in this order:

1. [Migration Posture](/en/guide/migration-posture) for redesign intent.
2. [Whitepaper](/en/whitepaper/index) for the system architecture spine.
3. [Performance](/en/performance/index) for baseline and methodology.

This sequencing matters because BitCal treats verification as the first filter against overclaiming.
