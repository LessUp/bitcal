# Functional API (ops Namespace)

The `bitcal::ops` namespace provides direct operations on raw `uint64_t*` pointers, suitable for scenarios where constructing `bitarray` objects is unnecessary or undesirable.

## Bit Counting

### popcount

```cpp
template<size_t Bits>
uint64_t popcount(const uint64_t* data);
```

Count the number of set bits in the specified bit-width data.

```cpp
uint64_t data[4] = {0xFF, 0xFF, 0, 0};
uint64_t ones = bitcal::ops::popcount<256>(data);  // 16
```

### count_leading_zeros

```cpp
template<size_t Bits>
int count_leading_zeros(const uint64_t* data);
```

Count leading zeros.

```cpp
uint64_t data[4] = {0, 0, 0, 0x8000000000000000};
int lz = bitcal::ops::count_leading_zeros<256>(data);  // 0 (MSB is 1)
```

### count_trailing_zeros

```cpp
template<size_t Bits>
int count_trailing_zeros(const uint64_t* data);
```

Count trailing zeros.

```cpp
uint64_t data[4] = {1, 0, 0, 0};
int tz = bitcal::ops::count_trailing_zeros<256>(data);  // 0 (LSB is 1)
```

## Utility Functions

### reverse_bits_64

```cpp
uint64_t reverse_bits_64(uint64_t x);
```

Reverse all bits of a single 64-bit integer.

```cpp
uint64_t reversed = bitcal::ops::reverse_bits_64(0xF0F0F0F0);
// reversed = 0x0F0F0F0F0F0F0F0F
```

**Implementation**: Lookup table for speed. Complexity: O(1)

### byte_swap_64

```cpp
uint64_t byte_swap_64(uint64_t x);
```

Swap byte order (big-endian ↔ little-endian).

```cpp
uint64_t swapped = bitcal::ops::byte_swap_64(0x0102030405060708ULL);
// swapped = 0x0807060504030201ULL
```

**Implementation**: Compiler intrinsic or shift/mask. Complexity: O(1)

## Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // Raw data array
    uint64_t data[4] = {0xFF00, 0x00FF, 0xF0F0, 0x0F0F};

    // Bit counting
    std::cout << "popcount: " << bitcal::ops::popcount<256>(data) << std::endl;

    // Leading/trailing zeros
    std::cout << "CLZ: " << bitcal::ops::count_leading_zeros<256>(data) << std::endl;
    std::cout << "CTZ: " << bitcal::ops::count_trailing_zeros<256>(data) << std::endl;

    // Bit reversal
    uint64_t x = 0x123456789ABCDEF0ULL;
    std::cout << "reversed: 0x" << std::hex << bitcal::ops::reverse_bits_64(x) << std::endl;

    // Byte swap
    std::cout << "swapped: 0x" << std::hex << bitcal::ops::byte_swap_64(x) << std::endl;

    return 0;
}
```

## Use Cases

### Integration with Existing Data Structures

```cpp
struct MyBitmap {
    uint64_t data[8];  // 512 bits
    
    uint64_t count_bits() const {
        return bitcal::ops::popcount<512>(data);
    }
};
```

### Batch Processing

```cpp
void process_bitmaps(const uint64_t** bitmaps, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        auto ones = bitcal::ops::popcount<256>(bitmaps[i]);
        // Process count...
    }
}
```

### Low-level Library Development

```cpp
// In a custom bit manipulation library
void custom_reverse(uint64_t* data, size_t bit_width) {
    // Use ops functions for known widths
    switch (bit_width) {
        case 64:  data[0] = bitcal::ops::reverse_bits_64(data[0]); break;
        // ...
    }
}
```

## Performance

All `ops` functions are force-inlined, providing the same performance as `bitarray` member functions. The compiler can often eliminate the function call overhead entirely.

| Function | Performance | Notes |
|----------|-------------|-------|
| popcount | Same as bitarray | Uses same algorithm |
| CLZ/CTZ | Same as bitarray | Uses same algorithm |
| reverse_bits_64 | ~1-2 cycles | Lookup table |
| byte_swap_64 | ~1 cycle | BSWAP instruction on x86 |

## Memory Alignment

The input pointer should be aligned to `alignof(uint64_t)` (8 bytes). For optimal SIMD performance on x86/ARM, 32 or 64-byte alignment is recommended.

## Differences from bitarray API

| Aspect | ops namespace | bitarray class |
|--------|--------------|----------------|
| Input | Raw pointer | Member function |
| Type safety | Template parameter | Encapsulated |
| Convenience | Lower | Higher |
| Use case | Low-level, existing data | New development |

## See Also

- [Bit Counting](bit-counting.html) — bitarray bit counting API
- [Types](types.html) — bitarray class documentation
