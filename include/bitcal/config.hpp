#pragma once

#include <cstddef>
#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define BITCAL_ARCH_X86 1
#else
#define BITCAL_ARCH_X86 0
#endif

#if BITCAL_ARCH_X86
#include <immintrin.h>
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

namespace bitcal {

enum class backend_kind {
    scalar,
    avx2,
};

// Optimal alignment based on bit width.
// Only widths >= 256 bits (4+ words) use the AVX2 path and benefit from
// 32-byte alignment. Smaller widths use the scalar fast path and are
// better served by natural alignment, which avoids the compiler emitting
// wide zero-initialization stores that cannot be eliminated as dead code.
template <size_t Bits>
constexpr size_t get_optimal_alignment() noexcept {
    if constexpr (Bits >= 256)
        return 32;      // 256-bit+: 32-byte alignment (AVX2 path)
    else
        return alignof(std::uint64_t);  // Scalar path: natural alignment
}

[[nodiscard]] constexpr backend_kind default_backend() noexcept {
#if BITCAL_HAS_AVX2
    return backend_kind::avx2;
#else
    return backend_kind::scalar;
#endif
}

}  // namespace bitcal
