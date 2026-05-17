/**
 * @file benchmark_compare.cpp
 * @brief BitCal vs std::bitset performance comparison benchmark
 *
 * Compares BitCal (bit256, bit512, bit1024) against std::bitset
 * for core operations: AND, OR, XOR, popcount, shift.
 *
 * Compile: g++ -O3 -march=native -std=c++23 benchmark_compare.cpp -I../../include
 */

#include <bitcal/bitcal.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <random>
#include <bitset>
#include <array>
#include <cstring>

namespace {

// Simple high-resolution timer
class Timer {
public:
    using clock = std::chrono::high_resolution_clock;

    void start() { start_ = clock::now(); }

    [[nodiscard]] double elapsed_ns() const {
        const auto end = clock::now();
        return std::chrono::duration<double, std::nano>(end - start_).count();
    }

private:
    clock::time_point start_{};
};

// Prevent compiler from optimizing away results
template <typename T>
void do_not_optimize(T&& value) {
    asm volatile("" : : "r,m"(value) : "memory");
}

// Fill BitCal array with random data
template <std::size_t Bits>
void fill_random(bitcal::bitarray<Bits>& arr, std::mt19937_64& rng) {
    for (std::size_t i = 0; i < bitcal::bitarray<Bits>::u64_count; ++i) {
        arr.set_word(i, rng());
    }
}

// Fill std::bitset with random data
template <std::size_t Bits>
void fill_random(std::bitset<Bits>& bs, std::mt19937_64& rng) {
    bs.reset();
    for (std::size_t i = 0; i < Bits; i += 64) {
        std::uint64_t word = rng();
        for (std::size_t j = 0; j < 64 && (i + j) < Bits; ++j) {
            if ((word >> j) & 1) {
                bs.set(i + j);
            }
        }
    }
}

// Benchmark configuration
constexpr int WARMUP_ITERATIONS = 100;
constexpr int BENCHMARK_ITERATIONS = 100000;

// Run benchmark and return average time in nanoseconds
template <typename Func>
double benchmark(Func&& func, int iterations = BENCHMARK_ITERATIONS) {
    Timer timer;

    // Warmup
    for (int i = 0; i < WARMUP_ITERATIONS; ++i) {
        func();
    }

    // Actual benchmark
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        func();
    }

    return timer.elapsed_ns() / static_cast<double>(iterations);
}

// Print table header
void print_header() {
    std::cout << "\n";
    std::cout << "| Operation           | BitCal (ns) | std::bitset (ns) | Ratio   |\n";
    std::cout << "|---------------------|-------------|------------------|---------|\n";
}

// Print result row
void print_row(const char* op, double bitcal_ns, double std_bitset_ns) {
    double ratio = std_bitset_ns / bitcal_ns;
    std::cout << "| " << std::left << std::setw(20) << op
              << "| " << std::right << std::setw(11) << std::fixed << std::setprecision(2) << bitcal_ns
              << " | " << std::setw(16) << std_bitset_ns
              << " | " << std::setw(7) << std::setprecision(2) << ratio << "x |\n";
}

// Print backend info
void print_backend() {
    std::cout << "BitCal backend: ";
    switch (bitcal::default_backend()) {
        case bitcal::backend_kind::scalar:
            std::cout << "scalar";
            break;
        case bitcal::backend_kind::sse2:
            std::cout << "SSE2";
            break;
        case bitcal::backend_kind::avx2:
            std::cout << "AVX2";
            break;
        case bitcal::backend_kind::avx512:
            std::cout << "AVX-512";
            break;
    }
    std::cout << "\n";
}

//------------------------------------------------------------------------------
// Benchmark templates for each size
//------------------------------------------------------------------------------

template <std::size_t Bits>
void run_benchmarks(std::mt19937_64& rng) {
    // Prepare test data
    bitcal::bitarray<Bits> bitcal_a, bitcal_b;
    std::bitset<Bits> std_a, std_b;

    fill_random(bitcal_a, rng);
    fill_random(bitcal_b, rng);
    fill_random(std_a, rng);
    fill_random(std_b, rng);

    // Results storage
    std::uint64_t bitcal_result_popcount = 0;
    bitcal::bitarray<Bits> bitcal_result;
    std::bitset<Bits> std_result;

    std::cout << "\n=== " << Bits << "-bit operations ===\n";
    print_header();

    // AND operation
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result = bitcal_a & bitcal_b;
            do_not_optimize(bitcal_result);
        });

        double std_ns = benchmark([&]() {
            std_result = std_a & std_b;
            do_not_optimize(std_result);
        });

        std::string op_name = "and<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // OR operation
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result = bitcal_a | bitcal_b;
            do_not_optimize(bitcal_result);
        });

        double std_ns = benchmark([&]() {
            std_result = std_a | std_b;
            do_not_optimize(std_result);
        });

        std::string op_name = "or<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // XOR operation
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result = bitcal_a ^ bitcal_b;
            do_not_optimize(bitcal_result);
        });

        double std_ns = benchmark([&]() {
            std_result = std_a ^ std_b;
            do_not_optimize(std_result);
        });

        std::string op_name = "xor<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // NOT operation
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result = ~bitcal_a;
            do_not_optimize(bitcal_result);
        });

        double std_ns = benchmark([&]() {
            std_result = ~std_a;
            do_not_optimize(std_result);
        });

        std::string op_name = "not<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // popcount operation
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result_popcount = bitcal_a.popcount();
            do_not_optimize(bitcal_result_popcount);
        });

        double std_ns = benchmark([&]() {
            bitcal_result_popcount = std_a.count();
            do_not_optimize(bitcal_result_popcount);
        });

        std::string op_name = "popcount<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // Left shift (by 8)
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result = bitcal_a << 8;
            do_not_optimize(bitcal_result);
        });

        double std_ns = benchmark([&]() {
            std_result = std_a << 8;
            do_not_optimize(std_result);
        });

        std::string op_name = "shift_left<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // Right shift (by 8)
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result = bitcal_a >> 8;
            do_not_optimize(bitcal_result);
        });

        double std_ns = benchmark([&]() {
            std_result = std_a >> 8;
            do_not_optimize(std_result);
        });

        std::string op_name = "shift_right<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // AND-NOT operation (BitCal specific optimization)
    {
        double bitcal_ns = benchmark([&]() {
            bitcal_result = bitcal_a.andnot(bitcal_b);
            do_not_optimize(bitcal_result);
        });

        // std::bitset equivalent: a & ~b
        double std_ns = benchmark([&]() {
            std_result = std_a & ~std_b;
            do_not_optimize(std_result);
        });

        std::string op_name = "andnot<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // is_zero / none operation
    {
        bool is_zero_result = false;

        double bitcal_ns = benchmark([&]() {
            is_zero_result = bitcal_a.is_zero();
            do_not_optimize(is_zero_result);
        });

        double std_ns = benchmark([&]() {
            is_zero_result = std_a.none();
            do_not_optimize(is_zero_result);
        });

        std::string op_name = "is_zero<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }

    // all() operation (check if all bits are set)
    {
        bool all_result = false;

        double bitcal_ns = benchmark([&]() {
            // BitCal: popcount == Bits
            all_result = (bitcal_a.popcount() == Bits);
            do_not_optimize(all_result);
        });

        double std_ns = benchmark([&]() {
            all_result = std_a.all();
            do_not_optimize(all_result);
        });

        std::string op_name = "all_set<" + std::to_string(Bits) + ">";
        print_row(op_name.c_str(), bitcal_ns, std_ns);
    }
}

}  // namespace

int main() {
    std::cout << "=== BitCal vs std::bitset Performance Comparison ===\n";
    std::cout << "Iterations: " << BENCHMARK_ITERATIONS << " (warmup: " << WARMUP_ITERATIONS << ")\n";
    print_backend();

    std::mt19937_64 rng(42);  // Fixed seed for reproducibility

    // Run benchmarks for different sizes
    run_benchmarks<256>(rng);
    run_benchmarks<512>(rng);
    run_benchmarks<1024>(rng);

    std::cout << "\n";
    return 0;
}
