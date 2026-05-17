# BitCal Academy

The Academy module provides learning paths from zero to one, helping you build the mental models needed to understand BitCal.

## Learning Paths

<ReadingPathGrid
  :items="[
    {
      title: 'Why BitCal',
      href: '/en/academy/why-bitcal',
      summary: 'Understand BitCal positioning and value in C++ bit operation ecosystem.',
      detail: 'BitCal chose a middle ground the standard library ignores: fixed width + SIMD + rich semantics.'
    },
    {
      title: 'Bit Mental Model',
      href: '/en/academy/bit-mental-model',
      summary: 'Build correct intuition for bit operations and memory layout.',
      detail: 'Understanding words, alignment, little-endian, and the three-role model is foundational.'
    },
    {
      title: 'SIMD Primer',
      href: '/en/academy/simd-primer',
      summary: 'Learn SIMD optimization principles and BitCal dispatch strategy.',
      detail: 'From SSE2 to AVX2, understand how compile-time dispatch achieves zero-overhead abstraction.'
    },
    {
      title: 'Terminology',
      href: '/en/academy/terminology',
      summary: 'English-Chinese term definitions for consistent understanding.',
      detail: 'Definitions of all key concepts from core types to SIMD terms.'
    }
  ]"
/>

## Prerequisites

Reading Academy content requires the following background:

| Knowledge Area | Minimum | Recommended |
|---------------|---------|-------------|
| C++ | Know templates and constexpr | Familiar with C++17/23 features |
| Bit Operations | Understand AND/OR/XOR | Understand bit-level optimization |
| Computer Architecture | Know memory alignment | Understand SIMD and cache lines |

---

> After learning the basics, proceed to [Whitepaper](../whitepaper/index.md) for BitCal's design architecture.
