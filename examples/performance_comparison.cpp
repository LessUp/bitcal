#include <bitcal/bitcal.hpp>
#include <iostream>
#include <chrono>
#include <cstring>

template<typename T>
BITCAL_FORCEINLINE void do_not_optimize(T const& value) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "r,m"(value) : "memory");
#else
    volatile auto sink = value;
    (void)sink;
#endif
}

template<typename Func>
double benchmark(const char* name, Func&& func, size_t iterations = 1000000) {
    // warmup
    for (size_t i = 0; i < iterations / 10; ++i) {
        func();
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double ns_per_op = static_cast<double>(duration) / iterations;
    
    std::cout << name << ": " << ns_per_op << " ns/op" << std::endl;
    return ns_per_op;
}

int main() {
    std::cout << "=== BitCal 性能对比 ===" << std::endl << std::endl;
    
    const size_t iterations = 1000000;
    
    std::cout << "64位操作:" << std::endl;
    {
        bitcal::bit64 a(0xDEADBEEFCAFEBABE);
        bitcal::bit64 b(0x0123456789ABCDEF);
        
        benchmark("bit64 AND", [&]() {
            do_not_optimize(a & b);
        }, iterations);
        
        benchmark("bit64 XOR", [&]() {
            do_not_optimize(a ^ b);
        }, iterations);
        
        benchmark("bit64 ANDNOT", [&]() {
            do_not_optimize(a.andnot(b));
        }, iterations);
        
        benchmark("bit64 shift_left", [&]() {
            auto tmp = a;
            tmp.shift_left(10);
            do_not_optimize(tmp);
        }, iterations);
        
        benchmark("bit64 popcount", [&]() {
            do_not_optimize(a.popcount());
        }, iterations);
    }
    
    std::cout << std::endl << "128位操作:" << std::endl;
    {
        bitcal::bit128 a(0xDEADBEEFCAFEBABE);
        bitcal::bit128 b(0x0123456789ABCDEF);
        
        benchmark("bit128 AND", [&]() {
            do_not_optimize(a & b);
        }, iterations);
        
        benchmark("bit128 XOR", [&]() {
            do_not_optimize(a ^ b);
        }, iterations);
        
        benchmark("bit128 ANDNOT", [&]() {
            do_not_optimize(a.andnot(b));
        }, iterations);
        
        benchmark("bit128 shift_left", [&]() {
            auto tmp = a;
            tmp.shift_left(10);
            do_not_optimize(tmp);
        }, iterations);
        
        benchmark("bit128 popcount", [&]() {
            do_not_optimize(a.popcount());
        }, iterations);
    }
    
    std::cout << std::endl << "256位操作:" << std::endl;
    {
        bitcal::bit256 a(0xDEADBEEFCAFEBABE);
        bitcal::bit256 b(0x0123456789ABCDEF);
        
        benchmark("bit256 AND", [&]() {
            do_not_optimize(a & b);
        }, iterations);
        
        benchmark("bit256 XOR", [&]() {
            do_not_optimize(a ^ b);
        }, iterations);
        
        benchmark("bit256 ANDNOT", [&]() {
            do_not_optimize(a.andnot(b));
        }, iterations);
        
        benchmark("bit256 shift_left", [&]() {
            auto tmp = a;
            tmp.shift_left(10);
            do_not_optimize(tmp);
        }, iterations);
        
        benchmark("bit256 popcount", [&]() {
            do_not_optimize(a.popcount());
        }, iterations);
    }
    
    std::cout << std::endl << "512位操作:" << std::endl;
    {
        bitcal::bit512 a(0xDEADBEEFCAFEBABE);
        bitcal::bit512 b(0x0123456789ABCDEF);
        
        benchmark("bit512 AND", [&]() {
            do_not_optimize(a & b);
        }, iterations);
        
        benchmark("bit512 XOR", [&]() {
            do_not_optimize(a ^ b);
        }, iterations);
        
        benchmark("bit512 ANDNOT", [&]() {
            do_not_optimize(a.andnot(b));
        }, iterations);
        
        benchmark("bit512 shift_left", [&]() {
            auto tmp = a;
            tmp.shift_left(10);
            do_not_optimize(tmp);
        }, iterations);
        
        benchmark("bit512 popcount", [&]() {
            do_not_optimize(a.popcount());
        }, iterations);
    }
    
    std::cout << std::endl << "当前使用的SIMD后端: ";
    switch (bitcal::get_default_backend()) {
        case bitcal::simd_backend::scalar:
            std::cout << "scalar (通用)" << std::endl;
            break;
        case bitcal::simd_backend::sse2:
            std::cout << "SSE2" << std::endl;
            break;
        case bitcal::simd_backend::avx:
            std::cout << "AVX" << std::endl;
            break;
        case bitcal::simd_backend::avx2:
            std::cout << "AVX2" << std::endl;
            break;
        case bitcal::simd_backend::avx512:
            std::cout << "AVX-512" << std::endl;
            break;
        case bitcal::simd_backend::neon:
            std::cout << "NEON (ARM)" << std::endl;
            break;
    }
    
    return 0;
}
