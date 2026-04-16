# BitCal 2.0 Migration Guide

This guide helps you migrate from legacy BitCal v1.x to the modern v2.x design.

## Quick Migration Checklist

- [ ] Update include path: `#include "BitCal.h"` → `#include <bitcal/bitcal.hpp>`
- [ ] Remove singleton calls: `BitCal::getInstance()` → use types and operators directly
- [ ] Update CMakeLists.txt
- [ ] Test compilation and execution
- [ ] Verify performance

## Migration Patterns

### Pattern 1: Shift Operations

**Old Code (v1.x)**:
```cpp
uint64_t data[2] = {0x123, 0x456};
uint64_t carryout;
BitCal::getInstance().bitShiftLeft128(data, 10, &carryout);
```

**New Code (v2.x)**:
```cpp
bitcal::bit128 data(0x123);
data[1] = 0x456;
data.shift_left(10);
// Carry can be checked by examining the highest bit before shift
```

### Pattern 2: Logical Operations

**Old Code (v1.x)**:
```cpp
uint64_t a[4], b[4], result[4];
// Initialize a, b...
BitCal::getInstance().bitAnd256(a, b, result);
```

**New Code (v2.x)**:
```cpp
bitcal::bit256 a, b;
// Initialize a, b...
auto result = a & b;
```

### Pattern 3: Bit Counting

**Old Code (v1.x)**:
```cpp
uint64_t data[8];
uint8_t cnt_arr[8];
uint64_t total = BitCal::getInstance().bitPopcnt512(data, cnt_arr);
```

**New Code (v2.x)**:
```cpp
bitcal::bit512 data;
// Initialize data...
uint64_t total = data.popcount();
// Per-word counts available via iteration
for (size_t i = 0; i < 8; ++i) {
    uint64_t count = bitcal::ops::popcount<64>(&data[i]);
}
```

### Pattern 4: Bit Reversal

**Old Code (v1.x)**:
```cpp
uint64_t input[4], output[4];
BitCal::getInstance().bitReverse256(input, output);
```

**New Code (v2.x)**:
```cpp
bitcal::bit256 data;
// Initialize data...
data.reverse();  // In-place reversal
```

## New Features in v2.x

### Single Bit Operations

```cpp
bitcal::bit256 arr;

arr.set_bit(42, true);    // Set bit 42 to 1
bool bit = arr.get_bit(42);   // Read bit 42
arr.flip_bit(42);         // Toggle bit 42
```

### Counting Operations

```cpp
bitcal::bit256 arr;

int clz = arr.count_leading_zeros();
int ctz = arr.count_trailing_zeros();
uint64_t ones = arr.popcount();
```

### Operator Overloading

```cpp
bitcal::bit256 a, b;

auto c = a & b;   // AND
auto d = a | b;   // OR
auto e = a ^ b;   // XOR
auto f = ~a;      // NOT
auto g = a << 10; // Left shift
auto h = a >> 5;  // Right shift

a &= b;           // Compound assignment
if (a == b) { }   // Comparison
```

## CMakeLists.txt Migration

**Old Configuration (v1.x)**:
```cmake
add_library(bitcal
  BitCal.cpp
  BitCalGpr.cpp
  # ...
)
target_link_libraries(your_app bitcal)
```

**New Configuration (v2.x)**:
```cmake
# Header-only: just add include path
target_include_directories(your_app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Or use installed version
find_package(bitcal REQUIRED)
target_link_libraries(your_app bitcal::bitcal)
```

## Performance Optimization

### SIMD Backend Selection

```cpp
// Automatic selection (recommended)
bitcal::bit256 auto_opt;

// Force specific backend
#if BITCAL_HAS_AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> avx2_opt;
#endif

#if BITCAL_HAS_NEON
bitcal::bitarray<128, bitcal::simd_backend::neon> neon_opt;
#endif

// Portable scalar
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable;
```

### Compiler Flags

**x86/x64**:
```bash
g++ -std=c++17 -O3 -march=native main.cpp
# Or: g++ -std=c++17 -O3 -mavx2 main.cpp
```

**ARM**:
```bash
# AArch64
g++ -std=c++17 -O3 -march=armv8-a+simd main.cpp

# ARMv7 with NEON
g++ -std=c++17 -O3 -mfpu=neon main.cpp
```

## Compatibility Wrapper (Temporary)

If you have large codebase to migrate:

```cpp
// bitcal_compat.hpp
#pragma once
#include <bitcal/bitcal.hpp>

class BitCalCompat {
public:
    static BitCalCompat& getInstance() {
        static BitCalCompat instance;
        return instance;
    }

    uint64_t bitShiftLeft64(uint64_t* data, int count) {
        bitcal::bit64 arr(*data);
        uint64_t carry = (count > 0 && count < 64) ? (*data >> (64 - count)) : 0;
        arr.shift_left(count);
        *data = arr[0];
        return carry;
    }

    // Add other compat functions as needed...
};

using BitCal = BitCalCompat;
```

## Validation

After migration:

```cpp
#include <bitcal/bitcal.hpp>
#include <cassert>

void validate_migration() {
    bitcal::bit64 a(0xFF00FF00);
    bitcal::bit64 b(0xF0F0F0F0);

    auto c = a & b;
    assert(c[0] == 0xF000F000);

    a.shift_left(8);
    assert(a[0] == 0xFF00FF0000);

    std::cout << "Migration validated!" << std::endl;
}
```

## Rollback Plan

If issues arise:

1. Keep old code on separate branch
2. Use compatibility wrapper temporarily
3. Migrate incrementally

## See Also

- [REFACTOR_NOTES.md](internal/refactor-notes.md) — Design rationale
- [CHANGELOG.md](../../CHANGELOG.md) — Version history
- [API Reference](../api/types.md) — Complete API documentation
