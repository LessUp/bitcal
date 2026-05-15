# Types Reference

This page defines the vNext public type glossary.

## Public glossary

| Term | Public API | Role |
| --- | --- | --- |
| Owning block | `bit_block<Bits>` | Fixed-width storage that owns contiguous words |
| Non-owning view | `bit_view` | Mutable borrowed access to words owned elsewhere |
| Read-only view | `const_bit_view` | Borrowed read-only access |
| Backend boundary | `backend_kind`, `default_backend()` | Diagnostic surface that describes execution choice without becoming the storage model |

## Stable include

```cpp
#include <bitcal/bitcal.hpp>
```

That umbrella header is the stable public include seam for the vNext model.

## Owning block: `bit_block<Bits>`

```cpp
template <std::size_t Bits>
class bit_block;
```

### Constraints

- `Bits >= 64`
- `Bits` must be a multiple of 64

### Public constants

| Constant | Meaning |
| --- | --- |
| `bit_block<Bits>::bits` | total bit width |
| `bit_block<Bits>::word_count` | number of 64-bit words |
| `bit_block<Bits>::storage_alignment` | alignment chosen for the owning storage |

On x86 builds today, `storage_alignment` is 32 bytes. On other builds it falls back to `alignof(std::uint64_t)`.

### Public operations

```cpp
constexpr bit_block() noexcept;
static constexpr bit_block from_words(std::span<const std::uint64_t> words) noexcept;
constexpr bit_view view() noexcept;
constexpr const_bit_view view() const noexcept;
constexpr std::uint64_t word(std::size_t index) const noexcept;
constexpr void copy_words_to(std::span<std::uint64_t> out) const noexcept;
```

### Example

```cpp
std::array<std::uint64_t, 4> words{1, 2, 3, 4};
auto block = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(words));
auto first = block.word(0);
```

## Non-owning view: `bit_view`

```cpp
class bit_view {
public:
    constexpr bit_view() noexcept = default;
    constexpr bit_view(std::uint64_t* data, std::size_t words) noexcept;

    constexpr std::uint64_t* data() noexcept;
    constexpr const std::uint64_t* data() const noexcept;
    constexpr std::size_t word_count() const noexcept;
    constexpr std::uint64_t word(std::size_t index) const noexcept;
    constexpr operator const_bit_view() const noexcept;
};
```

Use `bit_view` when you want a free algorithm to write into existing storage.

## Read-only view: `const_bit_view`

```cpp
class const_bit_view {
public:
    constexpr const_bit_view() noexcept = default;
    constexpr const_bit_view(const std::uint64_t* data, std::size_t words) noexcept;

    constexpr const std::uint64_t* data() const noexcept;
    constexpr std::size_t word_count() const noexcept;
    constexpr std::uint64_t word(std::size_t index) const noexcept;
};
```

Use `const_bit_view` for query algorithms and read-only inputs.

## Lifetime rule for views

Views never own memory. The caller is responsible for ensuring the underlying words outlive the view.

In practice, this usually means one of two patterns:

- borrow from an owning block via `view()`
- construct a view over an external buffer that your code already manages

## Backend boundary

```cpp
enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512,
};

constexpr backend_kind default_backend() noexcept;
```

These names are useful for diagnostics, testing notes, and benchmark records. They are not the core of the public storage model.

## How the pieces fit together

```cpp
bitcal::bit_block<256> owned;
auto writable = owned.view();
bitcal::const_bit_view readable = writable;
```

That is the whole public type story: own with a block, borrow with views, observe the backend boundary without turning it into the main abstraction.

## Read next

- [Core Operations](./core-operations.md)
- [Architecture Overview](../architecture/overview.md)
- [vNext Whitepaper](../architecture/vnext-whitepaper.md)
