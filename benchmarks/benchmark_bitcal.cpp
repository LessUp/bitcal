#include "benchmark_harness.hpp"

#include <cstdint>

#include <chrono>
#include <iostream>
#include <random>

#include <bitcal/bitcal.hpp>

namespace {

class SimpleTimer {
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

template <std::size_t Bits>
void fill_random(bitcal::bit_block<Bits>& block, std::mt19937_64& rng) {
    auto view = block.view();
    auto* words = view.data();

    for (std::size_t i = 0; i < bitcal::bit_block<Bits>::word_count; ++i) {
        words[i] = rng();
    }
}

template <typename Func>
void run_benchmark(const char* name, Func&& func, const int iterations = 10000) {
    constexpr int warmup_iterations = 100;
    SimpleTimer timer;

    for (int i = 0; i < warmup_iterations; ++i) {
        func();
    }

    timer.start();
    for (int i = 0; i < iterations; ++i) {
        func();
    }

    const auto total_ns = timer.elapsed_ns();
    const auto avg_ns = total_ns / static_cast<double>(iterations);
    std::cout << name << ": " << avg_ns << " ns/op" << std::endl;
}

}  // namespace

template <std::size_t Bits>
void run_bit_and_benchmark(const char* label, std::mt19937_64& rng) {
    bitcal::bit_block<Bits> lhs;
    bitcal::bit_block<Bits> rhs;
    fill_random(lhs, rng);
    fill_random(rhs, rng);

    run_benchmark(label, [&]() {
        auto out = bitcal::bit_and<Bits>(lhs.view(), rhs.view());
        bitcal::bench::do_not_optimize(out);
    });
}

void run_all_benchmarks() {
    std::mt19937_64 rng(42);

    std::cout << "=== BitCal benchmark baseline ===" << std::endl;
    std::cout << "profile: smoke-only" << std::endl;
    run_bit_and_benchmark<128>("bit_and<128>", rng);
    run_bit_and_benchmark<256>("bit_and<256>", rng);
    run_bit_and_benchmark<512>("bit_and<512>", rng);
    std::cout << "default backend: " << bitcal::bench::active_backend_name() << std::endl;
}

int main() {
    run_all_benchmarks();
    return 0;
}
