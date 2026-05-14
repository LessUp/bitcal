#pragma once

#include <cstddef>
#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
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

namespace bitcal {

enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512,
};

[[nodiscard]] constexpr backend_kind default_backend() noexcept {
#if BITCAL_ARCH_X86
#if defined(__AVX512F__)
    return backend_kind::avx512;
#elif defined(__AVX2__)
    return backend_kind::avx2;
#elif defined(__SSE2__) || defined(_M_X64)
    return backend_kind::sse2;
#else
    return backend_kind::scalar;
#endif
#else
    return backend_kind::scalar;
#endif
}

}  // namespace bitcal
