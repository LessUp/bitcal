# CONTRIBUTING

感谢你对 BitCal 的兴趣！本文档说明如何贡献代码。

## 开发环境

### 要求
- C++17 编译器 (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.16+

### 构建

```bash
git clone https://github.com/LessUp/bitcal.git
cd bitcal
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### 运行测试

```bash
./tests/test_bitcal
```

## 代码风格

- **文件命名**: 小写 + 下划线 (`scalar_ops.hpp`)
- **命名空间**: `bitcal::`
- **类名**: 小写 + 下划线 (`bitarray`)
- **函数名**: 小写 + 下划线 (`shift_left`)

## 提交规范

使用语义化提交信息：

```
feat: 添加新功能
fix: 修复 bug
docs: 文档更新
perf: 性能优化
refactor: 代码重构
test: 测试相关
chore: 构建/工具相关
```

## 添加新 SIMD 后端

1. 在 `include/bitcal/` 创建 `<backend>_ops.hpp`
2. 在 `config.hpp` 添加平台检测和 `BITCAL_HAS_*` 宏
3. 在 `simd_backend` 枚举添加新后端
4. 在 `bitcal.hpp` 添加分派逻辑
5. 添加测试用例

## 发布流程

1. 更新 `CHANGELOG.md`
2. 更新版本号 (`config.hpp`, `CMakeLists.txt`)
3. 创建 tag: `git tag v2.x.x`
4. 推送: `git push --tags`
