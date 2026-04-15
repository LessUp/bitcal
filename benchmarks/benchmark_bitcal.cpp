/**
 * @file benchmark_bitcal.cpp
 * @brief Performance benchmarks for BitCal library
 *
 * This benchmark suite measures the performance of BitCal operations:
 * - Bitwise operations (AND, OR, XOR, NOT, ANDNOT)
 * - Shift operations (left, right)
 * - Bit counting operations (popcount, clz, ctz)
 * - Comparison between scalar and SIMD backends
 *
 * Usage:
 *   ./benchmark_bitcal
 *
 * With Google Benchmark:
 *   ./benchmark_bitcal --benchmark_time_unit=ns
 */

#include <bitcal/bitcal.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <cstdint>
#include <random>
#include <cstring>

#ifdef BITCAL_HAS_GBENCH
#include <benchmark/benchmark.h>
#endif

namespace {

// Simple timer for non-Google Benchmark builds
class SimpleTimer {
public:
    using clock = std::chrono::high_resolution_clock;

    void start() { start_ = clock::now(); }

    double elapsed_ns() const {
        auto end = clock::now();
        return std::chrono::duration<double, std::nano>(end - start_).count();
    }

private:
    clock::time_point start_;
};

// Generate random data for testing
template<typename BitArray>
void fill_random(BitArray& arr, std::mt19937_64& rng) {
    for (size_t i = 0; i < BitArray::u64_count; ++i) {
        arr[i] = rng();
    }
}

// Benchmark configuration
constexpr int WARMUP_ITERATIONS = 100;
constexpr int BENCHMARK_ITERATIONS = 10000;

} // namespace

#ifndef BITCAL_HAS_GBENCH
// Simple benchmark runner when Google Benchmark is not available
template<typename Func>
void run_benchmark(const char* name, Func&& func, int iterations = BENCHMARK_ITERATIONS) {
    SimpleTimer timer;

    // Warmup
    for (int i = 0; i < WARMUP_ITERATIONS; ++i) {
        func();
    }

    // Benchmark
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    double total_ns = timer.elapsed_ns();
    double avg_ns = total_ns / iterations;

    std::cout << name << ": " << avg_ns << " ns/op" << std::endl;
}

void run_all_benchmarks() {
    std::mt19937_64 rng(42); // Fixed seed for reproducibility

    std::cout << "=== BitCal Performance Benchmarks ===" << std::endl;
    std::cout << std::endl;

    // Prepare test data
    bitcal::bit64 a64(0xDEADBEEFCAFEBABEULL), b64(0x123456789ABCDEF0ULL);
    bitcal::bit128 a128, b128; fill_random(a128, rng); fill_random(b128, rng);
    bitcal::bit256 a256, b256; fill_random(a256, rng); fill_random(b256, rng);
    bitcal::bit512 a512, b512; fill_random(a512, rng); fill_random(b512, rng);
    bitcal::bit1024 a1024, b1024; fill_random(a1024, rng); fill_random(b1024, rng);

    // 64-bit benchmarks
    std::cout << "[64-bit operations]" << std::endl;
    run_benchmark("AND", [&]() { volatile auto r = a64 & b64; });
    run_benchmark("OR", [&]() { volatile auto r = a64 | b64; });
    run_benchmark("XOR", [&]() { volatile auto r = a64 ^ b64; });
    run_benchmark("NOT", [&]() { volatile auto r = ~a64; });
    run_benchmark("ANDNOT", [&]() { volatile auto r = a64.andnot(b64); });
    run_benchmark("ShiftLeft", [&]() { bitcal::bit64 t = a64; t <<= 10; });
    run_benchmark("ShiftRight", [&]() { bitcal::bit64 t = a64; t >>= 10; });
    run_benchmark("Popcount", [&]() { volatile auto r = a64.popcount(); });
    run_benchmark("CLZ", [&]() { volatile auto r = a64.count_leading_zeros(); });
    run_benchmark("CTZ", [&]() { volatile auto r = a64.count_trailing_zeros(); });
    run_benchmark("is_zero", [&]() { volatile bool r = a64.is_zero(); });
    std::cout << std::endl;

    // 128-bit benchmarks
    std::cout << "[128-bit operations]" << std::endl;
    run_benchmark("AND", [&]() { volatile auto r = a128 & b128; });
    run_benchmark("OR", [&]() { volatile auto r = a128 | b128; });
    run_benchmark("XOR", [&]() { volatile auto r = a128 ^ b128; });
    run_benchmark("NOT", [&]() { volatile auto r = ~a128; });
    run_benchmark("ShiftLeft", [&]() { bitcal::bit128 t = a128; t <<= 10; });
    run_benchmark("ShiftRight", [&]() { bitcal::bit128 t = a128; t >>= 10; });
    run_benchmark("Popcount", [&]() { volatile auto r = a128.popcount(); });
    run_benchmark("is_zero", [&]() { volatile bool r = a128.is_zero(); });
    std::cout << std::endl;

    // 256-bit benchmarks
    std::cout << "[256-bit operations]" << std::endl;
    run_benchmark("AND", [&]() { volatile auto r = a256 & b256; });
    run_benchmark("OR", [&]() { volatile auto r = a256 | b256; });
    run_benchmark("XOR", [&]() { volatile auto r = a256 ^ b256; });
    run_benchmark("NOT", [&]() { volatile auto r = ~a256; });
    run_benchmark("ShiftLeft", [&]() { bitcal::bit256 t = a256; t <<= 10; });
    run_benchmark("ShiftRight", [&]() { bitcal::bit256 t = a256; t >>= 10; });
    run_benchmark("Popcount", [&]() { volatile auto r = a256.popcount(); });
    run_benchmark("is_zero", [&]() { volatile bool r = a256.is_zero(); });
    std::cout << std::endl;

    // 512-bit benchmarks
    std::cout << "[512-bit operations]" << std::endl;
    run_benchmark("AND", [&]() { volatile auto r = a512 & b512; });
    run_benchmark("OR", [&]() { volatile auto r = a512 | b512; });
    run_benchmark("XOR", [&]() { volatile auto r = a512 ^ b512; });
    run_benchmark("NOT", [&]() { volatile auto r = ~a512; });
    run_benchmark("ShiftLeft", [&]() { bitcal::bit512 t = a512; t <<= 10; });
    run_benchmark("ShiftRight", [&]() { bitcal::bit512 t = a512; t >>= 10; });
    run_benchmark("Popcount", [&]() { volatile auto r = a512.popcount(); });
    run_benchmark("is_zero", [&]() { volatile bool r = a512.is_zero(); });
    std::cout << std::endl;

    // 1024-bit benchmarks
    std::cout << "[1024-bit operations]" << std::endl;
    run_benchmark("AND", [&]() { volatile auto r = a1024 & b1024; });
    run_benchmark("OR", [&]() { volatile auto r = a1024 | b1024; });
    run_benchmark("XOR", [&]() { volatile auto r = a1024 ^ b1024; });
    run_benchmark("NOT", [&]() { volatile auto r = ~a1024; });
    run_benchmark("ShiftLeft", [&]() { bitcal::bit1024 t = a1024; t <<= 10; });
    run_benchmark("ShiftRight", [&]() { bitcal::bit1024 t = a1024; t >>= 10; });
    run_benchmark("Popcount", [&]() { volatile auto r = a1024.popcount(); });
    run_benchmark("is_zero", [&]() { volatile bool r = a1024.is_zero(); });
    std::cout << std::endl;

    // Backend information
    std::cout << "=== Backend Information ===" << std::endl;
    std::cout << "Default SIMD backend: ";
    switch (bitcal::get_default_backend()) {
        case bitcal::simd_backend::scalar: std::cout << "scalar"; break;
        case bitcal::simd_backend::sse2: std::cout << "SSE2"; break;
        case bitcal::simd_backend::avx: std::cout << "AVX"; break;
        case bitcal::simd_backend::avx2: std::cout << "AVX2"; break;
        case bitcal::simd_backend::avx512: std::cout << "AVX-512"; break;
        case bitcal::simd_backend::neon: std::cout << "NEON"; break;
    }
    std::cout << std::endl;
}

int main() {
    run_all_benchmarks();
    return 0;
}

#else // BITCAL_HAS_GBENCH

// Google Benchmark implementations

// 64-bit benchmarks
static void BM_Bit64_AND(benchmark::State& state) {
    bitcal::bit64 a(0xDEADBEEFCAFEBABEULL), b(0x123456789ABCDEF0ULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(a & b);
    }
}
BENCHMARK(BM_Bit64_AND);

static void BM_Bit64_XOR(benchmark::State& state) {
    bitcal::bit64 a(0xDEADBEEFCAFEBABEULL), b(0x123456789ABCDEF0ULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(a ^ b);
    }
}
BENCHMARK(BM_Bit64_XOR);

static void BM_Bit64_ShiftLeft(benchmark::State& state) {
    bitcal::bit64 a(0xDEADBEEFCAFEBABEULL);
    for (auto _ : state) {
        bitcal::bit64 t = a;
        t <<= 10;
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_Bit64_ShiftLeft);

static void BM_Bit64_Popcount(benchmark::State& state) {
    bitcal::bit64 a(0xDEADBEEFCAFEBABEULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(a.popcount());
    }
}
BENCHMARK(BM_Bit64_Popcount);

// 256-bit benchmarks
static void BM_Bit256_AND(benchmark::State& state) {
    bitcal::bit256 a, b;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 4; ++i) { a[i] = rng(); b[i] = rng(); }

    for (auto _ : state) {
        benchmark::DoNotOptimize(a & b);
    }
}
BENCHMARK(BM_Bit256_AND);

static void BM_Bit256_XOR(benchmark::State& state) {
    bitcal::bit256 a, b;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 4; ++i) { a[i] = rng(); b[i] = rng(); }

    for (auto _ : state) {
        benchmark::DoNotOptimize(a ^ b);
    }
}
BENCHMARK(BM_Bit256_XOR);

static void BM_Bit256_ShiftLeft(benchmark::State& state) {
    bitcal::bit256 a;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 4; ++i) a[i] = rng();

    for (auto _ : state) {
        bitcal::bit256 t = a;
        t <<= 10;
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_Bit256_ShiftLeft);

static void BM_Bit256_Popcount(benchmark::State& state) {
    bitcal::bit256 a;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 4; ++i) a[i] = rng();

    for (auto _ : state) {
        benchmark::DoNotOptimize(a.popcount());
    }
}
BENCHMARK(BM_Bit256_Popcount);

static void BM_Bit256_is_zero(benchmark::State& state) {
    bitcal::bit256 a;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 4; ++i) a[i] = rng();

    for (auto _ : state) {
        benchmark::DoNotOptimize(a.is_zero());
    }
}
BENCHMARK(BM_Bit256_is_zero);

// 512-bit benchmarks
static void BM_Bit512_AND(benchmark::State& state) {
    bitcal::bit512 a, b;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 8; ++i) { a[i] = rng(); b[i] = rng(); }

    for (auto _ : state) {
        benchmark::DoNotOptimize(a & b);
    }
}
BENCHMARK(BM_Bit512_AND);

static void BM_Bit512_XOR(benchmark::State& state) {
    bitcal::bit512 a, b;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 8; ++i) { a[i] = rng(); b[i] = rng(); }

    for (auto _ : state) {
        benchmark::DoNotOptimize(a ^ b);
    }
}
BENCHMARK(BM_Bit512_XOR);

static void BM_Bit512_ShiftLeft(benchmark::State& state) {
    bitcal::bit512 a;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 8; ++i) a[i] = rng();

    for (auto _ : state) {
        bitcal::bit512 t = a;
        t <<= 10;
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_Bit512_ShiftLeft);

// 1024-bit benchmarks
static void BM_Bit1024_AND(benchmark::State& state) {
    bitcal::bit1024 a, b;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 16; ++i) { a[i] = rng(); b[i] = rng(); }

    for (auto _ : state) {
        benchmark::DoNotOptimize(a & b);
    }
}
BENCHMARK(BM_Bit1024_AND);

static void BM_Bit1024_XOR(benchmark::State& state) {
    bitcal::bit1024 a, b;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 16; ++i) { a[i] = rng(); b[i] = rng(); }

    for (auto _ : state) {
        benchmark::DoNotOptimize(a ^ b);
    }
}
BENCHMARK(BM_Bit1024_XOR);

static void BM_Bit1024_ShiftLeft(benchmark::State& state) {
    bitcal::bit1024 a;
    std::mt19937_64 rng(42);
    for (size_t i = 0; i < 16; ++i) a[i] = rng();

    for (auto _ : state) {
        bitcal::bit1024 t = a;
        t <<= 10;
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_Bit1024_ShiftLeft);

BENCHMARK_MAIN();

#endif // BITCAL_HAS_GBENCH
