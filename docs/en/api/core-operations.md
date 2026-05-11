# Core Operations

Reference for the retained BitCal 3.0 core bitwise surface.

> **BitCal 3.0 migration:** all public core operations are expressed through `bitarray` operators and member functions. The removed `bitcal::ops` namespace is no longer part of the supported public API.

## Bitwise Operations

All bitwise operations dispatch to the backend chosen for the `bitarray` specialization.

### AND

```cpp
bitarray operator&(const bitarray& other) const noexcept;
bitarray& operator&=(const bitarray& other) noexcept;
```

### OR

```cpp
bitarray operator|(const bitarray& other) const noexcept;
bitarray& operator|=(const bitarray& other) noexcept;
```

### XOR

```cpp
bitarray operator^(const bitarray& other) const noexcept;
bitarray& operator^=(const bitarray& other) noexcept;
```

### NOT

```cpp
bitarray operator~() const noexcept;
```

Bitwise inversion of every bit.

### ANDNOT

```cpp
bitarray andnot(const bitarray& mask) const noexcept;
```

Computes `*this & ~mask` using the selected backend implementation.

## Comparison

```cpp
bool operator==(const bitarray& other) const noexcept;
bool operator!=(const bitarray& other) const noexcept;
```

## State Detection

### is_zero

```cpp
bool is_zero() const noexcept;
```

Returns `true` when every bit is clear.

### clear

```cpp
void clear() noexcept;
```

Clears the entire object to zero.

## Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 a(0xFF00);
    bitcal::bit256 b(0x0FF0);

    auto and_result = a & b;
    auto or_result = a | b;
    auto xor_result = a ^ b;
    auto not_result = ~a;
    auto andnot_result = a.andnot(b);

    a &= b;
    a |= b;
    a ^= b;

    if (and_result != or_result && !not_result.is_zero()) {
        std::cout << "core operations succeeded\n";
    }

    andnot_result.is_zero();
    a.clear();
    std::cout << std::boolalpha << a.is_zero() << "\n";
    return 0;
}
```

## Performance Tips

1. Prefer `andnot()` instead of spelling `a & ~b` when the intent matches
2. Prefer compound assignments when updating an existing object in place
3. Keep values in `bitarray` form instead of dropping to ad-hoc raw pointer helpers
