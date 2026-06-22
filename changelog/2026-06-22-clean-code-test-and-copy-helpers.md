# 2026-06-22 Clean Code 优化：copy_words_helper + 测试清理

## 目标

按《代码整洁之道》消除 `bit_block` 中 `if consteval` 分支重复，并清理测试中的代码异味（`(void)var;` 静默模式、冗余版本断言）。

## 根因

### #8 copy_words_helper

`bit_block::from_words` 与 `copy_words_to` 各自重复 `if consteval { per-word loop } else { memcpy/memmove }` 模式。两处分支结构相同，仅 byte-copy 函数不同（`memcpy` vs `memmove`，后者支持 self-overlap）。

### #9 `(void)var;` 静默模式

`test_public_contract_all_retained_algorithms_are_accessible` 用 9 个 `(void)var;` 静默未用变量警告。该测试本质是 API 可访问性检查，应编译期验证而非运行时凑数。

### #11 冗余版本断言

`test_public_contract_version_macros_are_defined` 运行时断言 `BITCAL_VERSION_MAJOR/MINOR/PATCH` + 组合值。文件顶部已有 4 个 `static_assert` 覆盖同内容，且组合值由分量位运算推导，重复。运行时版本断言无额外价值——版本是编译期常量。

## 代码改动

### `include/bitcal/bit_block.hpp` (#8)

- 新增 `detail::copy_words_helper<N>(src, dst, byte_copy)` 模板
  - 封装 `if consteval { per-word loop } else { byte_copy(dst, src, N*8) }` 分支
  - `ByteCopy` 为 lambda，调用方传入 `memcpy` 或 `memmove`
  - 注释说明为何 `from_words` 用 `memcpy`（src 是 const span，无 overlap）而 `copy_words_to` 用 `memmove`（支持 self-copy，见 `test_vnext_block_copy_words_to_supports_self_copy`）
- `from_words` / `copy_words_to` 改为单行调用 `copy_words_helper` + 对应 byte-copy lambda

### `tests/test_bitcal.cpp` (#9)

- `test_public_contract_all_retained_algorithms_are_accessible` 中 9 个 `(void)var;` 改为 `[[maybe_unused]]` 声明
- 新增 9 个 `static_assert` 编译期验证 API 形状：
  - `bit_and/or/xor/andnot<256>` 返回 `bit_block<256>`
  - `shift_left/right<256>` 返回 `bit_block<256>`
  - `is_zero` 返回 `bool`
  - `popcount` 返回 `std::uint64_t`
  - `equals` 返回 `bool`
- 注释明确：运行时函数仅为 test runner 提供可调用入口，真正 API 契约在 `static_assert`

### `tests/test_bitcal.cpp` (#11)

- 删除 `test_public_contract_version_macros_are_defined` 函数（已被文件顶部 `static_assert` 覆盖）
- 删除 `main()` 中对应 `run_case` 调用

## 收益

- `bit_block` 消除 ~10 行 `if consteval` 分支重复，单一修改点
- 测试 API 契约从运行时凑数升级为编译期 `static_assert`，失败更早暴露
- 消除 9 个 `(void)var;` 异味，改用语言原生 `[[maybe_unused]]`
- 删除冗余运行时版本断言，减少测试噪声

## 验证

- `ctest` 全部通过
- 所有新增 `static_assert` 编译期通过（含返回类型、invocability）
- `test_vnext_block_copy_words_to_supports_self_copy` 仍通过（`memmove` 路径保留）

## 后续

Clean Code 评估识别的高/中收益项已全部完成（A/B/C/D/E 批次）。剩余低收益项（如 `main()` run_case 列表数据驱动化）争议较大，暂不推进。
