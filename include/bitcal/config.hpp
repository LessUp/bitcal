#pragma once

#include <cstddef>
#include <cstdint>

// x86-64 only. 32-bit x86 (i386/_M_IX86) is excluded: AVX2 intrinsics are
// not reliably available there and the project is x86-64-first.
#if defined(__x86_64__) || defined(_M_X64)
#define BITCAL_ARCH_X86 1
#else
#define BITCAL_ARCH_X86 0
#endif

#if defined(__GNUC__) || defined(__clang__)
#define BITCAL_FORCEINLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define BITCAL_FORCEINLINE __forceinline
#else
#define BITCAL_FORCEINLINE inline
#endif

#define BITCAL_VERSION_MAJOR 4
#define BITCAL_VERSION_MINOR 0
#define BITCAL_VERSION_PATCH 0
#define BITCAL_VERSION ((BITCAL_VERSION_MAJOR << 16) | (BITCAL_VERSION_MINOR << 8) | BITCAL_VERSION_PATCH)

#if BITCAL_ARCH_X86 && defined(__AVX2__)
#define BITCAL_HAS_AVX2 1
#else
#define BITCAL_HAS_AVX2 0
#endif

// <immintrin.h> is only needed on the AVX2 path. Pulling it into every TU on
// plain x86-64 slows compilation for no benefit; gate it on BITCAL_HAS_AVX2.
#if BITCAL_HAS_AVX2
#include <immintrin.h>
#endif

namespace bitcal {

// Optimal alignment based on bit width. Only widths >= 256 bits AND an
// AVX2-capable build benefit from 32-byte alignment (the AVX2 dispatch path
// loads/stores __m256i). Scalar-only builds (BITCAL_HAS_AVX2 == 0) and smaller
// widths use natural alignment: this avoids the compiler emitting wide zero-
// initialization stores that cannot be eliminated as dead code.
template <size_t Bits>
constexpr size_t get_optimal_alignment() noexcept {
    if constexpr (Bits >= 256 && BITCAL_HAS_AVX2)
        return 32;  // 256-bit+ on AVX2: 32-byte alignment
    else
        return alignof(std::uint64_t);  // Scalar path: natural alignment
}

// Compile-time backend name. There is no runtime backend selection: the
// active backend is fixed at compile time by BITCAL_HAS_AVX2. Exposed as a
// string so benchmarks/reports can record which path compiled in, without
// pretending the backend is a runtime-selectable enum.
#if BITCAL_HAS_AVX2
inline constexpr const char* active_backend_name = "avx2";
#else
inline constexpr const char* active_backend_name = "scalar";
#endif

}  // namespace bitcal
