#pragma once

#include "config.hpp"

#include "backend_ops.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>

namespace bitcal {

template <size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray {
    static_assert(Bits >= 64, "Bits must be at least 64");
    static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");

    static_assert(!(Backend == simd_backend::sse2 && !BITCAL_HAS_SSE2),
                  "SSE2 backend selected but not available. Compile with -msse2 (x86) or use a different backend.");
    static_assert(!(Backend == simd_backend::avx2 && !BITCAL_HAS_AVX2),
                  "AVX2 backend selected but not available. Compile with -mavx2 (x86) or use a different backend.");
    static_assert(
        !(Backend == simd_backend::avx512 && !BITCAL_HAS_AVX512),
        "AVX-512 backend selected but not available. Compile with -mavx512f (x86) or use a different backend.");
    static_assert(!(Backend == simd_backend::neon && !BITCAL_HAS_NEON),
                  "NEON backend selected but not available. Use ARM platform or a different backend.");

public:
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;

    bitarray() noexcept { clear(); }

    explicit bitarray(uint64_t value) noexcept {
        clear();
        data_[0] = value;
    }

    bitarray(const bitarray& other) noexcept = default;
    bitarray(bitarray&& other) noexcept = default;
    bitarray& operator=(const bitarray& other) noexcept = default;
    bitarray& operator=(bitarray&& other) noexcept = default;

    BITCAL_FORCEINLINE void clear() noexcept { std::memset(data_, 0, sizeof(data_)); }

    [[nodiscard]] BITCAL_FORCEINLINE const uint64_t* data() const noexcept { return data_; }

    [[nodiscard]] BITCAL_FORCEINLINE uint64_t word(size_t index) const noexcept {
        assert(index < u64_count);
        return data_[index];
    }

    BITCAL_FORCEINLINE void set_word(size_t index, uint64_t value) noexcept {
        assert(index < u64_count);
        data_[index] = value;
    }

    [[nodiscard]] BITCAL_FORCEINLINE uint64_t operator[](size_t index) const noexcept {
        assert(index < u64_count);
        return data_[index];
    }

    [[nodiscard]] BITCAL_FORCEINLINE bool get_bit(size_t bit_index) const noexcept {
        assert(bit_index < Bits);
        const size_t word_idx = bit_index / 64;
        const size_t bit_offset = bit_index % 64;
        return (data_[word_idx] >> bit_offset) & 1;
    }

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

    BITCAL_FORCEINLINE void flip_bit(size_t bit_index) noexcept {
        assert(bit_index < Bits);
        const size_t word_idx = bit_index / 64;
        const size_t bit_offset = bit_index % 64;
        data_[word_idx] ^= (1ULL << bit_offset);
    }

    BITCAL_FORCEINLINE void shift_left(int count) noexcept { shift_impl<shift_dir::left>(count); }

    BITCAL_FORCEINLINE void shift_right(int count) noexcept { shift_impl<shift_dir::right>(count); }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator&(const bitarray& other) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_and>(*this, other, result);
        return result;
    }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator|(const bitarray& other) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_or>(*this, other, result);
        return result;
    }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator^(const bitarray& other) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_xor>(*this, other, result);
        return result;
    }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray andnot(const bitarray& mask) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_andnot>(*this, mask, result);
        return result;
    }

    [[nodiscard]] BITCAL_FORCEINLINE bitarray operator~() const noexcept {
        bitarray result;
        backend::ops<Bits, Backend>::bit_not(data_, result.data_);
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

    [[nodiscard]] BITCAL_FORCEINLINE uint64_t popcount() const noexcept {
        return backend::ops<Bits, Backend>::popcount(data_);
    }

    [[nodiscard]] BITCAL_FORCEINLINE int count_leading_zeros() const noexcept {
        return backend::ops<Bits, Backend>::count_leading_zeros(data_);
    }

    [[nodiscard]] BITCAL_FORCEINLINE int count_trailing_zeros() const noexcept {
        return backend::ops<Bits, Backend>::count_trailing_zeros(data_);
    }

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
        return backend::ops<Bits, Backend>::is_zero(data_);
    }

    [[nodiscard]] BITCAL_FORCEINLINE bool operator==(const bitarray& other) const noexcept {
        return backend::ops<Bits, Backend>::equals(data_, other.data_);
    }

    [[nodiscard]] BITCAL_FORCEINLINE bool operator!=(const bitarray& other) const noexcept { return !(*this == other); }

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
    BITCAL_FORCEINLINE void set_range(size_t start, size_t end) noexcept {
        scalar::set_range<u64_count>(data_, start, end);
    }

    /// Clear all bits in the range [start, end)
    BITCAL_FORCEINLINE void clear_range(size_t start, size_t end) noexcept {
        scalar::clear_range<u64_count>(data_, start, end);
    }

    /// Flip all bits in the range [start, end)
    BITCAL_FORCEINLINE void flip_range(size_t start, size_t end) noexcept {
        scalar::flip_range<u64_count>(data_, start, end);
    }

    // ============================================================================
    // Comparison operations
    // ============================================================================

    /// Check if all bits are set (all ones)
    [[nodiscard]] BITCAL_FORCEINLINE bool all() const noexcept { return backend::ops<Bits, Backend>::all(data_); }

    /// Check if any bit is set
    [[nodiscard]] BITCAL_FORCEINLINE bool any() const noexcept { return !is_zero(); }

    /// Check if no bits are set
    [[nodiscard]] BITCAL_FORCEINLINE bool none() const noexcept { return is_zero(); }

    /// Count the number of set bits (alias for popcount)
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t count() const noexcept { return popcount(); }

    /// Return the number of bits in the bitarray
    [[nodiscard]] static constexpr size_t size() noexcept { return Bits; }

    /// Test if a bit is set (alias for get_bit)
    [[nodiscard]] BITCAL_FORCEINLINE bool test(size_t bit_index) const noexcept { return get_bit(bit_index); }

private:
    alignas(get_optimal_alignment<Bits>()) uint64_t data_[u64_count];

    enum class binop { op_and, op_or, op_xor, op_andnot };
    enum class shift_dir { left, right };

    template <shift_dir Dir>
    BITCAL_FORCEINLINE void shift_impl(int count) noexcept {
        if constexpr (Dir == shift_dir::left) {
            backend::ops<Bits, Backend>::shift_left(data_, count);
        } else {
            backend::ops<Bits, Backend>::shift_right(data_, count);
        }
    }

    template <binop Op>
    static BITCAL_FORCEINLINE void dispatch_binop(const bitarray& a, const bitarray& b, bitarray& out) noexcept {
        if constexpr (Op == binop::op_and) {
            backend::ops<Bits, Backend>::bit_and(a.data_, b.data_, out.data_);
        } else if constexpr (Op == binop::op_or) {
            backend::ops<Bits, Backend>::bit_or(a.data_, b.data_, out.data_);
        } else if constexpr (Op == binop::op_xor) {
            backend::ops<Bits, Backend>::bit_xor(a.data_, b.data_, out.data_);
        } else if constexpr (Op == binop::op_andnot) {
            backend::ops<Bits, Backend>::bit_andnot(a.data_, b.data_, out.data_);
        }
    }
};

// ============================================================================
// bit64 conversion operator
// ============================================================================

template <>
class bitarray<64> : public bitarray<64, simd_backend::scalar> {
    using Base = bitarray<64, simd_backend::scalar>;

public:
    using Base::Base;

    [[nodiscard]] explicit BITCAL_FORCEINLINE operator uint64_t() const noexcept { return Base::data()[0]; }
};

using bit64 = bitarray<64>;
using bit128 = bitarray<128>;
using bit256 = bitarray<256>;
using bit512 = bitarray<512>;
using bit1024 = bitarray<1024>;

}  // namespace bitcal
