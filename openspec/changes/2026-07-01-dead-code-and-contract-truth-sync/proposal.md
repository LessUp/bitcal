## Why

BitCal v4.0.0 已落地 C++23 / `bit_block` / `bit_view` / 自由算法契约，但仓库仍残留多个与契约脱节的资产：

- `include/bitcal/sse_ops.hpp`、`avx_ops.hpp`、`avx512_ops.hpp`、`neon_ops.hpp`、`backend_ops.def` 不被任何 retained 路径 include，是孤立 SIMD 命名空间死代码。
- `config.hpp` 暴露 `simd_backend` 枚举与 `get_default_simd_backend()`，但公开契约只承认 `backend_kind` 与 `default_backend()`，且 `simd_backend` 无任何 retained 调用方。
- `detail/backend.hpp` 使用 `BITCAL_FORCEINLINE` 但未 include `config.hpp`，非自包含。
- `AGENTS.md` 仍写 "C++17" 且把 `bitarray` 当作核心抽象；`CONTRIBUTING.md` 仍要求 C++17 编译器并指引向已删除的 `<backend>_ops.hpp` 与 `simd_backend` 枚举。
- `2026-05-14-vnext-cpp23-x64-redesign` 与 `2026-05-22-prune-legacy-vnext-surface` 的实现早已完成，但 `tasks.md` 仍未勾选，造成规范真相与代码真相漂移。

## What Changes

- 删除五个孤立 SIMD 头文件与 `backend_ops.def`。
- 从 `config.hpp` 移除 `simd_backend` 枚举与 `get_default_simd_backend()`，保留契约化的 `backend_kind` 与 `default_backend()`。
- 修复 `detail/backend.hpp` 自包含性。
- 修正 `AGENTS.md` 与 `CONTRIBUTING.md` 的语言基线、核心抽象与 SIMD 后端指引。
- 勾选已实际完成的 `2026-05-14` 与 `2026-05-22` 任务，使 OpenSpec 真相与实现一致。

## Impact

- 公开契约不变：`backend_kind` 与 `default_backend()` 仍是唯一 retained 后端枚举。
- 移除的 `simd_backend` / `get_default_simd_backend()` 不在任何 retained 调用路径中，无迁移影响。
- 文档与 AI 执行手册与 v4.0.0 契约对齐，减少未来漂移。
