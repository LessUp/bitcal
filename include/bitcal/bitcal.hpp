#pragma once

#include "config.hpp"
#include "scalar_ops.hpp"

#if BITCAL_HAS_SSE2
#include "sse_ops.hpp"
#endif

#if BITCAL_HAS_AVX2
#include "avx_ops.hpp"
#endif

#if BITCAL_HAS_AVX512
#include "avx512_ops.hpp"
#endif

#if BITCAL_HAS_NEON
#include "neon_ops.hpp"
#endif

#include <cassert>
#include <cstring>
#include <cstdint>
#include <type_traits>
#include <limits>

namespace bitcal {

// bitarray<N, Backend>：固定宽度位数组
//
// - Bits：位宽（例如 64/128/256/512/1024）
// - Backend：编译期选择的 SIMD 后端；默认由 get_default_backend() 自动选择
//
// 该类型为 header-only + 零开销抽象：在开启优化时，编译器会把大多数操作内联到对应的
// 标量/SIMD 实现。

template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray {
    static_assert(Bits >= 64, "Bits must be at least 64");
    static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");

public:
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;

    bitarray() noexcept {
        clear();
    }

    explicit bitarray(uint64_t value) noexcept {
        clear();
        data_[0] = value;
    }

    // Copy and move constructors (explicitly defaulted for clarity with alignas)
    bitarray(const bitarray& other) noexcept = default;
    bitarray(bitarray&& other) noexcept = default;

    // Copy and move assignment operators
    bitarray& operator=(const bitarray& other) noexcept = default;
    bitarray& operator=(bitarray&& other) noexcept = default;

    // 清零所有位。
    BITCAL_FORCEINLINE void clear() noexcept {
        std::memset(data_, 0, sizeof(data_));
    }
    
    // 直接暴露底层的 word 数组（uint64_t），便于与外部算法对接。
    BITCAL_FORCEINLINE uint64_t* data() noexcept { return data_; }
    BITCAL_FORCEINLINE const uint64_t* data() const noexcept { return data_; }
    
    BITCAL_FORCEINLINE uint64_t operator[](size_t index) const noexcept {
        assert(index < u64_count);
        return data_[index];
    }
    
    BITCAL_FORCEINLINE uint64_t& operator[](size_t index) noexcept {
        assert(index < u64_count);
        return data_[index];
    }
    
    // 读取单个位（0..Bits-1）。
    [[nodiscard]] BITCAL_FORCEINLINE bool get_bit(size_t bit_index) const noexcept {
        assert(bit_index < Bits);
        const size_t word_idx = bit_index / 64;
        const size_t bit_offset = bit_index % 64;
        return (data_[word_idx] >> bit_offset) & 1;
    }
    
    // 设置单个位；value=false 表示清除该位。
    BITCAL_FORCEINLINE void set_bit(size_t bit_index, bool value = true) noexcept {
        assert(bit_index < Bits);
        const size_t word_idx = bit_index / 64;
        const size_t bit_offset = bit_index % 64;
        if (value) {
            data_[word_idx] |= (1ULL << bit_offset);
        } else {
            data_[word_idx] &= ~(1ULL << bit_offset);
        }
    }
    
    // 翻转单个位。
    BITCAL_FORCEINLINE void flip_bit(size_t bit_index) noexcept {
        assert(bit_index < Bits);
        const size_t word_idx = bit_index / 64;
        const size_t bit_offset = bit_index % 64;
        data_[word_idx] ^= (1ULL << bit_offset);
    }
    
    // 左移 count 位（超出位宽的部分被丢弃）。
    BITCAL_FORCEINLINE void shift_left(int count) noexcept {
        if constexpr (Bits == 64) {
            data_[0] = scalar::shift_left(data_[0], count);
        }
#if BITCAL_HAS_AVX512
        else if constexpr (Backend == simd_backend::avx512 && Bits == 256) {
            avx512::shift_left_256_vl(data_, count);
        }
        else if constexpr (Backend == simd_backend::avx512 && Bits == 512) {
            avx512::shift_left_512(data_, count);
        }
#endif
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            neon::shift_left_128(data_, count);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            neon::shift_left_256(data_, count);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            neon::shift_left_512(data_, count);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            sse::shift_left_128(data_, count);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            sse::shift_left_256(data_, count);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            avx::shift_left_256(data_, count);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            avx::shift_left_512(data_, count);
        }
#endif
        else {
            scalar::shift_left_array<u64_count>(data_, count);
        }
    }
    
    // 右移 count 位（超出位宽的部分被丢弃）。
    BITCAL_FORCEINLINE void shift_right(int count) noexcept {
        if constexpr (Bits == 64) {
            data_[0] = scalar::shift_right(data_[0], count);
        }
#if BITCAL_HAS_AVX512
        else if constexpr (Backend == simd_backend::avx512 && Bits == 256) {
            avx512::shift_right_256_vl(data_, count);
        }
        else if constexpr (Backend == simd_backend::avx512 && Bits == 512) {
            avx512::shift_right_512(data_, count);
        }
#endif
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            neon::shift_right_128(data_, count);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            neon::shift_right_256(data_, count);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            neon::shift_right_512(data_, count);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            sse::shift_right_128(data_, count);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            sse::shift_right_256(data_, count);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            avx::shift_right_256(data_, count);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            avx::shift_right_512(data_, count);
        }
#endif
        else {
            scalar::shift_right_array<u64_count>(data_, count);
        }
    }
    
    BITCAL_FORCEINLINE bitarray operator&(const bitarray& other) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_and>(*this, other, result);
        return result;
    }

    BITCAL_FORCEINLINE bitarray operator|(const bitarray& other) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_or>(*this, other, result);
        return result;
    }

    BITCAL_FORCEINLINE bitarray operator^(const bitarray& other) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_xor>(*this, other, result);
        return result;
    }

    // ANDNOT: result = a & ~b，利用原生 SIMD 指令（vpandn / vbic）比分开做 NOT+AND 更快。
    [[nodiscard]] BITCAL_FORCEINLINE bitarray andnot(const bitarray& mask) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_andnot>(*this, mask, result);
        return result;
    }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator~() const noexcept {
        bitarray result;
        if constexpr (Bits == 64) {
            result.data_[0] = ~data_[0];
        }
#if BITCAL_HAS_AVX512
        else if constexpr (Backend == simd_backend::avx512 && Bits == 256) {
            avx512::bit_not_256(data_, result.data_);
        }
        else if constexpr (Backend == simd_backend::avx512 && Bits == 512) {
            avx512::bit_not_512(data_, result.data_);
        }
#endif
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            neon::bit_not_128(data_, result.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            neon::bit_not_256(data_, result.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            neon::bit_not_512(data_, result.data_);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            sse::bit_not_128(data_, result.data_);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            sse::bit_not_256(data_, result.data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            avx::bit_not_256(data_, result.data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            avx::bit_not_512(data_, result.data_);
        }
#endif
        else {
            for (size_t i = 0; i < u64_count; ++i) {
                result.data_[i] = ~data_[i];
            }
        }
        return result;
    }
    
    BITCAL_FORCEINLINE bitarray& operator&=(const bitarray& other) noexcept {
        dispatch_binop<binop::op_and>(*this, other, *this);
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator|=(const bitarray& other) noexcept {
        dispatch_binop<binop::op_or>(*this, other, *this);
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator^=(const bitarray& other) noexcept {
        dispatch_binop<binop::op_xor>(*this, other, *this);
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator<<=(int count) noexcept {
        shift_left(count);
        return *this;
    }

    BITCAL_FORCEINLINE bitarray& operator>>=(int count) noexcept {
        shift_right(count);
        return *this;
    }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator<<(int count) const noexcept {
        bitarray result(*this);
        result.shift_left(count);
        return result;
    }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator>>(int count) const noexcept {
        bitarray result(*this);
        result.shift_right(count);
        return result;
    }

    // 统计 1 的个数（对 Bits>64 采用逐 word 累加）。
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t popcount() const noexcept {
        if constexpr (Bits == 64) {
            return scalar::popcount(data_[0]);
        }
#if BITCAL_HAS_AVX512
        else if constexpr (Backend == simd_backend::avx512 && Bits == 256) {
            return avx512::popcount_256(data_);
        }
        else if constexpr (Backend == simd_backend::avx512 && Bits == 512) {
            return avx512::popcount_512(data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            return avx::popcount_256(data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            return avx::popcount_512(data_);
        }
#endif
        else {
            return scalar::popcount_array<u64_count>(data_);
        }
    }

    // 前导零计数（CLZ）。
    [[nodiscard]] BITCAL_FORCEINLINE int count_leading_zeros() const noexcept {
        for (int i = u64_count - 1; i >= 0; --i) {
            if (data_[i] != 0) {
                int clz = scalar::count_leading_zeros(data_[i]);
                return (u64_count - 1 - i) * 64 + clz;
            }
        }
        return Bits;
    }

    // 尾部零计数（CTZ）。
    [[nodiscard]] BITCAL_FORCEINLINE int count_trailing_zeros() const noexcept {
        for (size_t i = 0; i < u64_count; ++i) {
            if (data_[i] != 0) {
                return i * 64 + scalar::count_trailing_zeros(data_[i]);
            }
        }
        return Bits;
    }
    
    // 位反转：bit0 <-> bit(Bits-1)。
    BITCAL_FORCEINLINE void reverse() noexcept {
        if constexpr (Bits == 64) {
            data_[0] = scalar::reverse_bits(data_[0]);
        } else {
            for (size_t i = 0; i < u64_count / 2; ++i) {
                uint64_t a = scalar::reverse_bits(data_[i]);
                uint64_t b = scalar::reverse_bits(data_[u64_count - 1 - i]);
                data_[i] = b;
                data_[u64_count - 1 - i] = a;
            }
            if constexpr (u64_count % 2 == 1) {
                data_[u64_count / 2] = scalar::reverse_bits(data_[u64_count / 2]);
            }
        }
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool is_zero() const noexcept {
        if constexpr (Bits == 64) {
            return data_[0] == 0;
        }
#if BITCAL_HAS_AVX512
        else if constexpr (Backend == simd_backend::avx512 && Bits == 256) {
            return avx512::is_zero_256(data_);
        }
        else if constexpr (Backend == simd_backend::avx512 && Bits == 512) {
            return avx512::is_zero_512(data_);
        }
#endif
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            return neon::is_zero_128(data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            return neon::is_zero_256(data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            return neon::is_zero_512(data_);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            return sse::is_zero_128(data_);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            return sse::is_zero_256(data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            return avx::is_zero_256(data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            return avx::is_zero_512(data_);
        }
#endif
        else {
            for (size_t i = 0; i < u64_count; ++i) {
                if (data_[i] != 0) return false;
            }
            return true;
        }
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool operator==(const bitarray& other) const noexcept {
        if constexpr (Bits == 64) {
            return data_[0] == other.data_[0];
        }
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            return avx::equals_256(data_, other.data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            return avx::equals_512(data_, other.data_);
        }
#endif
        else {
            return scalar::equals_array<u64_count>(data_, other.data_);
        }
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool operator!=(const bitarray& other) const noexcept {
        return !(*this == other);
    }
    
    // ============================================================================
    // Find first/last set bit
    // ============================================================================
    
    /// Find the position of the first (lowest index) set bit
    /// @return Position (0-indexed) of first set bit, or -1 if all bits are zero
    [[nodiscard]] BITCAL_FORCEINLINE int find_first_set() const noexcept {
        return scalar::find_first_set<u64_count>(data_);
    }
    
    /// Find the position of the last (highest index) set bit
    /// @return Position (0-indexed) of last set bit, or -1 if all bits are zero
    [[nodiscard]] BITCAL_FORCEINLINE int find_last_set() const noexcept {
        return scalar::find_last_set<u64_count>(data_);
    }
    
    // ============================================================================
    // Range operations
    // ============================================================================
    
    /// Set all bits in the range [start, end)
    /// @param start Start bit position (inclusive)
    /// @param end End bit position (exclusive)
    BITCAL_FORCEINLINE void set_range(size_t start, size_t end) noexcept {
        scalar::set_range<u64_count>(data_, start, end);
    }
    
    /// Clear all bits in the range [start, end)
    /// @param start Start bit position (inclusive)
    /// @param end End bit position (exclusive)
    BITCAL_FORCEINLINE void clear_range(size_t start, size_t end) noexcept {
        scalar::clear_range<u64_count>(data_, start, end);
    }
    
    /// Flip all bits in the range [start, end)
    /// @param start Start bit position (inclusive)
    /// @param end End bit position (exclusive)
    BITCAL_FORCEINLINE void flip_range(size_t start, size_t end) noexcept {
        scalar::flip_range<u64_count>(data_, start, end);
    }
    
    // ============================================================================
    // Comparison operations
    // ============================================================================

    /// Check if all bits are set (all ones)
    [[nodiscard]] BITCAL_FORCEINLINE bool all() const noexcept {
        if constexpr (Bits == 64) {
            return data_[0] == ~0ULL;
        }
#if BITCAL_HAS_AVX512
        else if constexpr (Backend == simd_backend::avx512 && Bits == 256) {
            return avx512::all_256(data_);
        }
        else if constexpr (Backend == simd_backend::avx512 && Bits == 512) {
            return avx512::all_512(data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            return avx::all_256(data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            return avx::all_512(data_);
        }
#endif
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            return neon::all_128(data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            return neon::all_256(data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            return neon::all_512(data_);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            return sse::all_128(data_);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            return sse::all_256(data_);
        }
#endif
        else {
            for (size_t i = 0; i < u64_count; ++i) {
                if (data_[i] != ~0ULL) return false;
            }
            return true;
        }
    }
    
    /// Check if any bit is set
    [[nodiscard]] BITCAL_FORCEINLINE bool any() const noexcept {
        return !is_zero();
    }
    
    /// Check if no bits are set
    [[nodiscard]] BITCAL_FORCEINLINE bool none() const noexcept {
        return is_zero();
    }
    
    /// Count the number of set bits (alias for popcount)
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t count() const noexcept {
        return popcount();
    }
    
    /// Return the number of bits in the bitarray
    [[nodiscard]] static constexpr size_t size() noexcept {
        return Bits;
    }
    
    /// Test if a bit is set (alias for get_bit)
    [[nodiscard]] BITCAL_FORCEINLINE bool test(size_t bit_index) const noexcept {
        return get_bit(bit_index);
    }

private:
    alignas(get_optimal_alignment<Bits>()) uint64_t data_[u64_count];
    
    enum class binop { op_and, op_or, op_xor, op_andnot };
    
    template<binop Op>
    static BITCAL_FORCEINLINE void dispatch_binop(const bitarray& a, const bitarray& b, bitarray& out) noexcept {
        if constexpr (Bits == 64) {
            if constexpr (Op == binop::op_and)    out.data_[0] = scalar::bit_and(a.data_[0], b.data_[0]);
            if constexpr (Op == binop::op_or)     out.data_[0] = scalar::bit_or(a.data_[0], b.data_[0]);
            if constexpr (Op == binop::op_xor)    out.data_[0] = scalar::bit_xor(a.data_[0], b.data_[0]);
            if constexpr (Op == binop::op_andnot) out.data_[0] = scalar::bit_andnot(a.data_[0], b.data_[0]);
        }
#if BITCAL_HAS_AVX512
        else if constexpr (Backend == simd_backend::avx512 && Bits == 256) {
            if constexpr (Op == binop::op_and)    avx512::bit_and_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     avx512::bit_or_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    avx512::bit_xor_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) avx512::bit_andnot_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::avx512 && Bits == 512) {
            if constexpr (Op == binop::op_and)    avx512::bit_and_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     avx512::bit_or_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    avx512::bit_xor_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) avx512::bit_andnot_512(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            if constexpr (Op == binop::op_and)    neon::bit_and_128(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     neon::bit_or_128(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    neon::bit_xor_128(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) neon::bit_andnot_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            if constexpr (Op == binop::op_and)    neon::bit_and_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     neon::bit_or_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    neon::bit_xor_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) neon::bit_andnot_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            if constexpr (Op == binop::op_and)    neon::bit_and_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     neon::bit_or_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    neon::bit_xor_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) neon::bit_andnot_512(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            if constexpr (Op == binop::op_and)    sse::bit_and_128(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     sse::bit_or_128(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    sse::bit_xor_128(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) sse::bit_andnot_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            if constexpr (Op == binop::op_and)    sse::bit_and_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     sse::bit_or_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    sse::bit_xor_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) sse::bit_andnot_256(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            if constexpr (Op == binop::op_and)    avx::bit_and_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     avx::bit_or_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    avx::bit_xor_256(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) avx::bit_andnot_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            if constexpr (Op == binop::op_and)    avx::bit_and_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     avx::bit_or_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    avx::bit_xor_512(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) avx::bit_andnot_512(a.data_, b.data_, out.data_);
        }
#endif
        else {
            if constexpr (Op == binop::op_and)    scalar::bit_and_array<u64_count>(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_or)     scalar::bit_or_array<u64_count>(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_xor)    scalar::bit_xor_array<u64_count>(a.data_, b.data_, out.data_);
            if constexpr (Op == binop::op_andnot) scalar::bit_andnot_array<u64_count>(a.data_, b.data_, out.data_);
        }
    }
};

// ============================================================================
// Optimized bit64 specialization using scalar uint64_t
// ============================================================================

template<>
class bitarray<64> {
public:
    static constexpr size_t bits = 64;
    static constexpr size_t u64_count = 1;
    static constexpr simd_backend backend = simd_backend::scalar;

    bitarray() noexcept : value_(0) {}
    explicit bitarray(uint64_t value) noexcept : value_(value) {}

    // Copy and move constructors
    bitarray(const bitarray&) noexcept = default;
    bitarray(bitarray&&) noexcept = default;
    bitarray& operator=(const bitarray&) noexcept = default;
    bitarray& operator=(bitarray&&) noexcept = default;
    ~bitarray() = default;

    // Direct access to underlying value
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t* data() noexcept { return &value_; }
    [[nodiscard]] BITCAL_FORCEINLINE const uint64_t* data() const noexcept { return &value_; }
    
    // Single element access (for API compatibility)
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t operator[](size_t index) const noexcept {
        (void)index;  // Always 0 for bit64
        assert(index == 0);
        return value_;
    }
    
    BITCAL_FORCEINLINE uint64_t& operator[](size_t index) noexcept {
        (void)index;
        assert(index == 0);
        return value_;
    }
    
    // Bit operations
    [[nodiscard]] BITCAL_FORCEINLINE bool get_bit(size_t bit_index) const noexcept {
        assert(bit_index < 64);
        return (value_ >> bit_index) & 1;
    }
    
    BITCAL_FORCEINLINE void set_bit(size_t bit_index, bool val = true) noexcept {
        assert(bit_index < 64);
        if (val) {
            value_ |= (1ULL << bit_index);
        } else {
            value_ &= ~(1ULL << bit_index);
        }
    }
    
    BITCAL_FORCEINLINE void flip_bit(size_t bit_index) noexcept {
        assert(bit_index < 64);
        value_ ^= (1ULL << bit_index);
    }
    
    // Shift operations
    BITCAL_FORCEINLINE void shift_left(int count) noexcept {
        value_ = scalar::shift_left(value_, count);
    }
    
    BITCAL_FORCEINLINE void shift_right(int count) noexcept {
        value_ = scalar::shift_right(value_, count);
    }
    
    BITCAL_FORCEINLINE void clear() noexcept { value_ = 0; }
    
    // Bitwise operators
    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator&(const bitarray& other) const noexcept {
        return bitarray(value_ & other.value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator|(const bitarray& other) const noexcept {
        return bitarray(value_ | other.value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator^(const bitarray& other) const noexcept {
        return bitarray(value_ ^ other.value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator~() const noexcept {
        return bitarray(~value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bitarray andnot(const bitarray& mask) const noexcept {
        return bitarray(scalar::bit_andnot(value_, mask.value_));
    }
    
    // Assignment operators
    BITCAL_FORCEINLINE bitarray& operator&=(const bitarray& other) noexcept {
        value_ &= other.value_;
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator|=(const bitarray& other) noexcept {
        value_ |= other.value_;
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator^=(const bitarray& other) noexcept {
        value_ ^= other.value_;
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator<<=(int count) noexcept {
        shift_left(count);
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator>>=(int count) noexcept {
        shift_right(count);
        return *this;
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator<<(int count) const noexcept {
        return bitarray(scalar::shift_left(value_, count));
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator>>(int count) const noexcept {
        return bitarray(scalar::shift_right(value_, count));
    }
    
    // Counting operations
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t popcount() const noexcept {
        return scalar::popcount(value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE int count_leading_zeros() const noexcept {
        return scalar::count_leading_zeros(value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE int count_trailing_zeros() const noexcept {
        return scalar::count_trailing_zeros(value_);
    }
    
    // Other operations
    BITCAL_FORCEINLINE void reverse() noexcept {
        value_ = scalar::reverse_bits(value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool is_zero() const noexcept {
        return value_ == 0;
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool operator==(const bitarray& other) const noexcept {
        return value_ == other.value_;
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool operator!=(const bitarray& other) const noexcept {
        return value_ != other.value_;
    }
    
    // Find first/last set bit
    [[nodiscard]] BITCAL_FORCEINLINE int find_first_set() const noexcept {
        if (value_ == 0) return -1;
        return scalar::count_trailing_zeros(value_);
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE int find_last_set() const noexcept {
        if (value_ == 0) return -1;
        return 63 - scalar::count_leading_zeros(value_);
    }
    
    // Range operations
    BITCAL_FORCEINLINE void set_range(size_t start, size_t end) noexcept {
        if (start >= end || start >= 64) return;
        if (end > 64) end = 64;
        uint64_t mask;
        size_t num_bits = end - start;
        if (num_bits == 64) {
            mask = ~0ULL;
        } else {
            mask = ((1ULL << num_bits) - 1) << start;
        }
        value_ |= mask;
    }
    
    BITCAL_FORCEINLINE void clear_range(size_t start, size_t end) noexcept {
        if (start >= end || start >= 64) return;
        if (end > 64) end = 64;
        uint64_t mask;
        size_t num_bits = end - start;
        if (num_bits == 64) {
            mask = ~0ULL;
        } else {
            mask = ((1ULL << num_bits) - 1) << start;
        }
        value_ &= ~mask;
    }
    
    BITCAL_FORCEINLINE void flip_range(size_t start, size_t end) noexcept {
        if (start >= end || start >= 64) return;
        if (end > 64) end = 64;
        uint64_t mask;
        size_t num_bits = end - start;
        if (num_bits == 64) {
            mask = ~0ULL;
        } else {
            mask = ((1ULL << num_bits) - 1) << start;
        }
        value_ ^= mask;
    }
    
    // Additional query methods
    [[nodiscard]] BITCAL_FORCEINLINE bool all() const noexcept {
        return value_ == ~0ULL;
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool any() const noexcept {
        return value_ != 0;
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool none() const noexcept {
        return value_ == 0;
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t count() const noexcept {
        return popcount();
    }
    
    [[nodiscard]] static constexpr size_t size() noexcept {
        return 64;
    }
    
    [[nodiscard]] BITCAL_FORCEINLINE bool test(size_t bit_index) const noexcept {
        return get_bit(bit_index);
    }
    
    // Explicit conversion to uint64_t
    [[nodiscard]] explicit constexpr operator uint64_t() const noexcept {
        return value_;
    }

private:
    uint64_t value_;
};

// 常用位宽别名（默认后端 = get_default_backend()）。
using bit64 = bitarray<64>;
using bit128 = bitarray<128>;
using bit256 = bitarray<256>;
using bit512 = bitarray<512>;
using bit1024 = bitarray<1024>;

namespace ops {

// ops：给“原始 uint64_t 数组”提供的函数式接口。
// 适合与外部数据结构对接，或在不想构造 bitarray 时直接做运算。

template<size_t Bits>
BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
    return scalar::popcount_array<(Bits + 63) / 64>(data);
}

template<size_t Bits>
BITCAL_FORCEINLINE int count_leading_zeros(const uint64_t* data) noexcept {
    constexpr size_t n = (Bits + 63) / 64;
    for (int i = n - 1; i >= 0; --i) {
        if (data[i] != 0) {
            return (n - 1 - i) * 64 + scalar::count_leading_zeros(data[i]);
        }
    }
    return Bits;
}

template<size_t Bits>
BITCAL_FORCEINLINE int count_trailing_zeros(const uint64_t* data) noexcept {
    constexpr size_t n = (Bits + 63) / 64;
    for (size_t i = 0; i < n; ++i) {
        if (data[i] != 0) {
            return i * 64 + scalar::count_trailing_zeros(data[i]);
        }
    }
    return Bits;
}

BITCAL_FORCEINLINE uint64_t reverse_bits_64(uint64_t x) noexcept {
    return scalar::reverse_bits(x);
}

BITCAL_FORCEINLINE uint64_t byte_swap_64(uint64_t x) noexcept {
    return scalar::byte_swap(x);
}

}

// ============================================================================
// Type Traits
// ============================================================================

/// Type trait to check if T is a bitarray type
template<typename T>
struct is_bitarray : std::false_type {};

template<size_t Bits, simd_backend Backend>
struct is_bitarray<bitarray<Bits, Backend>> : std::true_type {};

template<typename T>
inline constexpr bool is_bitarray_v = is_bitarray<T>::value;

/// Type traits for bitarray types
template<typename T>
struct bitarray_traits;

template<size_t Bits, simd_backend Backend>
struct bitarray_traits<bitarray<Bits, Backend>> {
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;
    using word_type = uint64_t;
    using type = bitarray<Bits, Backend>;
};

}
