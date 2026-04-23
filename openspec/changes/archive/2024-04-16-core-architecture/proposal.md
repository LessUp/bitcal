# Proposal: Core Architecture - SIMD-Accelerated Bit Array

## Why

BitCal v1.x used an object-oriented inheritance design with runtime dispatch that had several limitations:
- Virtual function call overhead on every operation
- Runtime backend selection
- Difficult to compile for specific SIMD widths
- Required compilation (not header-only)
- Complex inheritance hierarchy

## What

Redesign BitCal to use C++17 templates with compile-time dispatch:
- Template-based `bitarray<N>` class
- Compile-time SIMD backend selection
- Header-only library
- Zero-overhead abstraction

## Impact

- Complete rewrite of core architecture
- Breaking changes from v1.x API
- New type aliases: `bit64`, `bit128`, `bit256`, `bit512`, `bit1024`
- Performance improvements: 5-6× speedup for 256-bit operations

## Related

- RFC 0001: Core Architecture
- Product Spec: Bit Manipulation Library
