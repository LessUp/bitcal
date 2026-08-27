#pragma once

#include <cmath>
#include <cstdint>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include <bitcal/bitcal.hpp>

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
    std::string compiler = __VERSION__;
    std::string backend;
    std::string cpu;
};

struct run_metadata {
    std::string profile;
    std::size_t warmup_iterations = 0;
    std::size_t samples = 0;
    std::size_t iterations_per_sample = 0;
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

inline const char* active_backend_name() noexcept {
    return bitcal::active_backend_name;
}

inline std::string detect_cpu_model() {
#if defined(__linux__)
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        constexpr std::string_view prefix = "model name\t: ";
        if (line.rfind(prefix.data(), 0) == 0) {
            return line.substr(prefix.size());
        }
    }
#endif

    return "unknown";
}

template <typename Func>
sample_summary measure_ns(Func&& func, const std::size_t warmup, const std::size_t samples,
                          const std::size_t iterations_per_sample) {
    // steady_clock：libstdc++ 的 high_resolution_clock 是 system_clock 别名（非单调），计时须单调。
    using clock = std::chrono::steady_clock;

    for (std::size_t i = 0; i < warmup; ++i) {
        func();
    }

    std::vector<double> sample_values;
    sample_values.reserve(samples);

    for (std::size_t sample = 0; sample < samples; ++sample) {
        const auto start = clock::now();
        for (std::size_t iteration = 0; iteration < iterations_per_sample; ++iteration) {
            func();
        }
        const auto end = clock::now();
        sample_values.push_back(std::chrono::duration<double, std::nano>(end - start).count() /
                                static_cast<double>(iterations_per_sample));
    }

    sample_summary summary{};
    summary.samples = samples;
    summary.iterations_per_sample = iterations_per_sample;
    summary.min_ns = *std::min_element(sample_values.begin(), sample_values.end());
    summary.max_ns = *std::max_element(sample_values.begin(), sample_values.end());

    double total = 0.0;
    for (const auto value : sample_values) {
        total += value;
    }
    summary.mean_ns = total / static_cast<double>(sample_values.size());

    std::sort(sample_values.begin(), sample_values.end());
    if (sample_values.size() % 2 == 0) {
        const auto upper = sample_values[sample_values.size() / 2];
        const auto lower = sample_values[(sample_values.size() / 2) - 1];
        summary.median_ns = (lower + upper) / 2.0;
    } else {
        summary.median_ns = sample_values[sample_values.size() / 2];
    }

    double variance_sum = 0.0;
    for (const auto value : sample_values) {
        const auto delta = value - summary.mean_ns;
        variance_sum += delta * delta;
    }

    summary.stdev_ns = std::sqrt(variance_sum / static_cast<double>(sample_values.size()));
    summary.cv = summary.mean_ns == 0.0 ? 0.0 : (summary.stdev_ns / summary.mean_ns);
    return summary;
}

inline std::string json_escape(const std::string_view input) {
    std::string out;
    out.reserve(input.size());

    for (const auto ch : input) {
        switch (ch) {
            case '\\':
                out += "\\\\";
                break;
            case '"':
                out += "\\\"";
                break;
            case '\n':
                out += "\\n";
                break;
            default:
                out += ch;
                break;
        }
    }

    return out;
}

inline void write_json_report(std::ostream& out, const benchmark_report& report) {
    out << "{\n";
    out << "  \"metadata\": {\n";
    out << "    \"profile\": \"" << json_escape(report.metadata.profile) << "\",\n";
    out << "    \"warmup_iterations\": " << report.metadata.warmup_iterations << ",\n";
    out << "    \"samples\": " << report.metadata.samples << ",\n";
    out << "    \"iterations_per_sample\": " << report.metadata.iterations_per_sample << "\n";
    out << "  },\n";
    out << "  \"environment\": {\n";
    out << "    \"compiler\": \"" << json_escape(report.environment.compiler) << "\",\n";
    out << "    \"backend\": \"" << json_escape(report.environment.backend) << "\",\n";
    out << "    \"cpu\": \"" << json_escape(report.environment.cpu) << "\"\n";
    out << "  },\n";
    out << "  \"scenarios\": [\n";

    for (std::size_t i = 0; i < report.scenarios.size(); ++i) {
        const auto& row = report.scenarios[i];
        out << "    {\n";
        out << "      \"operation\": \"" << json_escape(row.operation) << "\",\n";
        out << "      \"bits\": " << row.bits << ",\n";
        out << "      \"bitcal\": {\n";
        out << "        \"samples\": " << row.bitcal.samples << ",\n";
        out << "        \"iterations_per_sample\": " << row.bitcal.iterations_per_sample << ",\n";
        out << "        \"min_ns\": " << row.bitcal.min_ns << ",\n";
        out << "        \"max_ns\": " << row.bitcal.max_ns << ",\n";
        out << "        \"mean_ns\": " << row.bitcal.mean_ns << ",\n";
        out << "        \"median_ns\": " << row.bitcal.median_ns << ",\n";
        out << "        \"stdev_ns\": " << row.bitcal.stdev_ns << ",\n";
        out << "        \"cv\": " << row.bitcal.cv << "\n";
        out << "      },\n";
        out << "      \"std_bitset\": {\n";
        out << "        \"samples\": " << row.std_bitset.samples << ",\n";
        out << "        \"iterations_per_sample\": " << row.std_bitset.iterations_per_sample << ",\n";
        out << "        \"min_ns\": " << row.std_bitset.min_ns << ",\n";
        out << "        \"max_ns\": " << row.std_bitset.max_ns << ",\n";
        out << "        \"mean_ns\": " << row.std_bitset.mean_ns << ",\n";
        out << "        \"median_ns\": " << row.std_bitset.median_ns << ",\n";
        out << "        \"stdev_ns\": " << row.std_bitset.stdev_ns << ",\n";
        out << "        \"cv\": " << row.std_bitset.cv << "\n";
        out << "      }\n";
        out << "    }";
        if (i + 1 != report.scenarios.size()) {
            out << ",";
        }
        out << "\n";
    }

    out << "  ]\n";
    out << "}\n";
}

}  // namespace bitcal::bench
