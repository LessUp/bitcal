#include "benchmark_harness.hpp"

#include <cstdint>

#include <array>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

#include <bitcal/bitcal.hpp>

namespace {

constexpr std::size_t kWarmupIterations = 100;
constexpr std::size_t kSamples = 25;
constexpr std::size_t kIterationsPerSample = 5000;
constexpr int kShiftCount = 65;

template <std::size_t Bits>
using word_array = std::array<std::uint64_t, Bits / 64>;

void print_header() {
    std::cout << "\n";
    std::cout << "| Operation           | BitCal min (ns) | std::bitset min (ns) | Ratio   |\n";
    std::cout << "|---------------------|-----------------|----------------------|---------|\n";
}

void print_row(const std::string& op, const bitcal::bench::sample_summary& bitcal_summary,
               const bitcal::bench::sample_summary& std_summary) {
    const auto ratio = bitcal_summary.min_ns == 0.0 ? 0.0 : (std_summary.min_ns / bitcal_summary.min_ns);
    std::cout << "| " << std::left << std::setw(20) << op << "| " << std::right << std::setw(17) << std::fixed
              << std::setprecision(2) << bitcal_summary.min_ns << " | " << std::setw(22) << std_summary.min_ns << " | "
              << std::setw(7) << std::setprecision(2) << ratio << "x |\n";
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
    return bitcal::bit_block<Bits>::from_words(words);
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

    auto append_row = [&](const char* op, auto bitcal_fn, auto std_fn) {
        bitcal::bench::comparison_row row{};
        row.operation = op;
        row.bits = Bits;
        row.bitcal = bitcal::bench::measure_ns(bitcal_fn, kWarmupIterations, kSamples, kIterationsPerSample);
        row.std_bitset = bitcal::bench::measure_ns(std_fn, kWarmupIterations, kSamples, kIterationsPerSample);
        report.scenarios.push_back(row);
    };

    append_row(
        "bit_and",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            bitcal::bench::do_not_optimize(rhs_block);
            const auto out = bitcal::bit_and(lhs_block, rhs_block);
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            bitcal::bench::do_not_optimize(rhs_bitset);
            const auto out = lhs_bitset & rhs_bitset;
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "bit_or",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            bitcal::bench::do_not_optimize(rhs_block);
            const auto out = bitcal::bit_or(lhs_block, rhs_block);
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            bitcal::bench::do_not_optimize(rhs_bitset);
            const auto out = lhs_bitset | rhs_bitset;
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "bit_xor",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            bitcal::bench::do_not_optimize(rhs_block);
            const auto out = bitcal::bit_xor(lhs_block, rhs_block);
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            bitcal::bench::do_not_optimize(rhs_bitset);
            const auto out = lhs_bitset ^ rhs_bitset;
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "bit_andnot",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            bitcal::bench::do_not_optimize(rhs_block);
            const auto out = bitcal::bit_andnot(lhs_block, rhs_block);
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            bitcal::bench::do_not_optimize(rhs_bitset);
            const auto out = lhs_bitset & ~rhs_bitset;
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "popcount",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            const auto out = bitcal::popcount(lhs_block.view());
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            const auto out = lhs_bitset.count();
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "equals",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            const auto out = bitcal::equals(lhs_block.view(), lhs_block.view());
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            const auto out = (lhs_bitset == lhs_bitset);
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "is_zero",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            const auto out = bitcal::is_zero(lhs_block.view());
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            const auto out = lhs_bitset.none();
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "shift_left",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            const auto out = bitcal::shift_left(lhs_block, kShiftCount);
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            const auto out = lhs_bitset << kShiftCount;
            bitcal::bench::do_not_optimize(out);
        });

    append_row(
        "shift_right",
        [&] {
            bitcal::bench::do_not_optimize(lhs_block);
            const auto out = bitcal::shift_right(lhs_block, kShiftCount);
            bitcal::bench::do_not_optimize(out);
        },
        [&] {
            bitcal::bench::do_not_optimize(lhs_bitset);
            const auto out = lhs_bitset >> kShiftCount;
            bitcal::bench::do_not_optimize(out);
        });
}

void print_report(const bitcal::bench::benchmark_report& report) {
    std::cout << "=== BitCal vs std::bitset comparison ===\n";
    std::cout << "Profile: " << report.metadata.profile << "\n";
    std::cout << "Warmup: " << report.metadata.warmup_iterations << ", samples: " << report.metadata.samples
              << ", iterations/sample: " << report.metadata.iterations_per_sample << "\n";
    std::cout << "Backend: " << report.environment.backend << "\n";

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
    report.metadata.profile = std::string("retained-") + bitcal::bench::active_backend_name();
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
