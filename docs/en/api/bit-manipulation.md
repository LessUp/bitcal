# Bit Manipulation

## get_bit — Read Single Bit

```cpp
bool get_bit(size_t bit_index) const;
```

Read the value of bit at position `bit_index` (0-indexed, from least significant bit).

**Parameters**:
- `bit_index`: Position of bit to read (0 to Bits-1)

**Returns**: `true` if bit is set, `false` otherwise

**Complexity**: O(1)

## set_bit — Set Single Bit

```cpp
void set_bit(size_t bit_index, bool value = true);
```

Set the bit at position `bit_index`. `value=true` sets to 1, `value=false` clears to 0.

**Parameters**:
- `bit_index`: Position of bit to modify
- `value`: New value (default: true)

**Complexity**: O(1)

## flip_bit — Toggle Single Bit

```cpp
void flip_bit(size_t bit_index);
```

Toggle (invert) the bit at position `bit_index` (0→1, 1→0).

**Parameters**:
- `bit_index`: Position of bit to flip

**Complexity**: O(1)

## reverse — Bit Reversal

```cpp
void reverse();
```

Reverse all bits: bit 0 swaps with bit (Bits-1), bit 1 swaps with bit (Bits-2), etc.

**Implementation**: In-place operation using swap from both ends (v2.1+). Previous versions used a temporary array.

**Complexity**: O(Bits/64)

## Examples

### Single Bit Operations

```cpp
bitcal::bit256 arr;

arr.set_bit(0);      // Set bit 0 to 1
arr.set_bit(255);    // Set bit 255 to 1
arr.set_bit(100, false);  // Clear bit 100

bool b0 = arr.get_bit(0);    // true
bool b100 = arr.get_bit(100); // false

arr.flip_bit(0);     // Toggle bit 0 → now 0
arr.flip_bit(42);    // Toggle bit 42 → now 1
```

### Bit Reversal

```cpp
bitcal::bit64 x(1);   // bit 0 = 1
x.reverse();           // Now bit 63 = 1
// x[0] == 0x8000000000000000

bitcal::bit256 y;
y[0] = 0x0000000000000001;  // bit 0 = 1
y[3] = 0x8000000000000000;  // bit 255 = 1
y.reverse();
// After reverse: bit 0 = 1 (was bit 255), bit 255 = 1 (was bit 0)
```

## Implementation Details

### Access Pattern

```cpp
// Internal implementation
bool get_bit(size_t bit_index) const {
    size_t word = bit_index / 64;
    size_t bit = bit_index % 64;
    return (data_[word] >> bit) & 1;
}

void set_bit(size_t bit_index, bool value) {
    size_t word = bit_index / 64;
    size_t bit = bit_index % 64;
    if (value) {
        data_[word] |= (1ULL << bit);
    } else {
        data_[word] &= ~(1ULL << bit);
    }
}

void flip_bit(size_t bit_index) {
    size_t word = bit_index / 64;
    size_t bit = bit_index % 64;
    data_[word] ^= (1ULL << bit);
}
```

### Reverse Algorithm

```cpp
// Simplified pseudocode
void reverse() {
    // Step 1: Reverse bits within each 64-bit word
    for (size_t i = 0; i < u64_count; ++i) {
        data_[i] = reverse_bits_64(data_[i]);
    }
    
    // Step 2: Swap words from both ends
    for (size_t i = 0, j = u64_count - 1; i < j; ++i, --j) {
        std::swap(data_[i], data_[j]);
    }
}
```

## Performance Notes

| Operation | Complexity | Notes |
|-----------|------------|-------|
| get_bit | O(1) | Single memory access |
| set_bit | O(1) | Read-modify-write |
| flip_bit | O(1) | XOR operation |
| reverse | O(Bits/64) | Two passes over data |

## Bounds Checking

**Note**: These functions do not perform bounds checking for performance reasons. Passing an `bit_index` >= `Bits` results in undefined behavior.

For debug builds with bounds checking, consider:
```cpp
assert(bit_index < Bits);
arr.set_bit(bit_index, true);
```

## Use Cases

### Bit Flags

```cpp
bitcal::bit1024 feature_flags;

feature_flags.set_bit(FEATURE_A, enabled_a);
feature_flags.set_bit(FEATURE_B, enabled_b);

if (feature_flags.get_bit(FEATURE_A)) {
    // Feature A is enabled
}
```

### Bit Matrix Transpose

```cpp
// Transpose an 8×8 bit matrix using reversals and shifts
bitcal::bit64 matrix;
// ... load matrix bits ...
matrix.reverse();  // Part of transpose operation
```
