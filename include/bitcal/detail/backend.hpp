#pragma once

#include <cstddef>
#include <cstdint>

namespace bitcal::detail {

BITCAL_FORCEINLINE void and_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = lhs[i] & rhs[i];
    }
}

}  // namespace bitcal::detail
