# 2026-01-08

- 说明：本文件记录的是早期“运行时 dispatch/legacy 路线”的一次阶段性工作。
- 当前仓库主线已明确选择 **仅保留 v2.x header-only 实现**：
  - 已移除 `src/` 目录
  - 已移除 `include/bitcal/bitcal.h` 与 `include/bitcal/encode.h`
  - 已移除本地 `spdlog/spdlog.h` 兼容头
  - 顶层构建以 `CMakeLists.txt` 的 `INTERFACE`（header-only）库为准
