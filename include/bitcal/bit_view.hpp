#pragma once

#include "config.hpp"

#include <cassert>

namespace bitcal {

// 覆盖一段连续 uint64 字的只读视图。
class const_bit_view {
public:
    constexpr const_bit_view() noexcept = default;

    constexpr const_bit_view(const std::uint64_t* data, std::size_t words) noexcept : data_(data), word_count_(words) {}

    [[nodiscard]] constexpr const std::uint64_t* data() const noexcept { return data_; }
    [[nodiscard]] constexpr std::size_t word_count() const noexcept { return word_count_; }

    [[nodiscard]] constexpr std::uint64_t word(const std::size_t index) const noexcept {
        assert(index < word_count_);
        return data_[index];
    }

private:
    const std::uint64_t* data_ = nullptr;
    std::size_t word_count_ = 0;
};

// 覆盖一段连续 uint64 字的可变视图。直接存 `uint64_t*`（无 const_cast），
// 并提供到 const_bit_view 的隐式转换，调用方可在需要 const_bit_view 处直接
// 传入 bit_view。两种类型共享同一接口形态但无继承关系，保持可变指针类型的
// 诚实性。
class bit_view {
public:
    constexpr bit_view() noexcept = default;

    constexpr bit_view(std::uint64_t* data, std::size_t words) noexcept : data_(data), word_count_(words) {}

    [[nodiscard]] constexpr std::uint64_t* data() noexcept { return data_; }
    [[nodiscard]] constexpr const std::uint64_t* data() const noexcept { return data_; }
    [[nodiscard]] constexpr std::size_t word_count() const noexcept { return word_count_; }

    [[nodiscard]] constexpr std::uint64_t word(const std::size_t index) const noexcept {
        assert(index < word_count_);
        return data_[index];
    }

    [[nodiscard]] constexpr operator const_bit_view() const noexcept { return const_bit_view(data_, word_count_); }

private:
    std::uint64_t* data_ = nullptr;
    std::size_t word_count_ = 0;
};

}  // namespace bitcal
