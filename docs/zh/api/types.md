# 类型参考

本页定义 vNext 的公开类型术语表。

## 公开术语表

| 术语 | 公开 API | 角色 |
| --- | --- | --- |
| 拥有型 block | `bit_block<Bits>` | 持有固定宽度、连续字数组的存储对象 |
| 非拥有视图 | `bit_view` | 对外部存储的可写借用访问 |
| 只读视图 | `const_bit_view` | 对外部存储的只读借用访问 |
| 后端边界 | `backend_kind`、`default_backend()` | 描述执行选择的诊断表面，但不成为主要存储模型 |

## 稳定 include 入口

```cpp
#include <bitcal/bitcal.hpp>
```

这个聚合头文件就是 vNext 模型稳定的公开 include seam。

## 拥有型 block：`bit_block<Bits>`

```cpp
template <std::size_t Bits>
class bit_block;
```

### 约束

- `Bits >= 64`
- `Bits` 必须是 64 的倍数

### 公开常量

| 常量 | 含义 |
| --- | --- |
| `bit_block<Bits>::bits` | 总位宽 |
| `bit_block<Bits>::word_count` | 64 位字数量 |
| `bit_block<Bits>::storage_alignment` | owning storage 采用的对齐值 |

在当前 x86 构建上，`storage_alignment` 为 32 字节；其他构建则回退到 `alignof(std::uint64_t)`。

### 公开操作

```cpp
constexpr bit_block() noexcept;
static constexpr bit_block from_words(std::span<const std::uint64_t> words) noexcept;
constexpr bit_view view() noexcept;
constexpr const_bit_view view() const noexcept;
constexpr std::uint64_t word(std::size_t index) const noexcept;
constexpr void copy_words_to(std::span<std::uint64_t> out) const noexcept;
```

### 示例

```cpp
std::array<std::uint64_t, 4> words{1, 2, 3, 4};
auto block = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(words));
auto first = block.word(0);
```

## 非拥有视图：`bit_view`

```cpp
class bit_view {
public:
    constexpr bit_view() noexcept = default;
    constexpr bit_view(std::uint64_t* data, std::size_t words) noexcept;

    constexpr std::uint64_t* data() noexcept;
    constexpr const std::uint64_t* data() const noexcept;
    constexpr std::size_t word_count() const noexcept;
    constexpr std::uint64_t word(std::size_t index) const noexcept;
    constexpr operator const_bit_view() const noexcept;
};
```

当你希望自由算法把结果写入现有存储时，就应该使用 `bit_view`。

## 只读视图：`const_bit_view`

```cpp
class const_bit_view {
public:
    constexpr const_bit_view() noexcept = default;
    constexpr const_bit_view(const std::uint64_t* data, std::size_t words) noexcept;

    constexpr const std::uint64_t* data() const noexcept;
    constexpr std::size_t word_count() const noexcept;
    constexpr std::uint64_t word(std::size_t index) const noexcept;
};
```

当算法只需要读取或查询时，使用 `const_bit_view`。

## 视图的生命周期规则

视图永远不拥有内存。调用方必须保证底层字数组在视图存活期间一直有效。

实际使用时通常只有两种常见模式：

- 通过 `view()` 从 owning block 借用
- 在你已经管理生命周期的外部缓冲区上构造视图

## 后端边界

```cpp
enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512,
};

constexpr backend_kind default_backend() noexcept;
```

这些名称适合用于诊断、测试说明和 benchmark 记录，但它们不是公开存储模型的中心。

## 这些类型如何组合

```cpp
bitcal::bit_block<256> owned;
auto writable = owned.view();
bitcal::const_bit_view readable = writable;
```

这就是完整的公开类型故事：用 block 持有，用 view 借用，观察后端边界，但不要把它变成主抽象。

## 下一步

- [核心操作](./core-operations.md)
- [架构概览](../architecture/overview.md)
- [vNext 白皮书](../architecture/vnext-whitepaper.md)
