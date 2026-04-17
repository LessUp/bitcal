# Shift Operations

## Left Shift

```cpp
void shift_left(int count);
bitarray operator<<(int count) const;
bitarray& operator<<=(int count);
```

Shift all bits toward more significant positions (left), filling with zeros. Bits shifted beyond the width are discarded.

**SIMD Dispatch**:
- 64-bit: Scalar
- 128-bit: SSE2 / NEON
- 256-bit: AVX2 / SSE2(2×128) / NEON(2×128)
- 512-bit: AVX2(2×256) / NEON(4×128)
- Others: Scalar array fallback

## Right Shift

```cpp
void shift_right(int count);
bitarray operator>>(int count) const;
bitarray& operator>>=(int count);
```

Shift all bits toward less significant positions (right), filling with zeros (logical right shift).

## Boundary Behavior

| Condition | Behavior |
|-----------|----------|
| `count == 0` | No operation |
| `count >= Bits` | All bits cleared |
| `count < 0` | Undefined behavior |

## Examples

```cpp
bitcal::bit256 a;
a[0] = 0x8000000000000000ULL;  // bit 63 = 1

a.shift_left(1);
// a[0] == 0, a[1] == 1  (carry correctly propagates to next word)

bitcal::bit256 b(1);
b <<= 255;
// b[3] == 0x8000000000000000  (most significant bit)
```

## Implementation Details

Shift operations are executed in two phases:
1. **Word-level shift** (count ≥ 64): Scalar whole-word movement
2. **Bit-level shift** (count < 64): SIMD parallel shift + carry propagation

### AVX2 256-bit Left Shift Example

```
Phase 1: word-shift (scalar)
  data[3] = data[2], data[2] = data[1], data[1] = data[0], data[0] = 0

Phase 2: bit-shift (AVX2)
  shifted = _mm256_slli_epi64(v, count)          // Each qword shifts independently
  carry   = _mm256_permute4x64_epi64(v, 0x93)   // Rotate: [q3,q2,q1,q0] -> [q0,q3,q2,q1]
  carry   = _mm256_srli_epi64(carry, 64-count)  // Extract carry bits
  carry   = _mm256_blend_epi32(carry, zero, 0x03) // Clear carry in lowest qword
  result  = shifted | carry
```

## Why Not `_mm256_slli_si256`?

`_mm256_slli_si256` operates **independently on 128-bit lanes** and does not shift across lanes. This is a common AVX2 pitfall. BitCal v2.1 fixes this by using the scalar word-shift + SIMD bit-shift strategy.

## Performance Notes

- Shifts by multiples of 64 are fastest (pure word movement)
- Cross-word carries add overhead but are necessary for correctness
- AVX2 256-bit shifts are significantly faster than scalar for most shift amounts

## See Also

- [Types](types.html) — Available bitarray types
- [Architecture](../architecture/simd-dispatch.html) — SIMD dispatch details
