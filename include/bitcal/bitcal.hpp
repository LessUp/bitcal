#pragma once

#include "config.hpp"
#include "scalar_ops.hpp"

#if BITCAL_HAS_SSE2
#include "sse_ops.hpp"
#endif

#if BITCAL_HAS_AVX2
#include "avx_ops.hpp"
#endif

#if BITCAL_HAS_NEON
#include "neon_ops.hpp"
#endif

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace bitcal {

template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray {
public:
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = (Bits + 63) / 64;
    static constexpr simd_backend backend = Backend;
    
    bitarray() noexcept {
        clear();
    }
    
    explicit bitarray(uint64_t value) noexcept {
        clear();
        data_[0] = value;
    }
    
    BITCAL_FORCEINLINE void clear() noexcept {
        for (size_t i = 0; i < u64_count; ++i) {
            data_[i] = 0;
        }
    }
    
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
    
    BITCAL_FORCEINLINE bool get_bit(size_t bit_index) const noexcept {
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
    
    BITCAL_FORCEINLINE void shift_left(int count) noexcept {
        if constexpr (Bits == 64) {
            data_[0] = scalar::shift_left(data_[0], count);
        }
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
    
    BITCAL_FORCEINLINE void shift_right(int count) noexcept {
        if constexpr (Bits == 64) {
            data_[0] = scalar::shift_right(data_[0], count);
        }
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
        bit_and(*this, other, result);
        return result;
    }
    
    BITCAL_FORCEINLINE bitarray operator|(const bitarray& other) const noexcept {
        bitarray result;
        bit_or(*this, other, result);
        return result;
    }
    
    BITCAL_FORCEINLINE bitarray operator^(const bitarray& other) const noexcept {
        bitarray result;
        bit_xor(*this, other, result);
        return result;
    }
    
    BITCAL_FORCEINLINE bitarray operator~() const noexcept {
        bitarray result;
        for (size_t i = 0; i < u64_count; ++i) {
            result.data_[i] = ~data_[i];
        }
        return result;
    }
    
    BITCAL_FORCEINLINE bitarray& operator&=(const bitarray& other) noexcept {
        bit_and(*this, other, *this);
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator|=(const bitarray& other) noexcept {
        bit_or(*this, other, *this);
        return *this;
    }
    
    BITCAL_FORCEINLINE bitarray& operator^=(const bitarray& other) noexcept {
        bit_xor(*this, other, *this);
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
    
    BITCAL_FORCEINLINE bitarray operator<<(int count) const noexcept {
        bitarray result(*this);
        result.shift_left(count);
        return result;
    }
    
    BITCAL_FORCEINLINE bitarray operator>>(int count) const noexcept {
        bitarray result(*this);
        result.shift_right(count);
        return result;
    }
    
    BITCAL_FORCEINLINE uint64_t popcount() const noexcept {
        if constexpr (Bits == 64) {
            return scalar::popcount(data_[0]);
        } else {
            return scalar::popcount_array<u64_count>(data_);
        }
    }
    
    BITCAL_FORCEINLINE int count_leading_zeros() const noexcept {
        for (int i = u64_count - 1; i >= 0; --i) {
            if (data_[i] != 0) {
                int clz = scalar::count_leading_zeros(data_[i]);
                return (u64_count - 1 - i) * 64 + clz;
            }
        }
        return Bits;
    }
    
    BITCAL_FORCEINLINE int count_trailing_zeros() const noexcept {
        for (size_t i = 0; i < u64_count; ++i) {
            if (data_[i] != 0) {
                return i * 64 + scalar::count_trailing_zeros(data_[i]);
            }
        }
        return Bits;
    }
    
    BITCAL_FORCEINLINE void reverse() noexcept {
        if constexpr (Bits == 64) {
            data_[0] = scalar::reverse_bits(data_[0]);
        } else {
            uint64_t temp[u64_count];
            scalar::reverse_bits_array<u64_count>(data_, temp);
            for (size_t i = 0; i < u64_count; ++i) {
                data_[i] = temp[i];
            }
        }
    }
    
    BITCAL_FORCEINLINE bool is_zero() const noexcept {
        for (size_t i = 0; i < u64_count; ++i) {
            if (data_[i] != 0) return false;
        }
        return true;
    }
    
    BITCAL_FORCEINLINE bool operator==(const bitarray& other) const noexcept {
        for (size_t i = 0; i < u64_count; ++i) {
            if (data_[i] != other.data_[i]) return false;
        }
        return true;
    }
    
    BITCAL_FORCEINLINE bool operator!=(const bitarray& other) const noexcept {
        return !(*this == other);
    }

private:
    alignas(BITCAL_ALIGNMENT) uint64_t data_[u64_count];
    
    static BITCAL_FORCEINLINE void bit_and(const bitarray& a, const bitarray& b, bitarray& out) noexcept {
        if constexpr (Bits == 64) {
            out.data_[0] = scalar::bit_and(a.data_[0], b.data_[0]);
        }
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            neon::bit_and_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            neon::bit_and_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            neon::bit_and_512(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            sse::bit_and_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            sse::bit_and_256(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            avx::bit_and_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            avx::bit_and_512(a.data_, b.data_, out.data_);
        }
#endif
        else {
            scalar::bit_and_array<u64_count>(a.data_, b.data_, out.data_);
        }
    }
    
    static BITCAL_FORCEINLINE void bit_or(const bitarray& a, const bitarray& b, bitarray& out) noexcept {
        if constexpr (Bits == 64) {
            out.data_[0] = scalar::bit_or(a.data_[0], b.data_[0]);
        }
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            neon::bit_or_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            neon::bit_or_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            neon::bit_or_512(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            sse::bit_or_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            sse::bit_or_256(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            avx::bit_or_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            avx::bit_or_512(a.data_, b.data_, out.data_);
        }
#endif
        else {
            scalar::bit_or_array<u64_count>(a.data_, b.data_, out.data_);
        }
    }
    
    static BITCAL_FORCEINLINE void bit_xor(const bitarray& a, const bitarray& b, bitarray& out) noexcept {
        if constexpr (Bits == 64) {
            out.data_[0] = scalar::bit_xor(a.data_[0], b.data_[0]);
        }
#if BITCAL_HAS_NEON
        else if constexpr (Backend == simd_backend::neon && Bits == 128) {
            neon::bit_xor_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 256) {
            neon::bit_xor_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::neon && Bits == 512) {
            neon::bit_xor_512(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_SSE2
        else if constexpr (Backend == simd_backend::sse2 && Bits == 128) {
            sse::bit_xor_128(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::sse2 && Bits == 256) {
            sse::bit_xor_256(a.data_, b.data_, out.data_);
        }
#endif
#if BITCAL_HAS_AVX2
        else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
            avx::bit_xor_256(a.data_, b.data_, out.data_);
        }
        else if constexpr (Backend == simd_backend::avx2 && Bits == 512) {
            avx::bit_xor_512(a.data_, b.data_, out.data_);
        }
#endif
        else {
            scalar::bit_xor_array<u64_count>(a.data_, b.data_, out.data_);
        }
    }
};

using bit64 = bitarray<64>;
using bit128 = bitarray<128>;
using bit256 = bitarray<256>;
using bit512 = bitarray<512>;
using bit1024 = bitarray<1024>;

namespace ops {

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

}
