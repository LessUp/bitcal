# 结构整理与中文文档补齐

**日期**: 2026-01-08  
**类型**: 维护更新

## 变更概述

本次更新聚焦于：
- 让仓库当前主线（header-only v2.x）在“开箱即用”的情况下可编译、可安装、可阅读。
- 修复历史兼容头文件指向已删除/不存在文件导致的包含失败。
- 补齐中文文档入口。

## 具体变更

### 1. 修复顶层构建脚本

- 将 `CMakeLists.txt` 从旧版 `.cpp` 静态库构建方式，调整为与现状一致的 `INTERFACE`（header-only）库：
  - `add_library(bitcal INTERFACE)`
  - `target_include_directories(... BUILD_INTERFACE/INSTALL_INTERFACE ...)`
  - 保留 `tests/`、`examples/` 子目录构建开关
  - 增加 install/export，支持 `find_package(bitcal)` 的配置生成

### 2. 移除旧版兼容入口头

- 移除：
  - `include/bitcal/bitcal.h`
  - `include/bitcal/encode.h`

说明：仓库明确选择仅保留 header-only v2.x 主线，以减少维护成本与使用者困惑。

### 3. 新增中文文档

- 新增 `docs/README.md`
  - 说明项目结构、推荐 include 方式
  - 解释 `include/bitcal/` 布局原因
  - 说明 `src/` 在当前主线下的角色与去留建议

### 4. 增强对外头文件可读性

- `include/bitcal/bitcal.hpp`
  - 为对外 API（`bitarray`、常用别名、`ops`）补充中文注释
  - 不改变任何行为，仅提升阅读与上手体验

### 5. 移除旧结构残留目录

- 移除 `src/` 目录（确认不再需要 legacy/dispatch/encode 兼容路线）

### 6. 清理冗余构建入口与文档残留

- 移除 `CMakeLists_new.txt`，统一以顶层 `CMakeLists.txt` 作为唯一权威构建入口
- 清理文档与历史变更记录中对已移除内容（legacy/dispatch/兼容入口头）的误导性描述

## 影响

- 对使用者：`#include <bitcal/bitcal.hpp>` 的推荐用法不变；旧版兼容入口头已移除以减少困惑。
- 对构建系统：顶层 `CMakeLists.txt` 与当前 header-only 实现保持一致。
