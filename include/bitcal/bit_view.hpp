#pragma once

#include "config.hpp"

#include <cassert>

namespace bitcal {

// Read-only view over a contiguous range of uint64 words.
// `bit_view` (below) extends this via public inheritance to add mutable
// access; the const-only interface here is the shared substrate so the
// word_count/word/default-ctor/member layout is defined once.
class const_bit_view {
public:
    constexpr const_bit_view() noexcept = default;

    constexpr const_bit_view(const std::uint64_t* data, std::size_t words) noexcept
        : data_(data), word_count_(words) {}

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

// Mutable view over a contiguous range of uint64 words. Inherits the
// read-only interface from const_bit_view and adds a mutable `data()`
// overload. The base stores the pointer as `const std::uint64_t*` (set by
// the implicit conversion in the constructor); `data()` restores the
// original mutability via const_cast, which is safe because a bit_view is
// only ever constructed from a non-const pointer. The implicit conversion
// to const_bit_view is a free public-base upcast, so callers can pass a
// bit_view wherever a const_bit_view is expected.
class bit_view : public const_bit_view {
public:
    constexpr bit_view() noexcept = default;

    constexpr bit_view(std::uint64_t* data, std::size_t words) noexcept
        : const_bit_view(data, words) {}

    [[nodiscard]] constexpr std::uint64_t* data() const noexcept {
        return const_cast<std::uint64_t*>(const_bit_view::data());
    }
};

}  // namespace bitcal
