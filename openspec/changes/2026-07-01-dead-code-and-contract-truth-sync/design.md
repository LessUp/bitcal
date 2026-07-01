## Context

v4.0.0 redesign 已将公开契约收敛到 `bit_block` / `bit_view` / 自由算法 / `backend_kind`。早期 SIMD 重构留下的孤立命名空间头文件、未契约化的 `simd_backend` 枚举、非自包含的 detail 头、以及过时的 AI/贡献者文档仍在仓库中，造成"代码真相 > 规范真相 > 文档真相"的多层漂移。

## Decision

采用激进收口：删除一切无 retained 调用方的 SIMD 死代码，移除未契约化的枚举，修复自包含性，同步文档与 OpenSpec 任务状态。不保留兼容 shim，因为 v4.0.0 本就是 breaking redesign 且这些符号不在公开 include seam 中。

### 删除范围

| 资产 | 删除理由 |
|------|----------|
| `include/bitcal/sse_ops.hpp` | 无任何 retained include |
| `include/bitcal/avx_ops.hpp` | 无任何 retained include |
| `include/bitcal/avx512_ops.hpp` | 无任何 retained include |
| `include/bitcal/neon_ops.hpp` | 无任何 retained include |
| `include/bitcal/backend_ops.def` | 仅被注释示例引用，无实际 include |

### 收口范围

| 资产 | 改动 |
|------|------|
| `include/bitcal/config.hpp` | 移除 `simd_backend` 枚举与 `get_default_simd_backend()` |
| `include/bitcal/detail/backend.hpp` | 补 include `config.hpp` 使其自包含 |
| `AGENTS.md` | C++17→C++23，`bitarray`→`bit_block`/`bit_view`/自由算法 |
| `CONTRIBUTING.md` | C++23 编译器要求，SIMD 后端指引对齐 `detail/x64_dispatch.hpp` |

### OpenSpec 真相同步

- `2026-05-14-vnext-cpp23-x64-redesign`：实现已完成，勾选全部任务。
- `2026-05-22-prune-legacy-vnext-surface`：实现已完成，勾选全部任务。
- `2026-05-15-pages-whitepaper-overhaul`：含滚动文档任务，不在本批代码收口范围，保留状态。

## Alternatives Considered

- **保留 `simd_backend` 作为别名**：被否决，它不在公开契约中且无调用方，保留只会延长漂移。
- **逐文件渐进删除**：被否决，五个文件均无 retained 依赖，一次性删除回滚边界更清晰。

## Risks and Mitigations

- **风险**：外部消费者直接 include 了被删头文件。
- **缓解**：这些头从未在 `<bitcal/bitcal.hpp>` 中暴露，且 API spec 已声明 umbrella header 是唯一稳定 include seam；直接 include internal 头本就在契约之外。
