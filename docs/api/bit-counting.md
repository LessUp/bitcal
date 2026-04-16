# Bit Counting

## popcount — Count Set Bits

```cpp
uint64_t popcount() const;
```

Count the number of bits set to 1.

**Implementation**:
- Uses `__builtin_popcountll` (GCC/Clang) or `__popcnt64` (MSVC x64)
- For Bits > 64, accumulates per-word results

**Example**:

```cpp
bitcal::bit256 a;
for (size_t i = 0; i < 4; ++i) a[i] = 0xFFFFFFFFFFFFFFFF;
assert(a.popcount() == 256);

bitcal::bit64 b(0xAAAAAAAAAAAAAAAA);
assert(b.popcount() == 32);
```

## count_leading_zeros — Leading Zero Count (CLZ)

```cpp
int count_leading_zeros() const;
```

Count consecutive zeros starting from the most significant bit.

| Input | Return Value |
|-------|--------------|
| All zeros | Bits |
| MSB is 1 | 0 |

**Implementation**: Uses `__builtin_clzll` (GCC/Clang) or `_BitScanReverse64` (MSVC)

## count_trailing_zeros — Trailing Zero Count (CTZ)

```cpp
int count_trailing_zeros() const;
```

Count consecutive zeros starting from the least significant bit.

| Input | Return Value |
|-------|--------------|
| All zeros | Bits |
| LSB is 1 | 0 |

**Implementation**: Uses `__builtin_ctzll` (GCC/Clang) or `_BitScanForward64` (MSVC)

## Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 arr;
    arr[0] = 0x00FFFFFFFFFFFFFFFF;  // 56 trailing zeros, 8 leading zeros
    arr[3] = 0xFF00000000000000;     // Upper byte set

    std::cout << "Popcount: " << arr.popcount() << std::endl;
    std::cout << "Leading zeros: " << arr.count_leading_zeros() << std::endl;
    std::cout << "Trailing zeros: " << arr.count_trailing_zeros() << std::endl;

    // Edge case: all zeros
    bitcal::bit256 zero;
    std::cout << "CLZ (all zeros): " << zero.count_leading_zeros() << std::endl;  // 256

    return 0;
}
```

## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|------------|-------|
| popcount | O(Bits/64) | Single instruction per 64-bit word |
| CLZ/CTZ | O(Bits/64) | Branch-free implementation |

## Platform Notes

- **GCC/Clang**: Uses compiler intrinsics for optimal code generation
- **MSVC**: Uses `_BitScanReverse64` / `_BitScanForward64` on x64
- **All platforms**: Falls back to portable implementation if intrinsics unavailable

## Functional Interface

```cpp
// Operate on raw uint64_t arrays
uint64_t bitcal::ops::popcount<256>(const uint64_t* data);
int bitcal::ops::count_leading_zeros<256>(const uint64_t* data);
int bitcal::ops::count_trailing_zeros<256>(const uint64_t* data);
```

Use the functional interface when working with existing data structures without creating `bitarray` objects.

## Algorithm Details

### popcount

Per 64-bit word:
```cpp
// GCC/Clang
__builtin_popcountll(word);

// MSVC x64
__popcnt64(word);
```

### CLZ

Per 64-bit word:
```cpp
// GCC/Clang
__builtin_clzll(word);

// MSVC x64
unsigned long index;
_BitScanReverse64(&index, word);
return 63 - index;  // For non-zero word
```

For multi-word arrays, finds the first non-zero word from MSB and counts leading zeros in that word, plus 64 for each preceding zero word.

### CTZ

Similar to CLZ but searches from LSB.
