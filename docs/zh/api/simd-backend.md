# SIMD 后端

## 后端枚举

```cpp
enum class simd_backend {
    scalar,   // 可移植标量回退（所有平台）
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

根据编译器标志在编译期自动选择最优后端。

**选择优先级**：
1. AVX-512 → AVX2（检测到但回退到 AVX2）
2. AVX2
3. AVX
4. SSE2
5. NEON
6. Scalar

## 手动指定

强制使用特定后端：

```cpp
// 强制 AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> a;

// 强制 NEON
bitcal::bitarray<128, bitcal::simd_backend::neon> b;

// 强制 scalar（最大可移植性）
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
| AND/OR/XOR | 所有宽度 | 128/256 | 256/512 | 128/256/512 |
| NOT | 所有宽度 | 128/256 | 256/512 | 128/256/512 |
| ANDNOT | 所有宽度 | 128/256 | 256/512 | 128/256/512 |
| 左移 | 所有宽度 | 128/256 | 256/512 | 128/256/512 |
| 右移 | 所有宽度 | 128/256 | 256/512 | 128/256/512 |
| is_zero | 所有宽度 | 128/256 | 256/512 | 128/256/512 |
| popcount | 所有宽度 | — | — | — |

> 不匹配的宽度/后端组合会自动回退到标量实现。

## 编译器标志

启用特定 SIMD 指令集：

### GCC/Clang (x86)

```bash
# 自动检测 CPU 能力
-march=native

# 手动指定
-msse2
-mavx
-mavx2
-mavx512f
```

### GCC/Clang (ARM)

```bash
# 带 NEON 的 ARM64
-march=armv8-a+simd

# 带 NEON 的 ARM32
-mfpu=neon
```

### MSVC

```cmd
/arch:AVX
/arch:AVX2
```

## AVX-512 支持状态

当前，检测到 AVX-512 时，库会回退到 AVX2 实现。完整的 AVX-512 支持计划在未来版本中提供。

要在 AVX-512 可用时显式使用 AVX2：

```cpp
bitcal::bitarray<256, bitcal::simd_backend::avx2> arr;
```

## 跨平台代码

为获得最大可移植性：

```cpp
// 让编译器自动选择
bitcal::bit256 automatic;

// 强制 scalar 以保证兼容性
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable;
```

## 性能比较

使用不同后端运行基准测试：

```bash
# 使用不同标志构建并比较
g++ -std=c++17 -O3 -march=native benchmark.cpp -o bench_native
g++ -std=c++17 -O3 -msse2 benchmark.cpp -o bench_sse2
g++ -std=c++17 -O3 -mavx2 benchmark.cpp -o bench_avx2

./bench_native
./bench_sse2
./bench_avx2
```

## 另见

- [架构](../architecture/simd-dispatch.md) — SIMD 分发实现
- [平台支持](../architecture/platform-support.md) — 支持的平台
