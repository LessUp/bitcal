# Bit Counting

Reference for the retained BitCal 3.0 bit-counting member functions.

> **BitCal 3.0 migration:** the public `bitcal::ops::{popcount,count_leading_zeros,count_trailing_zeros}` helpers were removed. Wrap data in `bitarray` and call the member functions documented here.

## popcount — Count Set Bits

```cpp
uint64_t popcount() const noexcept;
```

Counts the number of bits set to `1`.

**Implementation:**
- Uses scalar builtins per 64-bit word
- Delegates to the selected backend for accumulation

**Example:**

```cpp
bitcal::bit256 a;
a.set_word(0, 0xFFFFFFFFFFFFFFFFULL);
a.set_word(1, 0xFFFFFFFFFFFFFFFFULL);
a.set_word(2, 0xFFFFFFFFFFFFFFFFULL);
a.set_word(3, 0xFFFFFFFFFFFFFFFFULL);
assert(a.popcount() == 256);

bitcal::bit64 b(0xAAAAAAAAAAAAAAAAULL);
assert(b.popcount() == 32);
```

## count_leading_zeros — Leading Zero Count (CLZ)

```cpp
int count_leading_zeros() const noexcept;
```

Counts consecutive zero bits starting from the most significant bit.

| Input | Return Value |
|-------|--------------|
| All zeros | `Bits` |
| MSB is 1 | `0` |

## count_trailing_zeros — Trailing Zero Count (CTZ)

```cpp
int count_trailing_zeros() const noexcept;
```

Counts consecutive zero bits starting from the least significant bit.

| Input | Return Value |
|-------|--------------|
| All zeros | `Bits` |
| LSB is 1 | `0` |

## Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 arr;
    arr.set_word(0, 0x0000000000000100ULL);
    arr.set_word(3, 0x8000000000000000ULL);

    std::cout << "Popcount: " << arr.popcount() << "\n";
    std::cout << "Leading zeros: " << arr.count_leading_zeros() << "\n";
    std::cout << "Trailing zeros: " << arr.count_trailing_zeros() << "\n";

    bitcal::bit256 zero;
    std::cout << "CLZ (all zeros): " << zero.count_leading_zeros() << "\n";
    std::cout << "CTZ (all zeros): " << zero.count_trailing_zeros() << "\n";

    return 0;
}
```

## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|------------|-------|
| `popcount()` | `O(Bits / 64)` | One count per 64-bit word |
| `count_leading_zeros()` | `O(Bits / 64)` | Scans from the most-significant word |
| `count_trailing_zeros()` | `O(Bits / 64)` | Scans from the least-significant word |

## Platform Notes

- GCC/Clang builds use builtin bit-counting operations where available
- MSVC builds use the matching scalar intrinsics in the implementation layer
- Return values are width-aware and remain valid for every supported `bitarray<Bits>` specialization

## Migration Notes

If you previously counted bits over a raw `uint64_t*` buffer with removed `bitcal::ops` APIs, adapt the buffer into a `bitarray` and call the member functions instead:

```cpp
bitcal::bit256 arr;
for (size_t i = 0; i < bitcal::bit256::u64_count; ++i) {
    arr.set_word(i, source[i]);
}

uint64_t ones = arr.popcount();
```
