#pragma once

#include "config.hpp"
#include "scalar_ops.hpp"
#include "backend_ops.hpp"

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

    // 编译期验证：确保后端选择与编译选项一致
    // 如果用户显式指定了某个后端，但编译时未启用相应指令集，编译将失败
    static_assert(
        !(Backend == simd_backend::sse2 && !BITCAL_HAS_SSE2),
        "SSE2 backend selected but not available. Compile with -msse2 (x86) or use a different backend."
    );
    static_assert(
        !(Backend == simd_backend::avx2 && !BITCAL_HAS_AVX2),
        "AVX2 backend selected but not available. Compile with -mavx2 (x86) or use a different backend."
    );
    static_assert(
        !(Backend == simd_backend::avx512 && !BITCAL_HAS_AVX512),
        "AVX-512 backend selected but not available. Compile with -mavx512f (x86) or use a different backend."
    );
    static_assert(
        !(Backend == simd_backend::neon && !BITCAL_HAS_NEON),
        "NEON backend selected but not available. Use ARM platform or a different backend."
    );

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

    // ============================================================================
    // 数据访问接口
    // ============================================================================

    /// 只读访问底层 word 数组指针
    /// 用于与外部 C API 或需要连续内存的场景对接
    [[nodiscard]] BITCAL_FORCEINLINE const uint64_t* data() const noexcept { return data_; }

    /// 读取指定位置的 64-bit word（只读）
    /// @param index word 索引（0 到 u64_count-1）
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t word(size_t index) const noexcept {
        assert(index < u64_count);
        return data_[index];
    }

    /// 设置指定位置的 64-bit word
    /// @param index word 索引（0 到 u64_count-1）
    /// @param value 要设置的值
    BITCAL_FORCEINLINE void set_word(size_t index, uint64_t value) noexcept {
        assert(index < u64_count);
        data_[index] = value;
    }

    /// 读取指定位置的 64-bit word（只读）
    /// operator[] 现在是只读的，提供类似数组访问的语法
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t operator[](size_t index) const noexcept {
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
        shift_impl<shift_dir::left>(count);
    }

    // 右移 count 位（超出位宽的部分被丢弃）。
    BITCAL_FORCEINLINE void shift_right(int count) noexcept {
        shift_impl<shift_dir::right>(count);
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

    // 统计 1 的个数（对 Bits>64 采用逐 word 累加）。
    [[nodiscard]] BITCAL_FORCEINLINE uint64_t popcount() const noexcept {
        return backend::ops<Bits, Backend>::popcount(data_);
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
        return backend::ops<Bits, Backend>::is_zero(data_);
    }

    [[nodiscard]] BITCAL_FORCEINLINE bool operator==(const bitarray& other) const noexcept {
        return backend::ops<Bits, Backend>::equals(data_, other.data_);
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
        return backend::ops<Bits, Backend>::all(data_);
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
    enum class shift_dir { left, right };

    // 统一的 shift dispatch 模板，减少 shift_left/shift_right 的代码重复
    template<shift_dir Dir>
    BITCAL_FORCEINLINE void shift_impl(int count) noexcept {
        if constexpr (Dir == shift_dir::left) {
            backend::ops<Bits, Backend>::shift_left(data_, count);
        } else {
            backend::ops<Bits, Backend>::shift_right(data_, count);
        }
    }

    template<binop Op>
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
// Minimal specialization to add explicit conversion to uint64_t for bit64.
// All other operations are inherited from the primary template.

template<>
class bitarray<64> : public bitarray<64, simd_backend::scalar> {
    using Base = bitarray<64, simd_backend::scalar>;
public:
    using Base::Base;  // 继承所有构造函数

    // Explicit conversion to uint64_t - unique to bit64
    [[nodiscard]] explicit BITCAL_FORCEINLINE operator uint64_t() const noexcept {
        return Base::data()[0];
    }
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
