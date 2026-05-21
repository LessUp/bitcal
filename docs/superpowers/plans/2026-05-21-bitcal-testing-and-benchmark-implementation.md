# BitCal Testing and Benchmark Framework Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a serious but lightweight BitCal correctness and benchmark framework, commit retained performance results into the repository, and wire the docs to those retained artifacts instead of hand-written numbers.

**Architecture:** Keep the existing lightweight C++ macro test style and two-benchmark split, but add a shared test support layer, deterministic and randomized differential coverage, a retained vNext benchmark harness, committed JSON result artifacts, and docs that render from structured performance data. The retained evidence path will only cover the current vNext public surface and `std::bitset`; legacy `bitarray`-only comparisons move to a compatibility/research lane.

**Tech Stack:** C++23, CMake, existing lightweight test macros, Node.js built-ins for JSON normalization, VitePress

---

## File Structure Lock

### Test support and suites

- Create: `tests/support/test_macros.hpp`
- Create: `tests/support/deterministic_cases.hpp`
- Create: `tests/support/reference_model.hpp`
- Create: `tests/support/random_cases.hpp`
- Modify: `tests/test_bitcal.cpp`
- Modify: `tests/test_bitcal_detail.cpp`

### Benchmark harness and retained artifacts

- Create: `benchmarks/benchmark_harness.hpp`
- Create: `benchmarks/benchmark_compare_legacy.cpp`
- Modify: `benchmarks/CMakeLists.txt`
- Modify: `benchmarks/benchmark_bitcal.cpp`
- Modify: `benchmarks/benchmark_compare.cpp`
- Create: `benchmarks/scripts/generate-performance-summary.mjs`
- Create: `benchmarks/results/retained/baseline-x86_64-avx2.json`
- Create: `benchmarks/results/retained/baseline-x86_64-avx2.summary.json`

### Docs and content guards

- Create: `docs/.vitepress/theme/data/performanceBaseline.ts`
- Modify: `docs/en/performance/index.md`
- Modify: `docs/zh/performance/index.md`
- Modify: `docs/en/guide/verification.md`
- Modify: `docs/zh/guide/verification.md`
- Modify: `docs/en/research/evolution-and-comparisons.md`
- Modify: `docs/zh/research/evolution-and-comparisons.md`
- Modify: `docs/scripts/check-content-contract.mjs`

---

### Task 1: Refactor the public test suite into a reusable deterministic contract matrix

**Files:**
- Create: `tests/support/test_macros.hpp`
- Create: `tests/support/deterministic_cases.hpp`
- Modify: `tests/test_bitcal.cpp`
- Modify: `tests/test_bitcal_detail.cpp`
- Test: `build-test/tests/test_bitcal`
- Test: `build-test/tests/test_bitcal_detail`

- [ ] **Step 1: Add failing public contract coverage for representative widths and edge-pattern cases**

```cpp
bool test_vnext_bit_and_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitAndCases128) {
        const auto lhs = bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.lhs.data(), tc.lhs.size()));
        const auto rhs = bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.rhs.data(), tc.rhs.size()));
        const auto out = bitcal::bit_and<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_vnext_popcount_matches_deterministic_matrix_256() {
    for (const auto& tc : bitcal::test::kPopcountCases256) {
        const auto block = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(tc.words.data(), tc.words.size()));
        BITCAL_ASSERT_EQ(bitcal::popcount(block.view()), tc.expected);
    }

    return true;
}
```

Run:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --target test_bitcal -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release -R test_bitcal
```

Expected: FAIL because `bitcal::test::kBitAndCases128`, `kPopcountCases256`, and `BITCAL_ASSERT_EQ` do not exist yet.

- [ ] **Step 2: Create shared assertion helpers in `tests/support/test_macros.hpp`**

```cpp
#pragma once

#include <iostream>
#include <utility>

namespace bitcal::test {

struct suite_counters {
    int pass = 0;
    int fail = 0;
};

template <typename Func>
void run_case(suite_counters& counters, const char* name, Func&& func) {
    std::cout << "  " << name << " ... ";
    if (func()) {
        std::cout << "PASS" << std::endl;
        ++counters.pass;
    } else {
        std::cout << "FAIL" << std::endl;
        ++counters.fail;
    }
}

}  // namespace bitcal::test

#define BITCAL_ASSERT_TRUE(expr)                                                                        \
    do {                                                                                                \
        if (!(expr)) {                                                                                  \
            std::cerr << "  FAIL: " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl;   \
            return false;                                                                               \
        }                                                                                               \
    } while (0)

#define BITCAL_ASSERT_EQ(a, b)                                                                                         \
    do {                                                                                                               \
        if ((a) != (b)) {                                                                                              \
            std::cerr << "  FAIL: " << #a << " == " << #b << " (got " << (a) << " vs " << (b) << ")"                 \
                      << " at " << __FILE__ << ":" << __LINE__ << std::endl;                                           \
            return false;                                                                                              \
        }                                                                                                              \
    } while (0)
```

- [ ] **Step 3: Create deterministic case tables in `tests/support/deterministic_cases.hpp`**

```cpp
#pragma once

#include <array>
#include <cstdint>

namespace bitcal::test {

template <std::size_t WordCount>
struct binary_case {
    std::array<std::uint64_t, WordCount> lhs;
    std::array<std::uint64_t, WordCount> rhs;
    std::array<std::uint64_t, WordCount> expected;
};

template <std::size_t WordCount>
struct unary_count_case {
    std::array<std::uint64_t, WordCount> words;
    std::uint64_t expected;
};

inline constexpr std::array<binary_case<2>, 3> kBitAndCases128{{
    {{{0ULL, 0ULL}}, {{0ULL, 0ULL}}, {{0ULL, 0ULL}}},
    {{{0xFFFF0000FFFF0000ULL, 0xAAAAAAAAAAAAAAAAULL}},
     {{0x00FF00FF00FF00FFULL, 0xF0F0F0F0F0F0F0F0ULL}},
     {{0x00FF000000FF0000ULL, 0xA0A0A0A0A0A0A0A0ULL}}},
    {{{1ULL << 63, 1ULL}}, {{1ULL << 63, 0ULL}}, {{1ULL << 63, 0ULL}}},
}};

inline constexpr std::array<unary_count_case<4>, 3> kPopcountCases256{{
    {{{0ULL, 0ULL, 0ULL, 0ULL}}, 0ULL},
    {{{0xFFFFFFFFFFFFFFFFULL, 0ULL, 0ULL, 0ULL}}, 64ULL},
    {{{0b1011ULL, 0ULL, 0ULL, 0b1000ULL}}, 4ULL},
}};

inline constexpr std::array<binary_case<8>, 2> kBitAndCases512{{
    {{{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL}},
     {{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL}},
     {{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL}}},
    {{{0xFFFFFFFFFFFFFFFFULL, 0xAAAAAAAAAAAAAAAAULL, 0x1234567890ABCDEFULL, 0x0000FFFF0000FFFFULL,
       0x0F0F0F0F0F0F0F0FULL, 0x8000000000000000ULL, 0x0000000000000001ULL, 0x5555555555555555ULL}},
     {{0x0000000000000000ULL, 0xF0F0F0F0F0F0F0F0ULL, 0xFEDCBA0987654321ULL, 0xFFFF0000FFFF0000ULL,
       0x3333333333333333ULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, 0xAAAAAAAAAAAAAAAAULL}},
     {{0x0000000000000000ULL, 0xA0A0A0A0A0A0A0A0ULL, 0x1214120880004121ULL, 0x0000000000000000ULL,
       0x0303030303030303ULL, 0x8000000000000000ULL, 0x0000000000000001ULL, 0x0000000000000000ULL}}},
}};

}  // namespace bitcal::test
```

- [ ] **Step 4: Rewrite `tests/test_bitcal.cpp` and `tests/test_bitcal_detail.cpp` to consume the shared helpers**

```cpp
#include "support/test_macros.hpp"
#include "support/deterministic_cases.hpp"

static bitcal::test::suite_counters g_counters;

int main() {
    std::cout << "=== BitCal vNext test suite ===" << std::endl;

    bitcal::test::run_case(g_counters, "test_vnext_bit_and_matches_deterministic_matrix_128",
                           test_vnext_bit_and_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_vnext_popcount_matches_deterministic_matrix_256",
                           test_vnext_popcount_matches_deterministic_matrix_256);

    std::cout << "\nPassed: " << g_counters.pass << "\nFailed: " << g_counters.fail << std::endl;
    return g_counters.fail == 0 ? 0 : 1;
}
```

Also update `tests/test_bitcal_detail.cpp` to replace duplicated `ASSERT_*` and `RUN_TEST` macros with the shared `BITCAL_ASSERT_*` macros and `bitcal::test::run_case`.

- [ ] **Step 5: Expand the deterministic suite to cover 256-bit, 512-bit, and one custom width**

```cpp
bool test_vnext_bit_and_matches_deterministic_matrix_512() {
    for (const auto& tc : bitcal::test::kBitAndCases512) {
        const auto lhs = bitcal::bit_block<512>::from_words(std::span<const std::uint64_t>(tc.lhs.data(), tc.lhs.size()));
        const auto rhs = bitcal::bit_block<512>::from_words(std::span<const std::uint64_t>(tc.rhs.data(), tc.rhs.size()));
        const auto out = bitcal::bit_and<512>(lhs.view(), rhs.view());
        for (std::size_t i = 0; i < bitcal::bit_block<512>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }
    return true;
}

bool test_vnext_is_zero_detects_sparse_and_dense_patterns_192() {
    const std::array<std::uint64_t, 3> zero_words{0ULL, 0ULL, 0ULL};
    const std::array<std::uint64_t, 3> sparse_words{0ULL, 1ULL << 17, 0ULL};
    const auto zero = bitcal::bit_block<192>::from_words(std::span<const std::uint64_t>(zero_words.data(), zero_words.size()));
    const auto sparse = bitcal::bit_block<192>::from_words(std::span<const std::uint64_t>(sparse_words.data(), sparse_words.size()));

    BITCAL_ASSERT_TRUE(bitcal::is_zero(zero.view()));
    BITCAL_ASSERT_TRUE(!bitcal::is_zero(sparse.view()));
    return true;
}

bool test_vnext_view_word_count_matches_custom_width_192() {
    bitcal::bit_block<192> block;
    const auto view = block.view();
    BITCAL_ASSERT_EQ(bitcal::bit_block<192>::word_count, std::size_t{3});
    BITCAL_ASSERT_EQ(view.word_count(), std::size_t{3});
    return true;
}
```

Run:

```bash
cmake --build build-test --config Release --target test_bitcal test_bitcal_detail -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release -R "test_bitcal|test_bitcal_detail"
```

Expected: PASS for both test executables.

- [ ] **Step 6: Commit the deterministic contract test refactor**

```bash
git add tests/support/test_macros.hpp tests/support/deterministic_cases.hpp tests/test_bitcal.cpp tests/test_bitcal_detail.cpp
git commit -m "test: expand deterministic vnext contract coverage"
```

---

### Task 2: Add randomized differential tests against a scalar reference model

**Files:**
- Create: `tests/support/reference_model.hpp`
- Create: `tests/support/random_cases.hpp`
- Modify: `tests/test_bitcal.cpp`
- Test: `build-test/tests/test_bitcal`

- [ ] **Step 1: Add failing randomized differential tests for the current vNext public algorithms**

```cpp
bool test_vnext_random_bit_and_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0xB17CA1ULL, 64)) {
        const auto actual = bitcal::bit_and<256>(tc.lhs.view(), tc.rhs.view());
        const auto expected = bitcal::test::reference_bit_and<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_vnext_random_queries_match_reference_model_512() {
    for (const auto& tc : bitcal::test::make_random_unary_cases<512>(0xC0FFEEULL, 64)) {
        BITCAL_ASSERT_EQ(bitcal::popcount(tc.block.view()), bitcal::test::reference_popcount(tc.words));
        BITCAL_ASSERT_EQ(bitcal::is_zero(tc.block.view()), bitcal::test::reference_is_zero(tc.words));
    }

    return true;
}
```

Run:

```bash
cmake --build build-test --config Release --target test_bitcal -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release -R test_bitcal
```

Expected: FAIL because `make_random_binary_cases`, `make_random_unary_cases`, and the reference helpers do not exist yet.

- [ ] **Step 2: Create `tests/support/reference_model.hpp` with scalar gold-model helpers**

```cpp
#pragma once

#include <array>
#include <bit>
#include <cstdint>

namespace bitcal::test {

template <std::size_t Bits>
using word_array = std::array<std::uint64_t, Bits / 64>;

template <std::size_t Bits>
word_array<Bits> reference_bit_and(const word_array<Bits>& lhs, const word_array<Bits>& rhs) {
    word_array<Bits> out{};
    for (std::size_t i = 0; i < out.size(); ++i) out[i] = lhs[i] & rhs[i];
    return out;
}

template <std::size_t WordCount>
std::uint64_t reference_popcount(const std::array<std::uint64_t, WordCount>& words) {
    std::uint64_t total = 0;
    for (const auto word : words) total += std::popcount(word);
    return total;
}

template <std::size_t WordCount>
bool reference_is_zero(const std::array<std::uint64_t, WordCount>& words) {
    for (const auto word : words) {
        if (word != 0) return false;
    }
    return true;
}

}  // namespace bitcal::test
```

- [ ] **Step 3: Create `tests/support/random_cases.hpp` with deterministic seeded generators**

```cpp
#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <span>
#include <vector>

#include <bitcal/bit_block.hpp>

namespace bitcal::test {

template <std::size_t Bits>
struct random_binary_case {
    bitcal::bit_block<Bits> lhs;
    bitcal::bit_block<Bits> rhs;
    std::array<std::uint64_t, Bits / 64> lhs_words;
    std::array<std::uint64_t, Bits / 64> rhs_words;
};

template <std::size_t Bits>
struct random_unary_case {
    bitcal::bit_block<Bits> block;
    std::array<std::uint64_t, Bits / 64> words;
};

template <std::size_t Bits>
std::vector<random_binary_case<Bits>> make_random_binary_cases(std::uint64_t seed, std::size_t count) {
    std::mt19937_64 rng(seed);
    std::vector<random_binary_case<Bits>> cases;
    cases.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        random_binary_case<Bits> tc{};
        for (std::size_t word = 0; word < Bits / 64; ++word) {
            tc.lhs_words[word] = rng();
            tc.rhs_words[word] = rng();
        }
        tc.lhs = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(tc.lhs_words.data(), tc.lhs_words.size()));
        tc.rhs = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(tc.rhs_words.data(), tc.rhs_words.size()));
        cases.push_back(tc);
    }

    return cases;
}

template <std::size_t Bits>
std::vector<random_unary_case<Bits>> make_random_unary_cases(std::uint64_t seed, std::size_t count) {
    std::mt19937_64 rng(seed);
    std::vector<random_unary_case<Bits>> cases;
    cases.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        random_unary_case<Bits> tc{};
        for (std::size_t word = 0; word < Bits / 64; ++word) {
            tc.words[word] = rng();
        }
        tc.block = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(tc.words.data(), tc.words.size()));
        cases.push_back(tc);
    }

    return cases;
}

}  // namespace bitcal::test
```

- [ ] **Step 4: Wire the randomized tests into `tests/test_bitcal.cpp` and keep the seeds fixed**

```cpp
bitcal::test::run_case(g_counters, "test_vnext_random_bit_and_matches_reference_model_256",
                       test_vnext_random_bit_and_matches_reference_model_256);
bitcal::test::run_case(g_counters, "test_vnext_random_queries_match_reference_model_512",
                       test_vnext_random_queries_match_reference_model_512);
```

Run:

```bash
cmake --build build-test --config Release --target test_bitcal -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release -R test_bitcal
```

Expected: PASS. The suite now covers deterministic and randomized differential correctness for `bit_and`, `popcount`, and `is_zero`.

- [ ] **Step 5: Commit the differential test layer**

```bash
git add tests/support/reference_model.hpp tests/support/random_cases.hpp tests/test_bitcal.cpp
git commit -m "test: add randomized differential coverage"
```

---

### Task 3: Rebuild the retained benchmark around the vNext public surface and preserve the old compare as legacy research

**Files:**
- Create: `benchmarks/benchmark_harness.hpp`
- Create: `benchmarks/benchmark_compare_legacy.cpp`
- Modify: `benchmarks/CMakeLists.txt`
- Modify: `benchmarks/benchmark_bitcal.cpp`
- Modify: `benchmarks/benchmark_compare.cpp`
- Test: `build-test/benchmarks/bitcal_benchmark`
- Test: `build-test/benchmarks/benchmark_compare`
- Test: `build-test/benchmarks/benchmark_compare_legacy`

- [ ] **Step 1: Capture the current compare benchmark as the legacy lane before rewriting the retained baseline**

```cpp
// benchmarks/benchmark_compare_legacy.cpp
// Start by copying the current benchmarks/benchmark_compare.cpp verbatim.
// This preserves the broader bitarray-based comparison as a compatibility/research executable.
```

Run:

```bash
cmake --build build-test --config Release --target benchmark_compare_legacy -j"$(nproc)"
```

Expected: FAIL because the new target is not declared in `benchmarks/CMakeLists.txt` yet.

- [ ] **Step 2: Update `benchmarks/CMakeLists.txt` to keep three explicit benchmark roles**

```cmake
add_executable(bitcal_benchmark benchmark_bitcal.cpp)
bitcal_setup_dev_target(bitcal_benchmark)

add_executable(benchmark_compare benchmark_compare.cpp)
bitcal_setup_dev_target(benchmark_compare)

add_executable(benchmark_compare_legacy benchmark_compare_legacy.cpp)
bitcal_setup_dev_target(benchmark_compare_legacy)
```

Also keep the optional Google Benchmark discovery limited to `bitcal_benchmark`; do not make it a hard dependency for the retained compare lane.

- [ ] **Step 3: Create `benchmarks/benchmark_harness.hpp` with reusable timing, stats, environment, and JSON reporting helpers**

```cpp
#pragma once

#include <chrono>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace bitcal::bench {

struct sample_summary {
    std::size_t samples = 0;
    std::size_t iterations_per_sample = 0;
    double min_ns = 0.0;
    double max_ns = 0.0;
    double mean_ns = 0.0;
    double median_ns = 0.0;
    double stdev_ns = 0.0;
    double cv = 0.0;
};

struct comparison_row {
    std::string operation;
    std::size_t bits = 0;
    sample_summary bitcal;
    sample_summary std_bitset;
};

struct environment_fingerprint {
    std::string commit;
    std::string compiler;
    std::string build_type;
    std::string backend;
    std::string cpu;
};

struct run_metadata {
    std::string profile;
    std::string generated_at;
};

struct benchmark_report {
    run_metadata metadata;
    environment_fingerprint environment;
    std::vector<comparison_row> scenarios;
};

template <typename T>
inline void do_not_optimize(const T& value) {
    asm volatile("" : : "r,m"(value) : "memory");
}

template <typename Func>
sample_summary measure_ns(Func&& func, std::size_t warmup, std::size_t samples, std::size_t iterations_per_sample);

const char* active_backend_name() noexcept;

void write_json_report(std::ostream& out,
                       const benchmark_report& report);

}  // namespace bitcal::bench
```

- [ ] **Step 4: Rewrite `benchmarks/benchmark_compare.cpp` to benchmark only the current vNext public algorithms**

```cpp
template <std::size_t Bits>
void append_retained_cases(bitcal::bench::benchmark_report& report) {
    std::mt19937_64 rng(42);
    std::array<std::uint64_t, Bits / 64> lhs_words{};
    std::array<std::uint64_t, Bits / 64> rhs_words{};
    for (std::size_t i = 0; i < lhs_words.size(); ++i) {
        lhs_words[i] = rng();
        rhs_words[i] = rng();
    }

    const auto lhs_block = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(lhs_words.data(), lhs_words.size()));
    const auto rhs_block = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(rhs_words.data(), rhs_words.size()));

    auto to_bitset = [](const std::array<std::uint64_t, Bits / 64>& words) {
        std::bitset<Bits> out;
        for (std::size_t base = 0; base < Bits; base += 64) {
            const auto word = words[base / 64];
            for (std::size_t bit = 0; bit < 64 && (base + bit) < Bits; ++bit) {
                if ((word >> bit) & 1ULL) out.set(base + bit);
            }
        }
        return out;
    };

    const auto lhs_bitset = to_bitset(lhs_words);
    const auto rhs_bitset = to_bitset(rhs_words);

    report.scenarios.push_back({
        .operation = "bit_and",
        .bits = Bits,
        .bitcal = bitcal::bench::measure_ns([&]() {
            const auto out = bitcal::bit_and<Bits>(lhs_block.view(), rhs_block.view());
            bitcal::bench::do_not_optimize(out);
        }, 100, 25, 5000),
        .std_bitset = bitcal::bench::measure_ns([&]() {
            const auto out = lhs_bitset & rhs_bitset;
            bitcal::bench::do_not_optimize(out);
        }, 100, 25, 5000),
    });

    report.scenarios.push_back({
        .operation = "popcount",
        .bits = Bits,
        .bitcal = bitcal::bench::measure_ns([&]() {
            const auto out = bitcal::popcount(lhs_block.view());
            bitcal::bench::do_not_optimize(out);
        }, 100, 25, 5000),
        .std_bitset = bitcal::bench::measure_ns([&]() {
            const auto out = lhs_bitset.count();
            bitcal::bench::do_not_optimize(out);
        }, 100, 25, 5000),
    });

    report.scenarios.push_back({
        .operation = "is_zero",
        .bits = Bits,
        .bitcal = bitcal::bench::measure_ns([&]() {
            const auto out = bitcal::is_zero(lhs_block.view());
            bitcal::bench::do_not_optimize(out);
        }, 100, 25, 5000),
        .std_bitset = bitcal::bench::measure_ns([&]() {
            const auto out = lhs_bitset.none();
            bitcal::bench::do_not_optimize(out);
        }, 100, 25, 5000),
    });
}
```

Keep widths at `128`, `256`, `512`, and `192` (custom-width evidence), and add `--json-out <path>` to persist the raw retained report.

- [ ] **Step 5: Make `bitcal_benchmark` reuse the shared harness metadata printer without changing its smoke role**

```cpp
std::cout << "=== BitCal vNext benchmark baseline ===\n";
std::cout << "default backend: " << bitcal::bench::active_backend_name() << '\n';
std::cout << "profile: smoke-only\n";
```

The smoke executable should stay simple: it proves the benchmark target runs and reports a backend, but it does not publish retained results.

- [ ] **Step 6: Build and run the three benchmark executables**

Run:

```bash
cmake --build build-test --config Release --target bitcal_benchmark benchmark_compare benchmark_compare_legacy -j"$(nproc)"
./build-test/benchmarks/bitcal_benchmark
./build-test/benchmarks/benchmark_compare --json-out /tmp/bitcal-retained.json
./build-test/benchmarks/benchmark_compare_legacy | head -40
```

Expected:

- `bitcal_benchmark` prints a smoke baseline and backend name
- `benchmark_compare` prints the retained vNext comparison and writes `/tmp/bitcal-retained.json`
- `benchmark_compare_legacy` preserves the broader legacy/bitarray comparison lane

- [ ] **Step 7: Commit the benchmark harness split**

```bash
git add benchmarks/CMakeLists.txt benchmarks/benchmark_harness.hpp benchmarks/benchmark_bitcal.cpp benchmarks/benchmark_compare.cpp benchmarks/benchmark_compare_legacy.cpp
git commit -m "bench: split retained and legacy comparison lanes"
```

---

### Task 4: Normalize retained benchmark output and commit the baseline artifacts

**Files:**
- Create: `benchmarks/scripts/generate-performance-summary.mjs`
- Create: `benchmarks/results/retained/baseline-x86_64-avx2.json`
- Create: `benchmarks/results/retained/baseline-x86_64-avx2.summary.json`
- Test: `benchmarks/results/retained/baseline-x86_64-avx2.json`
- Test: `benchmarks/results/retained/baseline-x86_64-avx2.summary.json`

- [ ] **Step 1: Prove that the retained compare can already write a raw JSON file**

Run:

```bash
./build-test/benchmarks/benchmark_compare --json-out /tmp/bitcal-retained.json
node -e "const fs=require('node:fs'); const p='/tmp/bitcal-retained.json'; if (!fs.existsSync(p)) process.exit(1); console.log('json exists')"
```

Expected: `json exists`

- [ ] **Step 2: Create `benchmarks/scripts/generate-performance-summary.mjs`**

```js
import fs from 'node:fs'

const [inputPath, outputPath] = process.argv.slice(2)
const report = JSON.parse(fs.readFileSync(inputPath, 'utf8'))

const byBits = new Map()
for (const scenario of report.scenarios) {
  const key = String(scenario.bits)
  if (!byBits.has(key)) byBits.set(key, [])
  byBits.get(key).push({
    operation: `${scenario.operation}<${scenario.bits}>`,
    bitcal: scenario.bitcal.median_ns.toFixed(2),
    stdBitset: scenario.std_bitset.median_ns.toFixed(2),
    ratio: (scenario.std_bitset.median_ns / scenario.bitcal.median_ns).toFixed(2),
    highlight: scenario.std_bitset.median_ns > scenario.bitcal.median_ns,
  })
}

const summary = {
  profile: report.metadata.profile,
  backend: report.environment.backend,
  commit: report.environment.commit,
  tables: Object.fromEntries(byBits),
}

fs.writeFileSync(outputPath, `${JSON.stringify(summary, null, 2)}\n`)
```

- [ ] **Step 3: Generate the retained raw and summary artifacts in the repository**

Run:

```bash
mkdir -p benchmarks/results/retained
./build-test/benchmarks/benchmark_compare --json-out benchmarks/results/retained/baseline-x86_64-avx2.json
node benchmarks/scripts/generate-performance-summary.mjs \
  benchmarks/results/retained/baseline-x86_64-avx2.json \
  benchmarks/results/retained/baseline-x86_64-avx2.summary.json
```

Expected: both JSON files exist and are pretty-printed with trailing newlines.

- [ ] **Step 4: Sanity-check the committed artifacts**

Run:

```bash
node -e "const fs=require('node:fs'); const raw=JSON.parse(fs.readFileSync('benchmarks/results/retained/baseline-x86_64-avx2.json','utf8')); const summary=JSON.parse(fs.readFileSync('benchmarks/results/retained/baseline-x86_64-avx2.summary.json','utf8')); if (!raw.environment || !summary.tables['256']) process.exit(1); console.log('artifacts valid')"
```

Expected: `artifacts valid`

- [ ] **Step 5: Commit the retained benchmark artifacts**

```bash
git add benchmarks/scripts/generate-performance-summary.mjs benchmarks/results/retained/baseline-x86_64-avx2.json benchmarks/results/retained/baseline-x86_64-avx2.summary.json
git commit -m "bench: commit retained performance baseline artifacts"
```

---

### Task 5: Make the docs consume retained data and guard against hard-coded benchmark drift

**Files:**
- Create: `docs/.vitepress/theme/data/performanceBaseline.ts`
- Modify: `docs/en/performance/index.md`
- Modify: `docs/zh/performance/index.md`
- Modify: `docs/en/guide/verification.md`
- Modify: `docs/zh/guide/verification.md`
- Modify: `docs/en/research/evolution-and-comparisons.md`
- Modify: `docs/zh/research/evolution-and-comparisons.md`
- Modify: `docs/scripts/check-content-contract.mjs`
- Test: `cd docs && npm run check:content-contract`
- Test: `cd docs && npm run check:ia`
- Test: `cd docs && npm run check:theme-contract`
- Test: `cd docs && npm run build`

- [ ] **Step 1: Create `docs/.vitepress/theme/data/performanceBaseline.ts` as the docs-facing adapter around the summary JSON**

```ts
import summary from '../../../../benchmarks/results/retained/baseline-x86_64-avx2.summary.json'

export const performanceBaseline = summary

export const tableRows = (bits: '128' | '192' | '256' | '512') =>
  performanceBaseline.tables[bits].map((row: {
    operation: string
    bitcal: string
    stdBitset: string
    ratio: string
    highlight: boolean
  }) => row)
```

- [ ] **Step 2: Rewrite the English and Chinese performance pages to import data instead of hard-coding numbers**

```md
<script setup>
import { performanceBaseline, tableRows } from '../../.vitepress/theme/data/performanceBaseline'
</script>

<PerformanceTable
  title="256-bit operations"
  :caption="`Retained baseline on ${performanceBaseline.backend}`"
  :rows="tableRows('256')"
  :highlightBest="true"
/>
```

Apply the same pattern in both locales, and remove inline object literals such as `{ operation: 'and<256>', bitcal: '1.34', ... }`.

- [ ] **Step 3: Update the verification and research pages to explain the retained-vs-legacy split**

```md
| `benchmark_compare` | Retained vNext baseline for current public algorithms (`bit_and`, `popcount`, `is_zero`). | This executable is the source of the committed baseline JSON artifacts. |
| `benchmark_compare_legacy` | Compatibility/research lane for broader legacy `bitarray` comparisons. | Useful for exploration, but not part of the retained whitepaper evidence chain. |
```

Also update the research pages so comparator tiers are explicit: `std::bitset` retained, third-party libraries optional, legacy `bitarray`-only comparisons non-primary.

- [ ] **Step 4: Extend `docs/scripts/check-content-contract.mjs` to prevent a return to hand-written benchmark tables**

```js
const enPerformance = read('en/performance/index.md')
const zhPerformance = read('zh/performance/index.md')

expect(enPerformance.includes('performanceBaseline'), 'en/performance/index.md must import performanceBaseline data')
expect(zhPerformance.includes('performanceBaseline'), 'zh/performance/index.md must import performanceBaseline data')
expect(!enPerformance.includes("{ operation: 'and<256>'"), 'en/performance/index.md must not hardcode benchmark rows')
expect(!zhPerformance.includes("{ operation: 'and<256>'"), 'zh/performance/index.md must not hardcode benchmark rows')
```

- [ ] **Step 5: Run the docs validation stack**

Run:

```bash
cd docs && npm run check:content-contract && npm run check:theme-contract && npm run check:ia && npm run build
```

Expected: PASS for all four commands.

- [ ] **Step 6: Commit the docs/data integration**

```bash
git add docs/.vitepress/theme/data/performanceBaseline.ts docs/en/performance/index.md docs/zh/performance/index.md docs/en/guide/verification.md docs/zh/guide/verification.md docs/en/research/evolution-and-comparisons.md docs/zh/research/evolution-and-comparisons.md docs/scripts/check-content-contract.mjs
git commit -m "docs: drive performance pages from retained baseline data"
```

---

### Task 6: Run the full retained verification path and commit the final integrated slice

**Files:**
- Modify: `docs/superpowers/specs/2026-05-21-bitcal-testing-and-benchmark-design.md`
- Modify: `docs/superpowers/plans/2026-05-21-bitcal-testing-and-benchmark-implementation.md`
- Test: `build-test`

- [ ] **Step 1: Re-run the full C++ verification path from a clean build directory**

Run:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
./build-test/benchmarks/benchmark_compare --json-out /tmp/bitcal-retained-final.json
node benchmarks/scripts/generate-performance-summary.mjs /tmp/bitcal-retained-final.json /tmp/bitcal-retained-final.summary.json
```

Expected:

- configure/build pass
- `ctest` passes
- `bitcal_benchmark` runs
- retained compare writes JSON
- summary generation succeeds

- [ ] **Step 2: Re-run the docs validation stack and diff hygiene**

Run:

```bash
cd docs && npm run check:content-contract && npm run check:theme-contract && npm run check:ia && npm run build
cd ..
git diff --check
```

Expected: PASS with no whitespace or patch formatting errors.

- [ ] **Step 3: Update the design and plan docs only if implementation scope narrowed or expanded**

```md
- If retained baseline had to be limited to `bit_and`, `popcount`, and `is_zero` because the broader free-algorithm surface does not exist yet, keep that note explicit in the design and plan artifacts.
- If no scope changed, leave the docs untouched.
```

- [ ] **Step 4: Commit the final integrated slice**

```bash
git add tests benchmarks docs
git commit -m "feat: add retained testing and benchmark evidence pipeline"
```
