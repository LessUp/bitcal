/**
 * @file benchmark_compare.cpp
 * @brief Retained BitCal vNext vs std::bitset comparison benchmark
 */

#include <bitcal/bitcal.hpp>

#include "benchmark_harness.hpp"

#include <array>
#include <bitset>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <span>
#include <string>

namespace {

constexpr std::size_t kWarmupIterations = 100;
constexpr std::size_t kSamples = 25;
constexpr std::size_t kIterationsPerSample = 5000;

template <std::size_t Bits>
using word_array = std::array<std::uint64_t, Bits / 64>;

void print_header() {
    std::cout << "\n";
    std::cout << "| Operation           | BitCal median (ns) | std::bitset median (ns) | Ratio   |\n";
    std::cout << "|---------------------|--------------------|-------------------------|---------|\n";
}

void print_row(const std::string& op, const bitcal::bench::sample_summary& bitcal_summary,
               const bitcal::bench::sample_summary& std_summary) {
    const auto ratio = bitcal_summary.median_ns == 0.0 ? 0.0 : (std_summary.median_ns / bitcal_summary.median_ns);
    std::cout << "| " << std::left << std::setw(20) << op
              << "| " << std::right << std::setw(18) << std::fixed << std::setprecision(2)
              << bitcal_summary.median_ns << " | " << std::setw(23) << std_summary.median_ns
              << " | " << std::setw(7) << std::setprecision(2) << ratio << "x |\n";
}

template <std::size_t Bits>
word_array<Bits> make_random_words(std::mt19937_64& rng) {
    word_array<Bits> words{};
    for (auto& word : words) {
        word = rng();
    }
    return words;
}

template <std::size_t Bits>
bitcal::bit_block<Bits> to_block(const word_array<Bits>& words) {
    return bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(words.data(), words.size()));
}

template <std::size_t Bits>
std::bitset<Bits> to_bitset(const word_array<Bits>& words) {
    std::bitset<Bits> out;

    for (std::size_t base = 0; base < Bits; base += 64) {
        const auto word = words[base / 64];
        for (std::size_t bit = 0; bit < 64 && (base + bit) < Bits; ++bit) {
            if ((word >> bit) & 1ULL) {
                out.set(base + bit);
            }
        }
    }

    return out;
}

template <std::size_t Bits>
void append_retained_cases(bitcal::bench::benchmark_report& report) {
    std::mt19937_64 rng(42 + Bits);
    const auto lhs_words = make_random_words<Bits>(rng);
    const auto rhs_words = make_random_words<Bits>(rng);

    const auto lhs_block = to_block<Bits>(lhs_words);
    const auto rhs_block = to_block<Bits>(rhs_words);
    const auto lhs_bitset = to_bitset<Bits>(lhs_words);
    const auto rhs_bitset = to_bitset<Bits>(rhs_words);

    bitcal::bench::comparison_row and_row{};
    and_row.operation = "bit_and";
    and_row.bits = Bits;
    and_row.bitcal = bitcal::bench::measure_ns([&]() {
            const auto out = bitcal::bit_and<Bits>(lhs_block.view(), rhs_block.view());
            bitcal::bench::do_not_optimize(out);
        }, kWarmupIterations, kSamples, kIterationsPerSample);
    and_row.std_bitset = bitcal::bench::measure_ns([&]() {
            const auto out = lhs_bitset & rhs_bitset;
            bitcal::bench::do_not_optimize(out);
        }, kWarmupIterations, kSamples, kIterationsPerSample);
    report.scenarios.push_back(and_row);

    bitcal::bench::comparison_row popcount_row{};
    popcount_row.operation = "popcount";
    popcount_row.bits = Bits;
    popcount_row.bitcal = bitcal::bench::measure_ns([&]() {
            const auto out = bitcal::popcount(lhs_block.view());
            bitcal::bench::do_not_optimize(out);
        }, kWarmupIterations, kSamples, kIterationsPerSample);
    popcount_row.std_bitset = bitcal::bench::measure_ns([&]() {
            const auto out = lhs_bitset.count();
            bitcal::bench::do_not_optimize(out);
        }, kWarmupIterations, kSamples, kIterationsPerSample);
    report.scenarios.push_back(popcount_row);

    bitcal::bench::comparison_row is_zero_row{};
    is_zero_row.operation = "is_zero";
    is_zero_row.bits = Bits;
    is_zero_row.bitcal = bitcal::bench::measure_ns([&]() {
            const auto out = bitcal::is_zero(lhs_block.view());
            bitcal::bench::do_not_optimize(out);
        }, kWarmupIterations, kSamples, kIterationsPerSample);
    is_zero_row.std_bitset = bitcal::bench::measure_ns([&]() {
            const auto out = lhs_bitset.none();
            bitcal::bench::do_not_optimize(out);
        }, kWarmupIterations, kSamples, kIterationsPerSample);
    report.scenarios.push_back(is_zero_row);
}

void print_report(const bitcal::bench::benchmark_report& report) {
    std::cout << "=== BitCal vNext vs std::bitset Retained Comparison ===\n";
    std::cout << "Profile: " << report.metadata.profile << "\n";
    std::cout << "Warmup: " << report.metadata.warmup_iterations << ", samples: " << report.metadata.samples
              << ", iterations/sample: " << report.metadata.iterations_per_sample << "\n";
    std::cout << "Backend: " << report.environment.backend << "\n";
    std::cout << "Commit: " << report.environment.commit << "\n";

    std::size_t current_bits = 0;
    for (const auto& scenario : report.scenarios) {
        if (scenario.bits != current_bits) {
            current_bits = scenario.bits;
            std::cout << "\n=== " << current_bits << "-bit retained operations ===\n";
            print_header();
        }

        print_row(scenario.operation + "<" + std::to_string(scenario.bits) + ">", scenario.bitcal, scenario.std_bitset);
    }

    std::cout << "\n";
}

}  // namespace

int main(int argc, char** argv) {
    std::string json_out_path;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--json-out") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for --json-out\n";
                return 2;
            }
            json_out_path = argv[++i];
            continue;
        }

        std::cerr << "Unknown argument: " << arg << "\n";
        return 2;
    }

    bitcal::bench::benchmark_report report{};
    report.metadata.profile = std::string("retained-vnext-") + bitcal::bench::active_backend_name();
    report.metadata.warmup_iterations = kWarmupIterations;
    report.metadata.samples = kSamples;
    report.metadata.iterations_per_sample = kIterationsPerSample;
    report.environment.backend = bitcal::bench::active_backend_name();
    report.environment.cpu = bitcal::bench::detect_cpu_model();

    append_retained_cases<128>(report);
    append_retained_cases<192>(report);
    append_retained_cases<256>(report);
    append_retained_cases<512>(report);

    print_report(report);

    if (!json_out_path.empty()) {
        std::ofstream out(json_out_path);
        if (!out) {
            std::cerr << "Failed to open JSON output path: " << json_out_path << "\n";
            return 1;
        }
        bitcal::bench::write_json_report(out, report);
    }

    return 0;
}
