# 2026-06-21 仓库极简真相化重构

## 目标

- 收敛后端与公开契约到最小可维护集（scalar + avx2）
- 删除无调用 SIMD 资产与死宏层
- 降低测试/CI/文档/规则流程负担
- 去除 OpenSpec 对主流程的强耦合

## 代码改动

- 新增 `include/bitcal/detail/scalar_impl.hpp`
  - 承载 scalar 位运算 fallback
  - 承载移位数组实现（原 `scalar_ops.hpp` 活路径）
- 更新 `include/bitcal/detail/word_ops.hpp`
  - 迁移为依赖 `scalar_impl.hpp`
  - 直接调用 `detail::shift_left_array` / `detail::shift_right_array`
- 更新 `include/bitcal/detail/x64_dispatch.hpp`
  - 保留 AVX2 + scalar 双路径
- 更新 `include/bitcal/algorithms.hpp`
  - 新增公开 in-place API：`or_into` / `xor_into` / `andnot_into`
  - `bit_and` 与其他 bitwise 算法统一调用形态
- 更新 `include/bitcal/config.hpp`
  - 删除 `simd_backend` 与 `get_default_simd_backend()`
  - `backend_kind` 收敛为 `scalar` / `avx2`
  - `default_backend()` 与实际执行路径一致
- 删除死文件
  - `include/bitcal/avx_ops.hpp`
  - `include/bitcal/avx512_ops.hpp`
  - `include/bitcal/sse_ops.hpp`
  - `include/bitcal/neon_ops.hpp`
  - `include/bitcal/scalar_ops.hpp`
  - `include/bitcal/backend_ops.def`
  - `include/bitcal/detail/backend.hpp`

## 测试与基准改动

- 删除 `tests/test_bitcal_detail.cpp`
- 更新 `tests/CMakeLists.txt`，仅保留 `test_bitcal`
- 更新 `tests/test_bitcal.cpp`
  - backend 枚举断言改为 scalar + avx2
  - 新增 in-place API 测试：or/xor/andnot
  - 公共契约可见性测试纳入全部 in-place API
- 更新 `benchmarks/benchmark_harness.hpp`
  - backend 名称映射收敛为 scalar + avx2

## CI / docs 工具链改动

- 简化 `.github/workflows/ci.yml`
  - 保留 Linux GCC 主 job + sanitizer job
  - 删除 openspec 路径触发
- 更新 `.github/workflows/docs-pages.yml`
  - 删除 openspec 路径触发
- 更新 `docs/scripts/check-ia.mjs`
  - 移除 OpenSpec change 目录强依赖校验

## 文档与规则改动

- 更新 `README.md` / `README.zh-CN.md`
  - backend 说明收敛为 scalar / avx2（编译期选择）
  - API 列表加入 in-place 全家桶
  - 移除 OpenSpec canonical 描述
- 重写 `AGENTS.md`
  - 从 OpenSpec-first 改为轻流程、代码/测试真相优先
- 重写 `CLAUDE.md`
  - 移除 OpenSpec 强制流程
- 重写 `.github/copilot-instructions.md`
  - 移除 OpenSpec 强依赖，改为代码/测试/README 同步策略
- 重写 `CONTRIBUTING.md`
  - 从 Spec-driven 转为轻量贡献规范
  - C++ 基线明确为 C++23
- 更新 `.github/PULL_REQUEST_TEMPLATE/pull_request_template.md`
  - checklist 改为 tests + README/docs 同步
- 更新 docs 文案
  - 去除 status/migration 中 OpenSpec 强绑定表述
- 删除低价值白皮书资产
  - `docs/en/whitepaper/design-evolution.md`
  - `docs/zh/whitepaper/design-evolution.md`
  - `docs/diagrams/`
  - 并同步 whitepaper index 导航

## OpenSpec 资产处理

- 删除 `openspec/` 目录
- 删除 `.claude/commands/opsx/`（OpenSpec CLI 专用命令）

## 额外一致性修复

- `vcpkg.json` 描述从 C++17 改为 C++23

## 风险与注意

- 历史 CHANGELOG 中旧版本段落仍存在已删文件名（历史归档内容，未逐版本清洗）
- `docs/superpowers/` 为历史计划资产，可能仍出现 OpenSpec 文案（不在主 IA 内容路径）
