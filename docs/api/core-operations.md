# Core Operations

## Bitwise Operations

All bitwise operations are automatically dispatched to the optimal SIMD implementation.

### AND

```cpp
bitarray operator&(const bitarray& other) const;
bitarray& operator&=(const bitarray& other);
```

Bitwise AND operation.

### OR

```cpp
bitarray operator|(const bitarray& other) const;
bitarray& operator|=(const bitarray& other);
```

Bitwise OR operation.

### XOR

```cpp
bitarray operator^(const bitarray& other) const;
bitarray& operator^=(const bitarray& other);
```

Bitwise XOR operation.

### NOT

```cpp
bitarray operator~() const;
```

Bitwise NOT (inversion).

**SIMD Acceleration** (v2.1+):
- SSE: `_mm_xor_si128`
- AVX: `_mm256_xor_si256`
- NEON: `veorq_u64`

### ANDNOT

```cpp
bitarray andnot(const bitarray& mask) const;
```

Computes `*this & ~mask`, faster than performing `~mask` then `&` separately.

**Native SIMD Instructions**:
| Backend | Instruction |
|---------|-------------|
| SSE | `_mm_andnot_si128` |
| AVX | `_mm256_andnot_si256` |
| NEON | `vbicq_u64` |

**Example**:

```cpp
bitcal::bit256 data(0xFFFF0000FFFF0000);
bitcal::bit256 mask(0xFF00FF00FF00FF00);

auto result = data.andnot(mask);
// Equivalent to: data & ~mask
// = 0xFFFF0000FFFF0000 & 0x00FF00FF00FF00FF
// = 0x00FF000000FF0000
```

## Comparison

### Equality

```cpp
bool operator==(const bitarray& other) const;
```

### Inequality

```cpp
bool operator!=(const bitarray& other) const;
```

## State Detection

### is_zero

```cpp
bool is_zero() const;
```

Check if all bits are zero.

**SIMD Acceleration** (v2.1+):
- SSE: `_mm_movemask_epi8` + `_mm_cmpeq_epi8`
- AVX: `_mm256_testz_si256`
- NEON: `vceqq_u64` + lane checking

```cpp
bitcal::bit256 arr;
if (arr.is_zero()) {
    std::cout << "All bits are zero\n";
}
```

### clear

```cpp
void clear();
```

Clear all bits to zero. Internally uses `std::memset`.

```cpp
bitcal::bit256 arr(0xFFFFFFFF);
arr.clear();  // All bits become 0
```

## Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 a(0xFF00);
    bitcal::bit256 b(0x0FF0);

    // Basic bitwise operations
    auto and_result = a & b;    // 0x0F00
    auto or_result  = a | b;    // 0xFFF0
    auto xor_result = a ^ b;    // 0xF0F0
    auto not_result = ~a;       // ~0xFF00

    // ANDNOT
    auto andnot_result = a.andnot(b);  // a & ~b = 0xF000

    // Compound assignments
    a &= b;
    a |= b;
    a ^= b;

    // Comparison
    if (a == b) {
        std::cout << "a equals b\n";
    }

    // State detection
    a.clear();
    if (a.is_zero()) {
        std::cout << "a is zero\n";
    }

    return 0;
}
```

## Performance Tips

1. **Use ANDNOT** — Approximately 2× faster than separate NOT + AND
2. **Use is_zero** — SIMD-accelerated, faster than manual loop checking
3. **Use compound assignments** — `a &= b` may be more efficient than `a = a & b` (avoids temporary objects)
