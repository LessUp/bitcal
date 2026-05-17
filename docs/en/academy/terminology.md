# Terminology

Definitions of terms used in BitCal documentation.

## Core Types

| Term | Chinese | Definition |
|------|---------|------------|
| bit_block | 位块 | Owning storage container for fixed-width data |
| bit_view | 位视图 | Mutable borrowing accessor, does not own storage |
| const_bit_view | 常量位视图 | Read-only borrowing accessor, does not own storage |
| word | 字 | 64-bit storage unit, BitCal's base data unit |

## Design Concepts

| Term | Chinese | Definition |
|------|---------|------------|
| public model | 公开模型 | Types and operation contracts BitCal exposes externally |
| dispatch boundary | 分发边界 | Implementation boundary for compile-time backend selection |
| backend | 后端 | ISA-specific implementation (scalar/SSE2/AVX2/NEON) |
| kernel | 内核 | Specific algorithm implementation within a backend |
| contract | 契约 | Behavior guarantees users can rely on |

## Storage Terms

| Term | Chinese | Definition |
|------|---------|------------|
| alignment | 对齐 | Memory address is a multiple of some value (BitCal defaults to 32-byte) |
| little-endian | 小端序 | Lower byte stored at lower address |
| fixed-width | 固定位宽 | Bit count determined at compile time |

## SIMD Terms

| Term | Definition |
|------|------------|
| SIMD | Single Instruction, Multiple Data - parallel processing technology |
| SSE2 | Streaming SIMD Extensions 2 - 128-bit SIMD instruction set |
| AVX2 | Advanced Vector Extensions 2 - 256-bit integer SIMD instruction set |
| AVX-512 | Advanced Vector Extensions 512 - 512-bit SIMD instruction set |
| NEON | ARM NEON - ARM 128-bit SIMD instruction set |
| Intrinsic | Compiler-provided SIMD function interface |

## Operation Terms

| Term | Chinese | Definition |
|------|---------|------------|
| bit_and | 按位与 | Bitwise AND operation |
| bit_or | 按位或 | Bitwise OR operation |
| bit_xor | 按位异或 | Bitwise XOR operation |
| popcount | 位计数 | Count the number of 1s |
| clz | 前导零计数 | Count Leading Zeros |
| ctz | 尾随零计数 | Count Trailing Zeros |
| bit_reverse | 位反转 | Bit order reversal |

## Documentation Terms

| Term | Chinese | Definition |
|------|---------|------------|
| whitepaper | 白皮书 | Technical design document |
| baseline | 基线 | Current reproducible performance benchmark |
| methodology | 方法学 | Rules for measurement and interpretation |
| evidence | 证据 | Verifiable performance or correctness data |

## C++ Terms

| Term | Definition |
|------|------------|
| Header-only | No library file compilation needed, just include headers |
| if constexpr | C++17 feature for compile-time branch selection |
| zero-overhead abstraction | Abstraction without runtime cost |
| SFINAE | Substitution Failure Is Not An Error - template metaprogramming technique |

---

> Return to [Academy Home](./index.md) or continue to [Whitepaper](../whitepaper/index.md).
