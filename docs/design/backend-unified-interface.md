# SIMD 后端统一接口设计

## 目标

将分散的 SIMD 后端函数统一到一个模板接口中，使得 `bitarray` 类可以通过单一的模板调用执行操作，消除大量条件编译代码。

## 当前问题

每个后端都有自己的命名约定：
- `scalar::shift_left_array<N>(data, count)`
- `sse::shift_left_128(data, count)`, `sse::shift_left_256(data, count)`
- `avx::shift_left_256(data, count)`, `avx::shift_left_512(data, count)`
- `neon::shift_left_128(data, count)`, `neon::shift_left_256(data, count)`
- `avx512::shift_left_256(data, count)`, `avx512::shift_left_512(data, count)`

这导致 `bitarray` 类中每个操作都需要大量 `if constexpr` 分支。

## 设计方案

### 统一接口模板

```cpp
namespace bitcal::backend {

template<size_t Bits, simd_backend Backend>
struct ops {
    // 位运算
    static void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out);
    static void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out);
    static void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out);
    static void bit_not(const uint64_t* in, uint64_t* out);
    static void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out);

    // 移位
    static void shift_left(uint64_t* data, int count);
    static void shift_right(uint64_t* data, int count);

    // 查询
    static bool is_zero(const uint64_t* data);
    static bool all(const uint64_t* data);
    static bool equals(const uint64_t* a, const uint64_t* b);
    static uint64_t popcount(const uint64_t* data);
};

} // namespace bitcal::backend
```

### 特化示例

```cpp
namespace bitcal::backend {

// Scalar 后端特化（适用于所有位宽）
template<size_t Bits>
struct ops<Bits, simd_backend::scalar> {
    static void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) {
        for (size_t i = 0; i < Bits / 64; ++i) {
            out[i] = a[i] & b[i];
        }
    }

    static void shift_left(uint64_t* data, int count) {
        scalar::shift_left_array<Bits / 64>(data, count);
    }

    // ... 其他操作
};

// AVX2 后端特化（256-bit）
template<>
struct ops<256, simd_backend::avx2> {
    static void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) {
        avx::bit_and_256(a, b, out);
    }

    static void shift_left(uint64_t* data, int count) {
        avx::shift_left_256(data, count);
    }

    // ... 其他操作
};

} // namespace bitcal::backend
```

### bitarray 类中的使用

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
    // 之前：
    // bitarray operator&(const bitarray& other) const {
    //     bitarray result;
    //     dispatch_binop<binop::op_and>(*this, other, result);  // 76 行条件编译
    //     return result;
    // }

    // 之后：
    bitarray operator&(const bitarray& other) const {
        bitarray result;
        backend::ops<Bits, Backend>::bit_and(data_, other.data_, result.data_);
        return result;
    }
};
```

## 实施步骤

1. 创建 `backend_ops.hpp` 文件，定义统一的接口模板
2. 为 scalar 后端实现通用特化（适用于所有位宽）
3. 为 SSE2 后端实现 128/256-bit 特化
4. 为 AVX2 后端实现 256/512-bit 特化
5. 为 AVX-512 后端实现 256/512-bit 特化
6. 为 NEON 后端实现 128/256/512-bit 特化
7. 重构 `bitarray` 类使用统一接口
8. 运行测试验证正确性

## 预期收益

- **代码量减少**：`bitarray` 类从 ~1000 行减少到 ~600 行
- **可维护性提升**：添加新操作只需修改一处
- **扩展性提升**：添加新后端只需添加特化
- **编译时间优化**：减少模板实例化复杂度
