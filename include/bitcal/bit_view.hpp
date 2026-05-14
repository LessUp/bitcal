#pragma once

#include "config.hpp"

#include <cassert>

namespace bitcal {

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

protected:
    const std::uint64_t* data_ = nullptr;
    std::size_t word_count_ = 0;
};

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
