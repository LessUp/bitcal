# Core Operations

This page documents the retained vNext core algorithm surface.

## Public glossary

| Term | Meaning |
| --- | --- |
| Owning block | `bit_block<Bits>` returned or mutated by an algorithm |
| Non-owning view | `bit_view` / `const_bit_view` passed into an algorithm |
| Free algorithm | a namespace-level function such as `bit_and<Bits>()` |
| Backend boundary | the internal execution layer behind those algorithms |

## `and_into()`

```cpp
void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept;
```

### Use it when

- you already have an output block or external buffer
- you want to avoid allocating a new owning result

### Behavior

- requires matching word counts for `lhs`, `rhs`, and `out`
- writes the bitwise AND result into `out`
- crosses the backend boundary to the current implementation path

## `bit_and<Bits>()`

```cpp
template <std::size_t Bits>
[[nodiscard]] bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept;
```

### Use it when

- you want a new owning result block
- the result width is known at compile time

### Behavior

- requires both input views to match `bit_block<Bits>::word_count`
- internally reuses `and_into()` with a newly created owning block

## `is_zero()`

```cpp
[[nodiscard]] constexpr bool is_zero(const const_bit_view value) noexcept;
```

Returns `true` when every word in the view is zero.

## `popcount()`

```cpp
[[nodiscard]] constexpr std::uint64_t popcount(const const_bit_view value) noexcept;
```

Returns the total number of set bits across the entire view.

## Composition example

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
bitcal::bit_block<256> scratch;

auto produced = bitcal::bit_and<256>(lhs.view(), rhs.view());
bitcal::and_into(lhs.view(), rhs.view(), scratch.view());

auto empty = bitcal::is_zero(produced.view());
auto ones = bitcal::popcount(scratch.view());
```

## Choosing the right shape

| Need | Prefer |
| --- | --- |
| Return a fresh owning result | `bit_and<Bits>()` |
| Reuse existing writable storage | `and_into()` |
| Read-only check | `is_zero()` |
| Read-only counting query | `popcount()` |

## Notes on assertions and safety

The write-oriented algorithms rely on debug assertions for word-count agreement. That means callers should treat width matching as part of the contract, not as an optional runtime convenience.

## Backend boundary reminder

These algorithms define the public behavior. The exact SIMD kernel that executes them is intentionally an implementation detail documented in [SIMD Dispatch](../architecture/simd-dispatch.md).

## Read next

- [Types Reference](./types.md)
- [SIMD Dispatch](../architecture/simd-dispatch.md)
- [Performance Baseline](../architecture/performance-baseline.md)
