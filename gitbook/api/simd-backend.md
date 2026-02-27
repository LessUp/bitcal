# SIMD 后端

## 后端枚举

```cpp
enum class simd_backend {
    scalar,   // 通用标量实现（所有平台）
    sse2,     // SSE2 (x86)
    avx,      // AVX (x86)
    avx2,     // AVX2 (x86)
    avx512,   // AVX-512 (x86)
    neon      // NEON (ARM)
};
```

## 自动选择

```cpp
constexpr simd_backend get_default_backend();
```

在编译期根据当前编译器标志自动选择最优后端，优先级：

1. AVX-512（如果可用）
2. AVX2
3. AVX
4. SSE2
5. NEON
6. Scalar（兜底）

## 手动指定

```cpp
// 强制使用 AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> a;

// 强制使用标量（用于调试或可移植性）
bitcal::bitarray<256, bitcal::simd_backend::scalar> b;

// 强制使用 NEON（ARM 平台）
bitcal::bitarray<128, bitcal::simd_backend::neon> c;
```

## 运行时查询

```cpp
#include <iostream>

switch (bitcal::get_default_backend()) {
    case bitcal::simd_backend::avx2:
        std::cout << "Using AVX2" << std::endl;
        break;
    case bitcal::simd_backend::sse2:
        std::cout << "Using SSE2" << std::endl;
        break;
    // ...
}
```

## 各后端支持的操作

| 操作 | scalar | SSE2 | AVX2 | NEON |
|------|--------|------|------|------|
| AND/OR/XOR | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| NOT | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| ANDNOT | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| Shift | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| is_zero | 全位宽 | 128/256 | 256/512 | — |
| popcount | 全位宽 | — | — | — |

未匹配的位宽/后端组合会自动回退到标量实现。
