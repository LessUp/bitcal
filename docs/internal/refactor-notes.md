# BitCal Refactoring Notes

## Overview

BitCal 2.x represents a complete redesign from OOP inheritance to modern C++17 template design.

## Design Comparison

### Old Design (v1.x)

```cpp
// Inheritance-based with virtual functions
class BitCalBase { virtual uint64_t bitAnd64(...) = 0; };
class BitCalGpr : public BitCalBase { ... };
class BitCalXmm : public BitCalBase { ... };

// Runtime dispatch via singleton
BitCal::getInstance().bitAnd64(a, b);  // Virtual function overhead
```

### New Design (v2.x)

```cpp
// Compile-time template specialization
template<size_t Bits, simd_backend Backend>
class bitarray { ... };

// Zero-overhead abstraction
bitcal::bit256 a, b;
auto c = a & b;  // Inlined to optimal SIMD instruction
```

## Key Changes

| Aspect | Old Design | New Design |
|--------|-----------|-----------|
| **Architecture** | Inheritance + virtual functions | Templates + if constexpr |
| **Dispatch** | Runtime | Compile-time |
| **Distribution** | Requires compilation | Header-only |
| **API** | Method calls | Operator overloading |
| **SIMD** | Manual selection | Automatic selection |

## File Naming Convention

All files use lowercase with underscores:

```
include/bitcal/
├── bitcal.hpp       # Main entry point
├── config.hpp       # Platform configuration
├── simd_traits.hpp  # SIMD type traits
├── scalar_ops.hpp   # Scalar fallback
├── sse_ops.hpp      # SSE2 implementation
├── avx_ops.hpp      # AVX2 implementation
└── neon_ops.hpp     # NEON implementation
```

## Dependency Management

- ❌ Removed spdlog dependency
- ✅ Uses standard library only (`<iostream>`, `<stdexcept>`)
- ✅ Zero external dependencies

## Design Best Practices

The new design follows modern C++ library conventions:

1. ✅ **Header-only** — Easy integration
2. ✅ **Compile-time dispatch** — Zero runtime overhead
3. ✅ **Template interface** — Type safety
4. ✅ **Zero dependencies** — Standard library only

## Reference Projects

| Library | Characteristics |
|---------|----------------|
| Google Highway | Modern SIMD abstraction |
| xsimd | C++17 template design |
| simde | SIMD emulation |

## Summary

BitCal 2.x achieves:

- 🚀 **Faster**: Compile-time optimization
- 📦 **Simpler**: Header-only distribution
- 🔒 **Safer**: Template type checking
- 🌍 **Broader**: Full ARM support
