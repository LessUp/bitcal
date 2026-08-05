#include "benchmark_harness.hpp"

#include <cstdint>

#include <iomanip>
#include <iostream>
#include <random>

#include <bitcal/bitcal.hpp>

namespace {

// Same measurement profile as benchmark_compare so the baseline numbers are
// directly comparable across the two executables.
constexpr std::size_t kWarmupIterations = 100;
constexpr std::size_t kSamples = 25;
constexpr std::size_t kIterationsPerSample = 5000;

template <std::size_t Bits>
void fill_random(bitcal::bit_block<Bits>& block, std::mt19937_64& rng) {
    auto view = block.view();
    auto* words = view.data();

    for (std::size_t i = 0; i < bitcal::bit_block<Bits>::word_count; ++i) {
        words[i] = rng();
    }
}

template <std::size_t Bits>
void run_bit_and_benchmark(const char* label, std::mt19937_64& rng) {
    bitcal::bit_block<Bits> lhs;
    bitcal::bit_block<Bits> rhs;
    fill_random(lhs, rng);
    fill_random(rhs, rng);

    const auto summary = bitcal::bench::measure_ns(
        [&]() {
            auto out = bitcal::bit_and(lhs, rhs);
            bitcal::bench::do_not_optimize(out);
        },
        kWarmupIterations, kSamples, kIterationsPerSample);

    std::cout << label << ": min " << std::fixed << std::setprecision(2) << summary.min_ns << " ns/op (median "
              << summary.median_ns << ", cv " << summary.cv << ")" << std::endl;
}

void run_all_benchmarks() {
    std::mt19937_64 rng(42);

    std::cout << "=== BitCal benchmark baseline ===" << std::endl;
    std::cout << "profile: warmup " << kWarmupIterations << ", " << kSamples << " samples x " << kIterationsPerSample
              << " iterations" << std::endl;
    run_bit_and_benchmark<128>("bit_and<128>", rng);
    run_bit_and_benchmark<256>("bit_and<256>", rng);
    run_bit_and_benchmark<512>("bit_and<512>", rng);
    std::cout << "default backend: " << bitcal::bench::active_backend_name() << std::endl;
}

}  // namespace

int main() {
    run_all_benchmarks();
    return 0;
}
