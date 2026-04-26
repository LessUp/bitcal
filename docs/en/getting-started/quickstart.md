# Quick Start Guide

Get up and running with BitCal in 5 minutes.

## Table of Contents

- [First Program](#your-first-bitcal-program)
- [Core Concepts](#core-concepts)
- [API Overview](#api-overview)
- [Performance Tips](#performance-tips)
- [Next Steps](#next-steps)

---

## Your First BitCal Program

### 1. Create `hello_bitcal.cpp`

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // Create two 256-bit bit arrays
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);
    
    // SIMD-accelerated bitwise operations
    auto c = a & b;  // AND
    auto d = a | b;  // OR
    auto e = a ^ b;  // XOR
    
    // Count set bits (popcount)
    std::cout << "a & b has " << c.popcount() << " bits set\n";
    std::cout << "a | b has " << d.popcount() << " bits set\n";
    std::cout << "a ^ b has " << e.popcount() << " bits differ\n";
    
    return 0;
}
```

### 2. Compile and Run

```bash
g++ -std=c++17 -O2 -march=native hello_bitcal.cpp -o hello_bitcal
./hello_bitcal
```

**Expected Output:**
```
a & b has 11 bits set
a | b has 24 bits set
a ^ b has 13 bits differ
```

> 💡 **Tip:** `-march=native` enables all SIMD instructions supported by your CPU.

---

## Core Concepts

### BitArray Types

BitCal provides predefined type aliases for common bit widths:

| Type | Bit Width | Words | Typical Use Case |
|------|-----------|-------|------------------|
| `bit64` | 64 | 1 | Machine word |
| `bit128` | 128 | 2 | SSE2/NEON native |
| `bit256` | 256 | 4 | AVX2 native |
| `bit512` | 512 | 8 | Large data |
| `bit1024` | 1024 | 16 | Very large data |

```cpp
// Predefined types
bitcal::bit64 small;
bitcal::bit256 medium;
bitcal::bit1024 large;

// Custom width (must be multiple of 64)
bitcal::bitarray<2048> custom;
```

### SIMD Backend Selection

BitCal automatically selects the optimal backend:

```cpp
// Automatic selection
bitcal::bit256 arr;  // AVX2 on Intel, NEON on ARM

// Explicit backend selection
bitcal::bitarray<256, bitcal::simd_backend::avx2>   avx_arr;
bitcal::bitarray<256, bitcal::simd_backend::neon>   neon_arr;
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable_arr;
```

**Backend Detection:**
```cpp
auto backend = bitcal::get_default_backend();
std::cout << "Using: " << (backend == bitcal::simd_backend::avx2 ? "AVX2" : "Other");
```

### Memory Layout

```
bit256 (4 × 64-bit words):
┌──────────────────────────────────────────────────────────┐
│ data_[0] │ data_[1] │ data_[2] │ data_[3] │  ...padding...│
│ bits 0-63│bits 64-127│bits 128-191│bits 192-255│align to 64B│
└──────────────────────────────────────────────────────────┘
     ↓ Low                                              High ↓
```

---

## API Overview

### 1. Bitwise Operations

```cpp
bitcal::bit256 a(0xFF00);
bitcal::bit256 b(0x0FF0);

// Binary operations (all return new bitarray)
auto and_result = a & b;       // 0x0F00
auto or_result  = a | b;       // 0xFFF0
auto xor_result = a ^ b;       // 0xF0F0
auto not_result = ~a;          // ~0xFF00
auto andnot     = a.andnot(b); // a & ~b = 0xF000 (fast!)

// Compound assignment
a &= b;  // Same as: a = a & b (may avoid temporary)
a |= b;
a ^= b;
```

### 2. Shift Operations

```cpp
bitcal::bit256 arr(1);

// Within-word shift (fastest)
arr <<= 10;            // Left shift
arr >>= 5;             // Right shift

// Cross-word shift (handles carry)
arr.shift_left(128);   // Shift across word boundary
arr.shift_right(64);

// Notes:
// - shift >= Bits results in zero
// - Negative shift is undefined
```

### 3. Bit Counting

```cpp
bitcal::bit256 arr(0xF0F0F0F0F0F0F0F0);

// Population count (count set bits)
uint64_t ones = arr.popcount();

// Leading zeros
int lz = arr.count_leading_zeros();

// Trailing zeros  
int tz = arr.count_trailing_zeros();
```

### 4. Single Bit Operations

```cpp
bitcal::bit256 arr;

// Get/Set/Flip
arr.set_bit(42, true);   // Set bit 42 to 1
arr.set_bit(42, false);  // Clear bit 42
bool bit = arr.get_bit(42);   // Read bit 42
arr.flip_bit(42);        // Toggle bit 42

// Bounds checking: bits [0, Bits-1]
// Out-of-range access is undefined behavior
```

### 5. Bulk Operations

```cpp
bitcal::bit256 arr(0x123456789ABCDEF0);

arr.reverse();         // Reverse all bits
arr.clear();           // Set all bits to 0 (fast SIMD memset)

if (arr.is_zero()) {   // Fast SIMD zero detection
    // All bits are zero
}
```

### 6. Data Access

```cpp
bitcal::bit256 arr(0xDEADBEEF);

// Direct word access
arr[0] = 0x12345678;   // Set first 64 bits
arr[1] = 0x9ABCDEF0;
uint64_t word = arr[0]; // Get word

// Pointer access
uint64_t* data = arr.data();
data[0] = 0xFFFFFFFFFFFFFFFF;

// Type info
constexpr size_t bits = bitcal::bit256::bits;      // 256
constexpr size_t words = bitcal::bit256::u64_count; // 4
```

### 7. Low-level API (Ops Namespace)

For raw pointer operations without object overhead:

```cpp
#include <bitcal/ops.hpp>

uint64_t data[4] = {1, 2, 3, 4};

// Functional API
uint64_t ones = bitcal::ops::popcount<256>(data);
int lz = bitcal::ops::count_leading_zeros<256>(data);

// Utility functions
uint64_t reversed = bitcal::ops::reverse_bits_64(0xF0F0F0F0);
uint64_t swapped = bitcal::ops::byte_swap_64(0x12345678);
```

---

## Performance Tips

### 1. Compiler Flags Matter

```bash
# Fastest on your machine
g++ -std=c++17 -O3 -march=native program.cpp

# Portable with good performance
g++ -std=c++17 -O3 -msse2 program.cpp
```

### 2. Prefer ANDNOT

```cpp
// Fast (single instruction)
auto fast = a.andnot(b);  // ~2.1ns

// Slow (two instructions + temporary)
auto slow = a & ~b;       // ~4.5ns
```

### 3. Use Compound Assignments

```cpp
// May avoid temporary object
a &= b;  // Potentially faster

// Creates temporary
a = a & b;
```

### 4. Use is_zero() for Comparisons

```cpp
// Fast SIMD path (~1.8ns with AVX2)
if (arr.is_zero()) { ... }

// Slow manual comparison
bool zero = (arr.popcount() == 0);  // Much slower
```

### 5. Understand Alignment

```cpp
// BitCal types are 64-byte aligned
alignas(64) bitcal::bit256 arr;  // Optimal

// Stack allocation is fine for normal use
bitcal::bit256 arr;  // Still 64-byte aligned
```

---

## Complete Example: Bitmap Operations

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>
#include <vector>

// 1024-bit bitmap for tracking 1024 flags
using Bitmap = bitcal::bit1024;

class UserPermissions {
    Bitmap permissions_;
public:
    enum Permission { READ = 0, WRITE = 1, EXECUTE = 2, ADMIN = 3 };
    
    void grant(Permission p) { permissions_.set_bit(p, true); }
    void revoke(Permission p) { permissions_.set_bit(p, false); }
    bool has(Permission p) const { return permissions_.get_bit(p); }
    bool none() const { return permissions_.is_zero(); }
    
    Bitmap get() const { return permissions_; }
    void merge(const UserPermissions& other) {
        permissions_ |= other.permissions_;
    }
};

int main() {
    UserPermissions alice, bob;
    
    alice.grant(UserPermissions::READ);
    alice.grant(UserPermissions::WRITE);
    
    bob.grant(UserPermissions::EXECUTE);
    
    std::cout << "Alice has " << alice.get().popcount() << " permissions\n";
    
    // Union of permissions
    alice.merge(bob);
    std::cout << "After merge: " << alice.get().popcount() << " permissions\n";
    
    return 0;
}
```

---

## Next Steps

| Topic | Document | Description |
|-------|----------|-------------|
| Complete API | [Types](../api/types.md) | All types and their details |
| Operations | [Core Operations](../api/core-operations.md) | Bitwise operations reference |
| Performance | [Architecture](../architecture/overview.md) | Design and optimization |
| Troubleshooting | [Installation](installation.md) | Common issues and solutions |

---

<p align="center">
  <sub>Ready for the full API reference? → <a href="../api/types.md">Continue to Types</a></sub>
</p>
