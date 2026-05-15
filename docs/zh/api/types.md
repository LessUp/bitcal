# 类型参考

本页记录的是持久 vNext 类型契约，而不是把当前头文件里能看到的每个便利 helper 都当成公开承诺。

## 公开术语表

| 术语 | 公开 API | 角色 |
| --- | --- | --- |
| 拥有型 block | `bit_block<Bits>` | 持有固定宽度、连续字数组的存储对象 |
| 非拥有视图 | `bit_view` | 对外部存储的可写借用访问 |
| 只读视图 | `const_bit_view` | 对外部存储的只读借用访问 |
| 后端边界 | `backend_kind` | 已文档化的后端词汇表，而不是主要存储模型 |

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

### 持久契约中的操作

```cpp
bit_block() noexcept;
bit_view view() noexcept;
const_bit_view view() const noexcept;
```

持久 API spec **不**承诺 `storage_alignment`、`from_words(...)`、`bit_block::word(...)`、`copy_words_to(...)` 这类辅助成员。

## 非拥有视图：`bit_view`

```cpp
class bit_view;
```

### 持久视图契约

```cpp
std::uint64_t* data() noexcept;
std::size_t word_count() const noexcept;
std::uint64_t word(std::size_t index) const noexcept;
```

当你希望算法或调用方修改现有连续字存储时，使用 `bit_view`。

## 只读视图：`const_bit_view`

```cpp
class const_bit_view;
```

### 持久视图契约

```cpp
const std::uint64_t* data() const noexcept;
std::size_t word_count() const noexcept;
std::uint64_t word(std::size_t index) const noexcept;
```

当算法只需要读取或查询时，使用 `const_bit_view`。

## 视图的生命周期规则

视图永远不拥有内存。调用方必须保证底层字数组在视图存活期间一直有效。

稳定用法可以概括成三步：

- 用 `bit_block<Bits>` 持有存储
- 通过 `view()` 借用
- 把这些 view 传给自由算法

## 后端边界

```cpp
enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512
};
```

这些名称适合用于架构讨论与验证记录。当前持久 API spec 还没有承诺额外的后端选择 helper。

## 这些类型如何组合

```cpp
bitcal::bit_block<256> owned;
auto writable = owned.view();
writable.data()[0] = 0xFULL;

const auto& owned_const = owned;
auto readable = owned_const.view();
auto first_word = readable.word(0);
auto words = readable.word_count();
```

今天真正承诺的公开类型故事就是这些：用 block 持有，用 view 借用，不要把 spec 之外的便利 helper 当成持久契约。

## 下一步

- [核心操作](./core-operations.md)
- [架构概览](../architecture/overview.md)
- [vNext 白皮书](../architecture/vnext-whitepaper.md)
