# Evolution and Comparisons

BitCal is easier to trust when it states what it is and what it is not.

## Comparison axes

| Axis | Older tendency | vNext tendency |
| --- | --- | --- |
| Public center | one monolithic type | owner, view, algorithm split |
| Backend story | visible API identity | implementation boundary |
| Performance narrative | feature language | retained evidence first |
| Support posture | broad compatibility language | x86-64-first precision |

## Nearby reference points

| Project or source | Why it matters |
| --- | --- |
| Boost dynamic_bitset | establishes one C++ baseline for flexible bitset ergonomics |
| CRoaring | shows how seriously bitmap engineering can treat performance and benchmarking |
| libpopcnt | illustrates how much rigor even one small bit primitive can justify |
| Intel Intrinsics Guide | grounds x86 implementation discussion in authoritative instruction references |
| Agner Fog manuals | adds latency and throughput context for low-level design decisions |
| Sebastiano Vigna's broadword paper | supplies a useful conceptual background for word-parallel bit techniques |

<CitationList
  :items="[
    {
      title: 'Boost dynamic_bitset',
      href: 'https://www.boost.org/doc/libs/release/libs/dynamic_bitset/',
      note: 'Useful when contrasting fixed-width public models with more flexible but different design goals.'
    },
    {
      title: 'CRoaring',
      href: 'https://github.com/RoaringBitmap/CRoaring',
      note: 'A strong example of performance-oriented bitmap engineering with clear scope.'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      note: 'Relevant when discussing how specialized a serious bit primitive implementation can become.'
    },
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      note: 'Grounds x86 SIMD discussion in an authoritative reference.'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: 'Adds useful microarchitectural context for performance interpretation.'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      note: 'A practical theoretical bridge between bit-level reasoning and word-parallel implementation.'
    }
  ]"
/>
