# 类型别名

## bitarray 模板

```cpp
template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray;
```

- **Bits** — 位宽（必须是 64 的倍数）
- **Backend** — SIMD 后端，默认自动选择当前平台最优

## 预定义别名

| 别名 | 等价定义 | 内部 word 数 |
|------|----------|-------------|
| `bitcal::bit64` | `bitarray<64>` | 1 |
| `bitcal::bit128` | `bitarray<128>` | 2 |
| `bitcal::bit256` | `bitarray<256>` | 4 |
| `bitcal::bit512` | `bitarray<512>` | 8 |
| `bitcal::bit1024` | `bitarray<1024>` | 16 |

## 内存布局

- 底层存储为 `uint64_t data_[u64_count]`
- 内存对齐为 `BITCAL_ALIGNMENT`（默认 64 字节）
- `data_[0]` 是最低位 word，`data_[N-1]` 是最高位 word（小端序）

## 构造

```cpp
bitarray()                      // 默认构造，所有位清零
explicit bitarray(uint64_t v)   // 用 v 初始化最低 64 位，其余清零
```

## 数据访问

```cpp
uint64_t* data()                // 底层 word 数组指针
uint64_t  operator[](size_t i)  // 读取第 i 个 word
uint64_t& operator[](size_t i)  // 写入第 i 个 word
```
