## 1. Contract freeze

- [x] 1.1 创建 `2026-05-08-contract-public-surface` 变更目录，并写 proposal / design / tasks / spec deltas
- [x] 1.2 更新持久 OpenSpec，冻结 3.0.0 retained public surface，移除过时公共契约描述
- [x] 1.3 修正 RFC 0001 中与 public seam、头文件分层、后端命名相关的架构漂移描述

## 2. Header and implementation contraction

- [x] 2.1 将 retained `bitarray` 接口迁移到 `include/bitcal/bitarray.hpp`
- [x] 2.2 让 `include/bitcal/bitcal.hpp` 退化为唯一稳定 umbrella include，并清理不再保留的 helper surface
- [x] 2.3 删除或内部化 `bitcal::ops`、type traits 与未保留 convenience API

## 3. Verification and release alignment

- [x] 3.1 按 retained public contract 更新测试，仅保留公开表面的验证入口
- [x] 3.2 更新用户文档与迁移说明，明确 2.x → 3.0.0 breaking changes
- [x] 3.3 对齐版本语义、发布说明与其他仓库锚点到 3.0.0
