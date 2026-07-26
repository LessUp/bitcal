#pragma once

#include <cstddef>
#include <cstdint>

// x86-64 only, GCC/Clang. 32-bit x86 is excluded: AVX2 intrinsics are not
// reliably available there and the project is x86-64-first.
#if defined(__x86_64__)
#define BITCAL_ARCH_X86 1
#else
#define BITCAL_ARCH_X86 0
#endif

#if defined(__GNUC__) || defined(__clang__)
#define BITCAL_FORCEINLINE __attribute__((always_inline)) inline
#else
#define BITCAL_FORCEINLINE inline
#endif

#define BITCAL_VERSION_MAJOR 4
#define BITCAL_VERSION_MINOR 1
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

// Storage alignment for bit_block. Widths >= 256 on an AVX2 build use 32-byte
// alignment; smaller widths and scalar-only builds use natural uint64 alignment
// (which avoids the compiler emitting wide zero-init stores it cannot then
// eliminate as dead code).
//
// The 32-byte alignment is a predictable guarantee on the block's own storage,
// not a correctness requirement of the AVX2 path: the dispatch kernels use
// unaligned _mm256_loadu/storeu, so they also operate on externally-owned,
// naturally-aligned storage accessed via bit_view.
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
