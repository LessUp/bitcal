# BitCal 文档（中文）

本文档面向使用者，介绍 BitCal 当前版本（v2.x）的结构、推荐用法与扩展方式。

## 1. 这是什么库？

BitCal 是一个以 **header-only** 为目标的位运算/位数组工具库。

核心类型为 `bitcal::bitarray<Bits, Backend>`：
- `Bits`：位宽（例如 64/128/256/512/1024）
- `Backend`：SIMD 后端（标量 / SSE2 / AVX / AVX2 / AVX-512 / NEON）

你通常只需要：

```cpp
#include <bitcal/bitcal.hpp>
```

## 2. 推荐包含方式

- 唯一入口（推荐且仅支持）：
  - `#include <bitcal/bitcal.hpp>`

说明：本仓库已选择“只保留 v2.x header-only 主线”，不再提供旧版运行时 dispatch/encode 的兼容入口头。

## 3. 目录结构为什么是 include/bitcal 这样？

采用 `include/<库名>/...` 的布局是 C/C++ 生态的通用约定，目的：
- 避免头文件名冲突（例如别的项目也可能有 `config.hpp`）
- 配合 CMake 的 `target_include_directories(... INSTALL_INTERFACE:include)`，让用户以 `<bitcal/...>` 的方式稳定包含
- 对 header-only 库尤其友好：安装/拷贝 `include/` 即可使用

当前核心头文件：
- `include/bitcal/bitcal.hpp`：对外主入口（推荐用户只 include 这个）
- `include/bitcal/config.hpp`：平台/指令集检测、宏、`simd_backend`
- `include/bitcal/scalar_ops.hpp`：标量后备实现
- `include/bitcal/sse_ops.hpp` / `avx_ops.hpp` / `neon_ops.hpp`：不同 SIMD 实现

## 4. src/ 目录是否有必要？

从当前仓库状态看：
- 新版核心实现完全位于 `include/bitcal/`，并且通过 `bitcal.hpp` 聚合

因此：
- 对 **v2.x header-only 主线** 来说，`src/` 不仅不是必需的，而且会增加理解成本

建议：
- 已确认不需要兼容后，直接移除 `src/` 目录

## 5. 快速上手示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;
    a <<= 10;

    std::cout << "popcount(c) = " << c.popcount() << "\n";
    return 0;
}
```

## 6. 扩展/贡献建议

- 新增算法时，优先补标量版本（`scalar_ops.hpp`），再按需补 SSE/AVX/NEON。
- 尽量保持 `bitcal.hpp` 为“聚合入口”，让用户 include 成本最低。
