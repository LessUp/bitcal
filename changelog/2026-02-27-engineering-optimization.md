# 工程全面优化 - 2026-02-27

## 构建系统

### CMake SIMD 编译标志自动检测
- 新增 `BITCAL_NATIVE_ARCH` 选项（默认 ON）
- GCC/Clang: 自动添加 `-march=native`
- MSVC: 自动检测并添加 `/arch:AVX2`
- SIMD 标志仅应用于 tests/examples，不污染 INTERFACE 库
- 版本号更新为 2.1.0

### C++ 标准统一
- `tests/CMakeLists.txt` 和 `examples/CMakeLists.txt` 从 `cxx_std_20` 改回 `cxx_std_17`
- 与库本身的 `cxx_std_17` 要求保持一致

## 测试

### 测试框架重构
- 从全局静态对象触发改为 main 驱动 + `RUN_TEST` 宏
- 新增 `ASSERT_EQ` / `ASSERT_TRUE` 宏，失败时打印具体值和行号
- main 返回值反映测试结果（失败返回 1）
- 统计 pass/fail 计数

### 新增测试用例（从 ~15 增至 30+）
- **128-bit shift cross-carry**: 验证 qword 边界进位
- **256-bit shift cross-carry**: 验证所有 4 个 qword 之间的进位链，特别是 word1→word2 的 128-bit 边界
- **256-bit shift 边界值**: shift 64, 65, 255, 256
- **512-bit shift cross-carry**: 验证 word3→word4 的 256-bit 边界
- **512-bit shift 边界值**: shift 511, 512
- **ANDNOT**: 64-bit 和 256-bit
- **NOT**: 128-bit 和 256-bit
- **is_zero**: 各位宽 + 非零检测
- **256-bit reverse**

## Benchmark 改进

- 添加 `do_not_optimize()` 防止编译器消除 benchmark 循环
- 添加 warmup 阶段（10% 迭代次数）
- 所有位宽新增 ANDNOT benchmark
- 移除 unused variable warnings

## 示例

- `basic_usage.cpp` 新增 ANDNOT 操作演示

## 文档

### README.md
- 新增 `andnot(mask)` 和 `is_zero()` 到 API 列表
- 移除占位符链接（yourusername, your.email）
- 添加版本标记 v2.1.0

### SUMMARY.md
- 版本号更新为 2.1.0
- 日期更新为 2026-02-27

### .gitignore
- 新增 `.vs/`、`*.suo`、`*.user`、`*.vcxproj.user`（Visual Studio）
- 新增 `.windsurf/`

## 修改的文件

| 文件 | 变更 |
|------|------|
| `CMakeLists.txt` | SIMD 标志 + 版本号 |
| `tests/CMakeLists.txt` | C++17 + SIMD 标志 |
| `tests/test_bitcal.cpp` | 完全重写 |
| `examples/CMakeLists.txt` | C++17 + SIMD 标志 |
| `examples/basic_usage.cpp` | +ANDNOT 演示 |
| `examples/performance_comparison.cpp` | warmup + DoNotOptimize |
| `README.md` | API 更新 + 版本标记 |
| `SUMMARY.md` | 版本更新 |
| `.gitignore` | +Windows/VS 项 |
