## Context

这个变更只先定义契约，不在本任务中改实现。目标是在后续代码收口前，先把 BitCal 3.0.0 的 retained public surface、头文件分层和迁移边界写成单一真相。

## Goals / Non-Goals

**Goals**
- 固定 `<bitcal/bitcal.hpp>` 作为唯一稳定公共 include seam。
- 固定 `bitarray` 作为 retained public type，并给出新的头文件分层。
- 从 retained public contract 中移除 `bitcal::ops`、类型 traits 和未规范化 convenience API。
- 让 API / testing / product / architecture 四类 OpenSpec 对同一个 3.0.0 breaking contract 给出一致描述。

**Non-Goals**
- 本任务不修改 `include/bitcal/` 实现代码。
- 本任务不修改用户文档、README 或测试源码。
- 本任务不重新设计 `bitarray` 的核心算子语义，只收缩和澄清 public surface。

## Decisions

### 1. 稳定公共入口只保留 `<bitcal/bitcal.hpp>`
- **Decision**：3.0.0 之后，`<bitcal/bitcal.hpp>` 是唯一稳定公共 include。
- **Why**：这给后续头文件拆分留下空间，同时避免调用方直接绑定内部层次结构。
- **Consequence**：即使仓库中存在 `include/bitcal/bitarray.hpp`，直接 include 该文件也不属于稳定 include contract，兼容性承诺只针对 umbrella header。

### 2. 将 retained `bitarray` 定义下沉到 `include/bitcal/bitarray.hpp`
- **Decision**：`bitarray` 主模板、稳定别名和其 retained 成员接口物理上迁移到 `include/bitcal/bitarray.hpp`，`bitcal/bitcal.hpp` 只做公共汇总入口。
- **Why**：当前 `bitcal.hpp` 同时承载公开类型、traits 和 helper namespace，导致 public seam 与内部实现边界混杂。
- **Consequence**：后续实现需要把 retained `bitarray` 契约和内部 helper 分开；`backend_ops.hpp`、`scalar_ops.hpp`、`config.hpp` 保持为分层依赖，而不是文档中的公共入口。

### 3. retained public contract 只覆盖规范中列出的 `bitarray` 能力
- **Decision**：保留的接口仅包括 `bitarray` 构造、字访问、按位运算、移位、比较、`andnot`、`is_zero`、`clear`、计数、单 bit 操作、`reverse`、稳定别名、`simd_backend` 与 `get_default_backend()`。
- **Why**：这些能力已有公开文档、示例或测试基础，能构成最小可信公共表面。
- **Consequence**：任何未在 OpenSpec 中重新列出的 convenience 方法都不进入 3.0.0 retained contract。

### 4. 明确移除的公开表面
- **Decision**：`bitcal::ops`、`is_bitarray`、`is_bitarray_v`、`bitarray_traits`，以及当前实现里存在但未被 OpenSpec 保留的方法（如 `find_first_set`、`find_last_set`、`set_range`、`clear_range`、`flip_range`、`all`、`any`、`none`、`count`、`size()`、`test()`、`bit64` 到 `uint64_t` 的特化转换）全部从 retained public contract 中移除。
- **Why**：这些接口要么属于内部辅助层，要么没有稳定文档/契约背书；继续保留只会扩大兼容面。
- **Consequence**：后续测试与文档必须停止把这些接口当成 retained public API；如果未来要重新暴露，必须重新写入 OpenSpec。

### 5. 版本语义和验证入口同步切到 3.0.0 breaking release
- **Decision**：本次 contract contraction 的版本目标固定为 3.0.0。
- **Why**：公共 include seam、辅助命名空间和 convenience API 的删除都构成明确 breaking change。
- **Consequence**：后续代码、测试、文档和版本锚点都必须以 3.0.0 语义推进，不再把该收缩伪装成 2.x 非破坏升级。

## Target Layering

```text
Stable public include
└── include/bitcal/bitcal.hpp
    └── include/bitcal/bitarray.hpp         # retained public bitarray contract
        ├── include/bitcal/config.hpp       # backend enum/default selection/alignment helpers
        ├── include/bitcal/backend_ops.hpp  # internal backend dispatch layer
        └── include/bitcal/scalar_ops.hpp   # internal scalar helpers
```

约束：
- `bitcal/bitcal.hpp` 是唯一稳定 include seam。
- `bitarray.hpp` 是 retained public type definition 的承载位置，但不是单独承诺的稳定入口。
- `backend_ops.hpp`、`scalar_ops.hpp`、`config.hpp` 的存在可被架构文档说明，但不构成额外的 stable public include contract。

## Verification Implications

- API 规范只列 retained public contract，不再记录被移除 helper surface。
- Testing 规范只要求通过 `<bitcal/bitcal.hpp>` 验证 retained public API。
- Product 规范必须把这次收缩描述为 3.0.0 breaking release，并要求迁移说明。
- RFC 0001 必须移除 `simd_traits.hpp`、`simd_backend::avx` 等过时描述，改写为当前/目标分层。

## Risks / Mitigations

- **风险：** 持久 OpenSpec 先于实现更新，短期内会暴露“规范先行”的过渡状态。  
  **Mitigation：** 在 proposal、design 和 tasks 中明确这是后续测试/代码/文档工作的基础变更。
- **风险：** 调用方可能已经直接使用 helper API。  
  **Mitigation：** 明确这些接口不再属于 retained contract，并将迁移说明列为后续任务。
