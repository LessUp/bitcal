## Why

BitCal 当前公开表面已经和仓库内实现、测试与文档产生持续漂移：

- `openspec/specs/api/bitcal-public-api.md` 仍把 `simd_backend::avx` 写成公开枚举值，但实现中只保留 `scalar`、`sse2`、`avx2`、`avx512`、`neon`。
- 公开 API 说明长期把 `bitcal::ops`、类型 traits、以及一批未进入规范的 convenience 方法一起暴露出来，和“只保留可维护公共契约”的终局目标冲突。
- 架构 RFC 仍引用 `simd_traits.hpp` 与过时的文件结构，而现有实现已经转向 `backend_ops.hpp`。
- `docs/README.md` 已要求 API 文档只覆盖 retained public API，但 OpenSpec 还没有先把 retained contract 写死。

在进入后续测试、代码和文档收口前，必须先用 OpenSpec 明确 BitCal 3.0.0 的 breaking contract：什么仍然是稳定公共接口，什么会从 retained public surface 中移除，新的头文件分层如何落地。

## What Changes

- 创建变更 `2026-05-08-contract-public-surface`，作为 BitCal 3.0.0 公共接口收缩的唯一活动变更入口。
- **BREAKING**：规定 `<bitcal/bitcal.hpp>` 是唯一稳定公共 include seam；其他头文件不再承诺为稳定入口。
- **BREAKING**：规定 retained `bitarray` 契约迁移到 `include/bitcal/bitarray.hpp`，并由 `bitcal/bitcal.hpp` 统一再导出。
- **BREAKING**：将 `bitcal::ops`、`is_bitarray` / `bitarray_traits` 以及未进入 OpenSpec 的 convenience 方法从 retained public contract 中移除。
- 将持久 OpenSpec 与架构 RFC 一并更新到 3.0.0 目标契约，修正过时的后端与文件分层描述。

## Impact

- 影响 OpenSpec：`openspec/specs/api/bitcal-public-api.md`、`openspec/specs/testing/bitcal-testing-spec.md`、`openspec/specs/product/bit-manipulation-library.md`、`openspec/specs/rfc/0001-core-architecture.md`
- 影响后续实现：`include/bitcal/bitcal.hpp` 将被拆分，新增 `include/bitcal/bitarray.hpp`，并同步清理不再保留的公开辅助接口
- 影响后续测试/文档：测试与 API 文档只验证 retained public API；迁移文档需要明确 2.x → 3.0.0 的 breaking changes
- 版本语义：该 contract contraction 计划作为 **v3.0.0** 发布
