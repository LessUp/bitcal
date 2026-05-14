#include <chrono>
#include <iostream>

#include <bitcal/bitcal.hpp>

template <typename Func>
double benchmark(const char* name, Func&& func, const std::size_t iterations = 1000000) {
    for (std::size_t i = 0; i < iterations / 10; ++i) {
        func();
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        func();
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto total = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    const auto ns_per_op = static_cast<double>(total) / static_cast<double>(iterations);

    std::cout << name << ": " << ns_per_op << " ns/op" << std::endl;
    return ns_per_op;
}

int main() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    std::cout << "=== BitCal vNext performance smoke ===" << std::endl;
    benchmark("bit_and<256>", [&]() {
        auto out = bitcal::bit_and<256>(lhs.view(), rhs.view());
        (void)out;
    });

    std::cout << "default backend: ";
    switch (bitcal::default_backend()) {
        case bitcal::backend_kind::scalar:
            std::cout << "scalar";
            break;
        case bitcal::backend_kind::sse2:
            std::cout << "sse2";
            break;
        case bitcal::backend_kind::avx2:
            std::cout << "avx2";
            break;
        case bitcal::backend_kind::avx512:
            std::cout << "avx512";
            break;
    }
    std::cout << std::endl;

    return 0;
}
