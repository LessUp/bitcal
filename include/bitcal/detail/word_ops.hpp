#pragma once

#include "../bit_view.hpp"
#include "../config.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <bit>

namespace bitcal::detail {

// --- scalar 内核 ------------------------------------------------------------

template <typename WordOp>
inline void binary_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                               const std::size_t word_count, WordOp&& word_op) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = word_op(lhs[i], rhs[i]);
    }
}

// shift 单遍融合内核：读 `in`、写 `out`，一次扫描完成词移（word_shift）+ 位移（bit_shift）。
// 每个输出字只依赖两个独立输入字，无串行 carry 依赖链；count==0 与 count>=N*64 均由公式
// 天然正确（越界项取 0），无需特判。`out` / `in` 不得重叠（公开 API 的 `out` 总是新建
// bit_block，in/out 永不重叠），`assert(out != in)` 仅作 Debug 前置检查。
template <std::size_t N>
constexpr void shift_left_fused(std::uint64_t* out, const std::uint64_t* in, const std::size_t count) noexcept {
    static_assert(N > 0, "Array size must be positive");
    assert(out != in);

    const std::size_t word_shift = count / 64;
    const std::size_t bit_shift = count % 64;
    for (std::size_t i = 0; i < N; ++i) {
        // 主源词：in[i-word_shift] 左移 bit_shift；i < word_shift 时为 0（低位补零）。
        const std::uint64_t shifted = (i >= word_shift) ? in[i - word_shift] << bit_shift : 0;
        // 进位词：in[i-word_shift-1] 的高 bit_shift 位落到 out[i] 低位；bit_shift==0 短路避免 >>64 UB。
        const std::uint64_t carry = (bit_shift != 0 && i > word_shift) ? in[i - word_shift - 1] >> (64 - bit_shift) : 0;
        out[i] = shifted | carry;
    }
}

template <std::size_t N>
constexpr void shift_right_fused(std::uint64_t* out, const std::uint64_t* in, const std::size_t count) noexcept {
    static_assert(N > 0, "Array size must be positive");
    assert(out != in);

    const std::size_t word_shift = count / 64;
    const std::size_t bit_shift = count % 64;
    for (std::size_t i = 0; i < N; ++i) {
        // 主源词：in[i+word_shift] 右移 bit_shift；i+word_shift >= N 时为 0（高位补零）。
        const std::uint64_t shifted = (i + word_shift < N) ? in[i + word_shift] >> bit_shift : 0;
        // 进位词：in[i+word_shift+1] 的低 bit_shift 位落到 out[i] 高位；bit_shift==0 短路避免 <<64 UB。
        const std::uint64_t carry =
            (bit_shift != 0 && i + word_shift + 1 < N) ? in[i + word_shift + 1] << (64 - bit_shift) : 0;
        out[i] = shifted | carry;
    }
}

// --- x86-64 分派 ------------------------------------------------------------
// `vector_op` 仅在 AVX2 路径被调用；scalar-only 目标上它未被使用，因此调用方
// 可以传引用 AVX2 intrinsics 的 generic lambda 而不破坏非 x86 构建
// （lambda 函数体仅在真正被调用时才实例化）。

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
template <typename VectorOp, typename ScalarWordOp>
BITCAL_FORCEINLINE void binary_into_avx2_or_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs,
                                                   std::uint64_t* out, const std::size_t word_count,
                                                   VectorOp&& vector_op, ScalarWordOp&& scalar_word_op) noexcept {
    std::size_t i = 0;
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        const auto out_vec = vector_op(lhs_vec, rhs_vec);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i), out_vec);
    }

    for (; i < word_count; ++i) {
        out[i] = scalar_word_op(lhs[i], rhs[i]);
    }
}
#endif

template <typename VectorOp, typename WordOp>
BITCAL_FORCEINLINE void binary_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count, VectorOp&& vector_op, WordOp&& word_op) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(lhs, rhs, out, word_count, std::forward<VectorOp>(vector_op),
                               std::forward<WordOp>(word_op));
#else
    (void)vector_op;
    binary_into_scalar(lhs, rhs, out, word_count, std::forward<WordOp>(word_op));
#endif
}

// --- 查询分派（is_zero / equals）---------------------------------------------
// is_zero / equals 复刻二元分派模式：4 字 AVX2 块 + scalar 尾部；下方视图层
// 包装里的 `if consteval` 保留 constexpr scalar 路径以支持编译期求值。popcount
// 不参与 AVX2 分派（见下方 popcount_words）。

[[nodiscard]] BITCAL_FORCEINLINE bool is_zero_x64(const std::uint64_t* data, const std::size_t word_count) noexcept {
    std::size_t i = 0;

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    for (; i + 4 <= word_count; i += 4) {
        const auto value = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        // _mm256_testz_si256 在 (value & value) == 0 时返回 1，即 value 全零。
        if (_mm256_testz_si256(value, value) == 0) {
            return false;
        }
    }
#endif

    for (; i < word_count; ++i) {
        if (data[i] != 0) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] BITCAL_FORCEINLINE bool equals_x64(const std::uint64_t* lhs, const std::uint64_t* rhs,
                                                 const std::size_t word_count) noexcept {
    std::size_t i = 0;

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    const auto all_ones = _mm256_set1_epi32(-1);
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        // _mm256_testc_si256 在 (cmp & all_ones) == all_ones 时返回 1，即所有 lane 相等。
        const auto cmp = _mm256_cmpeq_epi64(lhs_vec, rhs_vec);
        if (_mm256_testc_si256(cmp, all_ones) == 0) {
            return false;
        }
    }
#endif

    for (; i < word_count; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

// --- 视图层 word 操作 --------------------------------------------------------

inline void assert_binary_word_layout(const const_bit_view lhs, const const_bit_view rhs, const bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());
}

template <typename VectorOp, typename WordOp>
inline void binary_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out, VectorOp&& vector_op,
                         WordOp&& word_op) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    binary_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count(), std::forward<VectorOp>(vector_op),
                    std::forward<WordOp>(word_op));
}

[[nodiscard]] constexpr bool is_zero_words(const const_bit_view value) noexcept {
    if consteval {
        for (std::size_t i = 0; i < value.word_count(); ++i) {
            if (value.word(i) != 0) {
                return false;
            }
        }
        return true;
    } else {
        return is_zero_x64(value.data(), value.word_count());
    }
}

// popcount 不走 AVX2 分派：≤ 512 位块上 per-word popcnt 实测快于此前的
// LUT 方案，取舍与实测数据见 NOTES.md「popcount 走 scalar 路径」。
[[nodiscard]] constexpr std::uint64_t popcount_words(const const_bit_view value) noexcept {
    std::uint64_t total = 0;
    for (std::size_t i = 0; i < value.word_count(); ++i) {
        total += static_cast<std::uint64_t>(std::popcount(value.word(i)));
    }
    return total;
}

[[nodiscard]] constexpr bool equals_words(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    if (lhs.word_count() != rhs.word_count()) {
        return false;
    }

    if consteval {
        for (std::size_t i = 0; i < lhs.word_count(); ++i) {
            if (lhs.word(i) != rhs.word(i)) {
                return false;
            }
        }
        return true;
    } else {
        return equals_x64(lhs.data(), rhs.data(), lhs.word_count());
    }
}

}  // namespace bitcal::detail
