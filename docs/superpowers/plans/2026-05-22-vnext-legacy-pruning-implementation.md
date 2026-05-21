# VNext Legacy Pruning Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remove the old `bitarray`-centered public surface, promote the missing vNext free-algorithm contract into the real implementation, and collapse legacy benchmark/example drift so BitCal clearly centers on `bit_block` / `bit_view` / free algorithms only.

**Architecture:** This change treats the current wobble as one problem: the repository already documents a vNext owner/view/algorithm model, but the code still ships legacy `bitarray` and a legacy benchmark lane. The implementation should first tighten OpenSpec and public tests, then fill the missing free-algorithm surface on the vNext path, then delete the obsolete legacy entry points and update docs/benchmark evidence to match the narrower public contract.

**Tech Stack:** C++23 header-only library, CMake, existing lightweight macro tests in `tests/test_bitcal.cpp`, retained benchmark harness in `benchmarks/`, OpenSpec, VitePress docs

---

## File map

- **Create:** `openspec/changes/2026-05-22-prune-legacy-vnext-surface/proposal.md`
- **Create:** `openspec/changes/2026-05-22-prune-legacy-vnext-surface/design.md`
- **Create:** `openspec/changes/2026-05-22-prune-legacy-vnext-surface/tasks.md`
- **Create:** `openspec/changes/2026-05-22-prune-legacy-vnext-surface/specs/bitcal-public-api/spec.md`
- **Create:** `openspec/changes/2026-05-22-prune-legacy-vnext-surface/specs/bitcal-testing-spec/spec.md`
- **Modify:** `include/bitcal/bitcal.hpp`
- **Modify:** `include/bitcal/algorithms.hpp`
- **Modify:** `include/bitcal/detail/word_ops.hpp`
- **Modify:** `include/bitcal/detail/backend.hpp`
- **Modify:** `include/bitcal/detail/x64_dispatch.hpp`
- **Delete:** `include/bitcal/bitarray.hpp`
- **Modify:** `tests/test_bitcal.cpp`
- **Modify:** `tests/test_bitcal_detail.cpp`
- **Modify:** `benchmarks/CMakeLists.txt`
- **Modify:** `benchmarks/benchmark_compare.cpp`
- **Delete:** `benchmarks/benchmark_compare_legacy.cpp`
- **Modify:** `benchmarks/scripts/generate-performance-summary.mjs`
- **Modify:** `benchmarks/results/retained/baseline-x86_64-avx2.json`
- **Modify:** `benchmarks/results/retained/baseline-x86_64-avx2.summary.json`
- **Modify:** `docs/.vitepress/theme/data/performanceBaseline.ts`
- **Modify:** `docs/en/performance/index.md`
- **Modify:** `docs/zh/performance/index.md`
- **Modify:** `docs/en/research/evolution-and-comparisons.md`
- **Modify:** `docs/zh/research/evolution-and-comparisons.md`
- **Modify:** `docs/en/status/index.md`
- **Modify:** `docs/zh/status/index.md`
- **Modify:** `README.md`
- **Modify:** `README.zh-CN.md`
- **Modify:** `examples/CMakeLists.txt`
- **Delete:** `examples/performance_comparison.cpp`

### Task 1: Lock the aggressive contract in OpenSpec first

**Files:**
- Create: `openspec/changes/2026-05-22-prune-legacy-vnext-surface/proposal.md`
- Create: `openspec/changes/2026-05-22-prune-legacy-vnext-surface/design.md`
- Create: `openspec/changes/2026-05-22-prune-legacy-vnext-surface/tasks.md`
- Create: `openspec/changes/2026-05-22-prune-legacy-vnext-surface/specs/bitcal-public-api/spec.md`
- Create: `openspec/changes/2026-05-22-prune-legacy-vnext-surface/specs/bitcal-testing-spec/spec.md`
- Modify: `openspec/specs/api/bitcal-public-api.md`
- Modify: `openspec/specs/testing/bitcal-testing-spec.md`

- [ ] **Step 1: Write the change proposal**

```md
## Why

BitCal already documents a vNext public model centered on `bit_block`, `bit_view`, and free algorithms, but the shipped umbrella header still exports `bitarray` and the benchmark tree still builds a legacy `bitarray` lane. This keeps two incompatible architectural centers alive at once.

## What Changes

- **BREAKING**: remove `bitarray.hpp` from the stable umbrella include and delete the legacy `bitarray` public type.
- **BREAKING**: delete the `benchmark_compare_legacy` executable and stop describing a retained legacy benchmark lane.
- Promote the missing vNext free algorithms (`bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, `shift_right`) from specification-only surface to real implementation and test coverage.
```

- [ ] **Step 2: Write the spec delta for API removal and new required free algorithms**

```md
### ADDED Requirements

### Requirement: The umbrella include SHALL export only the vNext public model
BitCal SHALL keep `<bitcal/bitcal.hpp>` as the only stable include seam and SHALL expose only `bit_block`, `bit_view` / `const_bit_view`, `backend_kind`, and the vNext free algorithms through that seam.

#### Scenario: Maintainers review public symbols shipped by the umbrella header
- **WHEN** `<bitcal/bitcal.hpp>` is updated
- **THEN** it MUST NOT export `bitarray`
- **AND** the shipped public behavior MUST center on free algorithms rather than a monolithic legacy type

### Requirement: The vNext free algorithm surface SHALL be implemented as the retained contract
BitCal SHALL retain `bit_and`, `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `is_zero`, `popcount`, `shift_left`, and `shift_right` as the supported public algorithm surface for the current redesign stage.
```

- [ ] **Step 3: Write the testing delta that forbids legacy completion criteria**

```md
### ADDED Requirements

### Requirement: Legacy `bitarray` coverage SHALL not remain in the redesign path
BitCal SHALL delete or rewrite tests and benchmarks that still treat `bitarray` as a supported public surface.

#### Scenario: Maintainers review redesign validation assets
- **WHEN** a test, example, or benchmark still requires `bitarray`
- **THEN** it MUST be deleted or rewritten against `bit_block` / `bit_view` / free algorithms
- **AND** it MUST NOT remain part of the retained verification path
```

- [ ] **Step 4: Validate the OpenSpec surface is locally coherent**

Run:

```bash
git --no-pager diff -- openspec
```

Expected: only the new change directory and the touched persistent specs describe the same no-compatibility direction.

- [ ] **Step 5: Commit the spec-first change**

```bash
git add openspec
git commit -m "spec: prune legacy vNext surface"
```

### Task 2: Use TDD to backfill the missing vNext free algorithms

**Files:**
- Modify: `tests/test_bitcal.cpp`
- Modify: `tests/test_bitcal_detail.cpp`
- Modify: `include/bitcal/algorithms.hpp`
- Modify: `include/bitcal/detail/word_ops.hpp`
- Modify: `include/bitcal/detail/backend.hpp`
- Modify: `include/bitcal/detail/x64_dispatch.hpp`

- [ ] **Step 1: Write failing public tests for the missing free algorithms**

```cpp
bool test_vnext_bit_or_matches_word_patterns_256() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;
    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    lhs_view.data()[0] = 0x0F0FULL;
    rhs_view.data()[0] = 0xF0F0ULL;

    const auto out = bitcal::bit_or<256>(lhs.view(), rhs.view());
    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xFFFFULL});
    return true;
}

bool test_vnext_equals_reports_equal_and_not_equal_192() {
    const std::array<std::uint64_t, 3> a{1ULL, 2ULL, 3ULL};
    const std::array<std::uint64_t, 3> b{1ULL, 2ULL, 3ULL};
    const std::array<std::uint64_t, 3> c{1ULL, 2ULL, 4ULL};

    BITCAL_ASSERT_TRUE(bitcal::equals(bitcal::const_bit_view(a.data(), a.size()),
                                      bitcal::const_bit_view(b.data(), b.size())));
    BITCAL_ASSERT_TRUE(!bitcal::equals(bitcal::const_bit_view(a.data(), a.size()),
                                       bitcal::const_bit_view(c.data(), c.size())));
    return true;
}

bool test_vnext_shift_left_clears_when_count_reaches_width_128() {
    const std::array<std::uint64_t, 2> words{0xFFFFFFFFFFFFFFFFULL, 0x1ULL};
    const auto block =
        bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(words.data(), words.size()));

    const auto out = bitcal::shift_left<128>(block.view(), 128);
    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{0});
    return true;
}
```

- [ ] **Step 2: Add failing detail-level tests for helper behavior**

```cpp
bool test_detail_word_ops_or_words_writes_every_word() {
    std::uint64_t lhs_words[] = {0x0FULL, 0xF000ULL};
    std::uint64_t rhs_words[] = {0xF0ULL, 0x0FF0ULL};
    std::uint64_t out_words[] = {0ULL, 0ULL};

    bitcal::detail::or_words(bitcal::const_bit_view(lhs_words, 2), bitcal::const_bit_view(rhs_words, 2),
                             bitcal::bit_view(out_words, 2));

    BITCAL_ASSERT_EQ(out_words[0], std::uint64_t{0xFFULL});
    BITCAL_ASSERT_EQ(out_words[1], std::uint64_t{0xFFF0ULL});
    return true;
}
```

- [ ] **Step 3: Run the focused test target and confirm missing-symbol failure**

Run:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON && cmake --build build-test --config Release -j"$(nproc)"
```

Expected: build failure on unresolved or undeclared vNext free algorithms such as `bit_or`, `equals`, or `shift_left`.

- [ ] **Step 4: Implement the missing algorithms on the vNext path**

```cpp
inline void or_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::or_words(lhs, rhs, out);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    bit_block<Bits> out;
    or_into(lhs, rhs, out.view());
    return out;
}

[[nodiscard]] constexpr bool equals(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::equals_words(lhs, rhs);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_left(const const_bit_view value, const int count) noexcept {
    bit_block<Bits> out;
    detail::shift_left_words<Bits>(value, out.view(), count);
    return out;
}
```

- [ ] **Step 5: Keep the implementation vocabulary vNext-centric**

```cpp
/**
 * @brief Internal backend helpers for vNext free algorithms
 *
 * These helpers support the owner/view/algorithm model.
 * They are not a public compatibility layer for the removed `bitarray` API.
 */
```

- [ ] **Step 6: Re-run the focused tests until they pass**

Run:

```bash
cmake --build build-test --config Release -j"$(nproc)" && ctest --test-dir build-test --output-on-failure -C Release -R "test_bitcal|test_bitcal_detail"
```

Expected: the public and detail suites pass with the new algorithm coverage active.

- [ ] **Step 7: Commit the algorithm backfill**

```bash
git add include/bitcal/algorithms.hpp include/bitcal/detail/word_ops.hpp include/bitcal/detail/backend.hpp include/bitcal/detail/x64_dispatch.hpp tests/test_bitcal.cpp tests/test_bitcal_detail.cpp
git commit -m "feat: complete vNext free algorithm surface"
```

### Task 3: Delete the legacy public type and other ambiguous entry points

**Files:**
- Modify: `include/bitcal/bitcal.hpp`
- Delete: `include/bitcal/bitarray.hpp`
- Modify: `benchmarks/CMakeLists.txt`
- Delete: `benchmarks/benchmark_compare_legacy.cpp`
- Modify: `examples/CMakeLists.txt`
- Delete: `examples/performance_comparison.cpp`
- Modify: `README.md`
- Modify: `README.zh-CN.md`

- [ ] **Step 1: Remove `bitarray` from the umbrella header**

```cpp
#pragma once

#include "config.hpp"
#include "bit_view.hpp"
#include "bit_block.hpp"
#include "algorithms.hpp"
```

- [ ] **Step 2: Delete the legacy public header**

Run:

```bash
rm include/bitcal/bitarray.hpp
```

Expected: the repository no longer ships a public `bitarray` contract file.

- [ ] **Step 3: Stop building the legacy comparison benchmark**

```cmake
add_executable(bitcal_benchmark benchmark_bitcal.cpp)
bitcal_setup_dev_target(bitcal_benchmark)

add_executable(benchmark_compare benchmark_compare.cpp)
bitcal_setup_dev_target(benchmark_compare)

foreach(target bitcal_benchmark benchmark_compare)
  target_compile_definitions(${target} PRIVATE
    BITCAL_GIT_COMMIT="${BITCAL_GIT_COMMIT}"
    BITCAL_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
  )
endforeach()
```

- [ ] **Step 4: Remove the benchmark-flavored example**

```cmake
add_executable(basic_usage basic_usage.cpp)
bitcal_setup_dev_target(basic_usage)
```

- [ ] **Step 5: Tighten the README migration wording**

```md
## Migration note

The legacy `bitarray` API has been removed from the shipped vNext public surface.
BitCal 4.x expects consumers to migrate to `bit_block`, `bit_view` / `const_bit_view`, and free algorithms through `<bitcal/bitcal.hpp>`.
```

- [ ] **Step 6: Reconfigure to catch accidental legacy references**

Run:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON && cmake --build build-test --config Release -j"$(nproc)"
```

Expected: no compile unit still references `bitcal::bitarray` or `benchmark_compare_legacy`.

- [ ] **Step 7: Commit the legacy-surface deletion**

```bash
git add include/bitcal/bitcal.hpp benchmarks/CMakeLists.txt examples/CMakeLists.txt README.md README.zh-CN.md
git add -u include/bitcal/bitarray.hpp benchmarks/benchmark_compare_legacy.cpp examples/performance_comparison.cpp
git commit -m "refactor: remove legacy bitarray surface"
```

### Task 4: Update retained benchmarks and evidence after the surface cleanup

**Files:**
- Modify: `benchmarks/benchmark_compare.cpp`
- Modify: `benchmarks/scripts/generate-performance-summary.mjs`
- Modify: `benchmarks/results/retained/baseline-x86_64-avx2.json`
- Modify: `benchmarks/results/retained/baseline-x86_64-avx2.summary.json`
- Modify: `docs/.vitepress/theme/data/performanceBaseline.ts`

- [ ] **Step 1: Expand the retained comparison benchmark to the new public algorithm surface**

```cpp
bitcal::bench::comparison_row or_row{};
or_row.operation = "bit_or";
or_row.bits = Bits;
or_row.bitcal = bitcal::bench::measure_ns([&]() {
        const auto out = bitcal::bit_or<Bits>(lhs_block.view(), rhs_block.view());
        bitcal::bench::do_not_optimize(out);
    }, kWarmupIterations, kSamples, kIterationsPerSample);
or_row.std_bitset = bitcal::bench::measure_ns([&]() {
        const auto out = lhs_bitset | rhs_bitset;
        bitcal::bench::do_not_optimize(out);
    }, kWarmupIterations, kSamples, kIterationsPerSample);
report.scenarios.push_back(or_row);
```

- [ ] **Step 2: Keep the published summary generator aligned with the wider retained row set**

```js
const retainedOperationOrder = [
  'bit_and',
  'bit_or',
  'bit_xor',
  'bit_andnot',
  'popcount',
  'equals',
  'is_zero',
  'shift_left',
  'shift_right'
]
```

- [ ] **Step 3: Regenerate the retained raw and summary artifacts**

Run:

```bash
./build-test/benchmarks/benchmark_compare --json-out benchmarks/results/retained/baseline-x86_64-avx2.json && node benchmarks/scripts/generate-performance-summary.mjs benchmarks/results/retained/baseline-x86_64-avx2.json benchmarks/results/retained/baseline-x86_64-avx2.summary.json
```

Expected: the committed retained artifacts now match the widened vNext public algorithm set and no legacy benchmark binary is involved.

- [ ] **Step 4: Commit the refreshed benchmark evidence**

```bash
git add benchmarks/benchmark_compare.cpp benchmarks/scripts/generate-performance-summary.mjs benchmarks/results/retained/baseline-x86_64-avx2.json benchmarks/results/retained/baseline-x86_64-avx2.summary.json docs/.vitepress/theme/data/performanceBaseline.ts
git commit -m "bench: refresh retained vNext baseline"
```

### Task 5: Rewrite docs so the public story matches the cleaned codebase

**Files:**
- Modify: `docs/en/performance/index.md`
- Modify: `docs/zh/performance/index.md`
- Modify: `docs/en/research/evolution-and-comparisons.md`
- Modify: `docs/zh/research/evolution-and-comparisons.md`
- Modify: `docs/en/status/index.md`
- Modify: `docs/zh/status/index.md`
- Modify: `README.md`
- Modify: `README.zh-CN.md`

- [ ] **Step 1: Remove legacy-lane wording from the performance page**

```md
| `benchmark_compare` | Publishes the retained vNext baseline for the current public algorithms and writes the raw JSON artifact. | The baseline needs a reproducible comparison harness, a structured report, and an explicit claim boundary. |
| `bitcal_benchmark` | Stays in Verification Path as the smoke-level executable baseline. | Verification still needs a lighter executable check distinct from the published comparison experiment. |
```

- [ ] **Step 2: Rewrite the research page comparator tiers without a built-in legacy lane**

```md
## Comparator tiers

1. **Retained baseline** — `benchmark_compare` measures the current vNext public algorithms against `std::bitset` and feeds the committed performance artifacts.
2. **Future optional comparators** — libraries such as Boost `dynamic_bitset`, CRoaring, or `libpopcnt` remain useful research candidates, but only as explicitly opt-in comparisons.
```

- [ ] **Step 3: Tighten the status page and README to say the migration is complete at the public seam**

```md
- The stable public model is `bit_block<Bits>`, `bit_view`, `const_bit_view`, and free algorithms.
- The legacy `bitarray` API is no longer shipped as part of the retained vNext public surface.
```

- [ ] **Step 4: Build the docs contract after the wording change**

Run:

```bash
cd docs && npm run check:content-contract && npm run check:theme-contract && npm run check:ia && npm run build
```

Expected: the docs tree no longer mentions a maintained legacy benchmark lane or shipped `bitarray` compatibility surface.

- [ ] **Step 5: Commit the docs cleanup**

```bash
git add docs/en/performance/index.md docs/zh/performance/index.md docs/en/research/evolution-and-comparisons.md docs/zh/research/evolution-and-comparisons.md docs/en/status/index.md docs/zh/status/index.md README.md README.zh-CN.md
git commit -m "docs: align site with legacy pruning"
```

### Task 6: Run the full retained verification path and finish cleanly

**Files:**
- Verify only

- [ ] **Step 1: Run the full C++ retained path**

Run:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
./build-test/benchmarks/benchmark_compare --json-out benchmarks/results/retained/baseline-x86_64-avx2.json
node benchmarks/scripts/generate-performance-summary.mjs benchmarks/results/retained/baseline-x86_64-avx2.json benchmarks/results/retained/baseline-x86_64-avx2.summary.json
```

Expected: all retained validation steps pass using only the vNext public model.

- [ ] **Step 2: Run the doc contract and whitespace checks**

Run:

```bash
cd docs && npm run check:content-contract && npm run check:theme-contract && npm run check:ia && npm run build
git --no-pager diff --check
```

Expected: docs contract passes and the tree is free of patch-formatting issues.

- [ ] **Step 3: Inspect the final diff for scope control**

Run:

```bash
git --no-pager status --short
git --no-pager diff --stat
```

Expected: only the planned legacy-pruning, vNext algorithm, benchmark, and docs files are changed.

- [ ] **Step 4: Create the final commit**

```bash
git add -A
git commit -m "refactor: finish vNext legacy pruning"
```

## Self-review

- **Spec coverage:** the plan covers the new OpenSpec change, vNext algorithm backfill, legacy public-surface deletion, retained benchmark refresh, and docs/status alignment. No public-breaking action is left undocumented.
- **Placeholder scan:** no `TODO` / `TBD` placeholders remain; every task names exact files, commands, and concrete code targets.
- **Type consistency:** the plan consistently treats `bit_block`, `bit_view`, `const_bit_view`, `backend_kind`, and the free algorithms as the only retained public model, and consistently treats `bitarray` as deleted rather than “temporarily public”.
