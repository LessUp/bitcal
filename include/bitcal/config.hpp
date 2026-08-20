#pragma once

#include <cstddef>
#include <cstdint>

// 仅 x86-64（GCC / Clang）。排除 32 位 x86：AVX2 intrinsics 在其上不可靠，
// 且项目以 x86-64 优先。
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
#define BITCAL_VERSION_MINOR 2
#define BITCAL_VERSION_PATCH 0
#define BITCAL_VERSION ((BITCAL_VERSION_MAJOR << 16) | (BITCAL_VERSION_MINOR << 8) | BITCAL_VERSION_PATCH)

#if BITCAL_ARCH_X86 && defined(__AVX2__)
#define BITCAL_HAS_AVX2 1
#else
#define BITCAL_HAS_AVX2 0
#endif

// <immintrin.h> 仅在 AVX2 路径需要。在普通 x86-64 上引入到每个 TU 只会拖慢
// 编译而无收益；以 BITCAL_HAS_AVX2 门控。
#if BITCAL_HAS_AVX2
#include <immintrin.h>
#endif

namespace bitcal {

// bit_block 的存储对齐。AVX2 构建下 >= 256 位宽度用 32 字节对齐；更小宽度与
// scalar-only 构建用 uint64 自然对齐（避免编译器发出无法作为死代码消除的
// 宽零初始化存储）。
//
// 32 字节对齐是块自身存储的可预期保证，而非 AVX2 路径的正确性前提：分派内核
// 使用 unaligned _mm256_loadu/storeu，因此也能作用于经 bit_view 访问的
// 外部自有、自然对齐的存储。
template <size_t Bits>
constexpr size_t get_optimal_alignment() noexcept {
    if constexpr (Bits >= 256 && BITCAL_HAS_AVX2)
        return 32;  // AVX2 下 256 位及以上：32 字节对齐
    else
        return alignof(std::uint64_t);  // Scalar 路径：自然对齐
}

// 编译期后端名。不存在运行时后端选择：当前后端在编译期由 BITCAL_HAS_AVX2
// 固定。以字符串暴露，供基准/报告记录编译进的是哪条路径，而不假装后端是
// 可在运行时选择的 enum。
#if BITCAL_HAS_AVX2
inline constexpr const char* active_backend_name = "avx2";
#else
inline constexpr const char* active_backend_name = "scalar";
#endif

}  // namespace bitcal
