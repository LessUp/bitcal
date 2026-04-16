# SIMD 后端

## 后端枚举

```cpp
enum class simd_backend {
    scalar,   // 通用标量实现（所有平台）
    sse2,     // SSE2 (x86)
    avx,      // AVX (x86)
    avx2,     // AVX2 (x86)
    avx512,   // AVX-512 (x86) — 回退到 AVX2
    neon      // NEON (ARM)
};
```

## 自动选择

```cpp
constexpr simd_backend get_default_backend();
```

编译期根据编译器标志自动选择最优后端。

**选择优先级**：
1. AVX-512 → AVX2（AVX-512 检测到但回退到 AVX2 实现）
2. AVX2
3. AVX
4. SSE2
5. NEON
6. Scalar

## 手动指定

强制使用特定后端：

```cpp
// 强制使用 AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> a;

// 强制使用 NEON
bitcal::bitarray<128, bitcal::simd_backend::neon> b;

// 强制使用标量（最大可移植性）
bitcal::bitarray<256, bitcal::simd_backend::scalar> c;
```

## 运行时查询

```cpp
#include <iostream>

std::string backend_name(bitcal::simd_backend b) {
    switch (b) {
        case bitcal::simd_backend::avx2:   return "AVX2";
        case bitcal::simd_backend::avx:    return "AVX";
        case bitcal::simd_backend::sse2:   return "SSE2";
        case bitcal::simd_backend::neon:   return "NEON";
        case bitcal::simd_backend::avx512: return "AVX-512";
        default: return "scalar";
    }
}

int main() {
    std::cout << "Backend: " << backend_name(bitcal::get_default_backend()) << std::endl;
}
```

## 后端支持矩阵

| 操作 | Scalar | SSE2 | AVX2 | NEON |
|------|--------|------|------|------|
| AND/OR/XOR | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| NOT | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| ANDNOT | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| Shift Left | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| Shift Right | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| is_zero | 全位宽 | 128/256 | 256/512 | 128/256/512 |
| popcount | 全位宽 | — | — | — |

> 未匹配的位宽/后端组合自动回退到标量实现。

## 编译器标志

启用特定 SIMD 指令集：

### GCC/Clang (x86)

```bash
# 自动检测 CPU 支持的指令集
-march=native

# 手动指定
-msse2
-mavx
-mavx2
-mavx512f
```

### GCC/Clang (ARM)

```bash
# ARM64 with NEON
-march=armv8-a+simd

# ARM32 with NEON
-mfpu=neon
```

### MSVC

```cmd
/arch:AVX
/arch:AVX2
```

## 注意事项

1. **AVX-512 状态**: 目前检测到 AVX-512 时回退到 AVX2 实现。完整的 AVX-512 支持计划在未来版本中提供。

2. **跨平台代码**: 如果需要最大可移植性，使用 `simd_backend::scalar` 或不指定后端让编译器自动选择。

3. **性能测试**: 使用不同后端运行 `benchmarks/bitcal_benchmark` 比较性能。
