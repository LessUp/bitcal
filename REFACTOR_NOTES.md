# BitCal 重构说明

## 已完成的修复

### ✅ 1. 目录结构整理

**问题**: cpp和h文件混乱地放在根目录，不符合规范。

**解决**:
- 根目录现在只保留：
  - `include/` - 新版 header-only 实现（主线）
  - `examples/` - 示例代码
  - `tests/` - 测试代码
  - `docs/` - 中文文档

说明：已确认不需要兼容/legacy 路线，相关 `src/` 目录与兼容入口头已从仓库移除。

**目录结构**:
```
bitcal/
├── include/bitcal/          # 新版本 (推荐使用)
│   ├── bitcal.hpp
│   ├── config.hpp
│   ├── simd_traits.hpp
│   ├── scalar_ops.hpp
│   ├── sse_ops.hpp
│   ├── avx_ops.hpp
│   └── neon_ops.hpp
├── examples/
├── tests/
├── docs/
└── cmake/
```

### ✅ 2. 文件命名规范化

**问题**: 文件命名混用大写字母（如 `BitCal.cpp`, `CheckCpu.h`），不符合现代C++约定。

**解决**: 统一使用小写字母+下划线命名风格：
- `bitcal.hpp` 
- `config.hpp` 
- `simd_traits.hpp` 
- `scalar_ops.hpp` 
- `sse_ops.hpp` 
- `avx_ops.hpp` 
- `neon_ops.hpp` 
- `Encode.h` → `encode.h`
- `EncodeGpr.cpp` → `encode_gpr.cpp`
- `EncodeGpr.h` → `encode_gpr.h`
- `EncodeYmm.cpp` → `encode_ymm.cpp`
- `EncodeYmm.h` → `encode_ymm.h`

**符合标准**: Google C++ Style Guide, LLVM Coding Standards

### ✅ 3. 移除spdlog依赖

**问题**: 引入了额外的spdlog库，增加了依赖复杂度。

**解决**: 
- 移除 `#include "spdlog/spdlog.h"`
- 所有 `spdlog::error()` 替换为 `std::cerr`
- 所有 `spdlog::info()` 替换为 `std::cout`
- 使用C++标准库：`<iostream>` 和 `<stdexcept>`

**示例**:
```cpp
// 旧代码
#include "spdlog/spdlog.h"
spdlog::error("BitCal::{} input data is illegal!", __FUNCTION__);

// 新代码
#include <iostream>
std::cerr << "BitCal::convUint64ToBinStr: input data is illegal!" << std::endl;
```

**说明**: 对于库代码，应尽量减少依赖。日志功能可由用户自行集成或使用标准输出。

### ✅ 4. C++20 标准支持

**问题**: CMakeLists.txt使用C++17，未支持C++20新特性。

**解决**:
- `CMakeLists.txt`: `target_compile_features(bitcal INTERFACE cxx_std_17)`（主线保持 C++17，示例/测试可按需启用 C++20）
- `examples/CMakeLists.txt`: 添加 `target_compile_features(... cxx_std_20)`
- `tests/CMakeLists.txt`: 添加 `target_compile_features(... cxx_std_20)`

**C++20优势**:
- Concepts (概念)
- Ranges (范围库)
- Coroutines (协程)
- 更好的constexpr支持
- std::span, std::bit_cast等新工具

**当前实现**: 虽然设置了C++20，但现有代码使用C++17特性，保持向下兼容。未来可以逐步采用C++20特性优化。

## 🔄 待完成的修复

### 1. 批量替换spdlog调用

说明：已确认不再维护 legacy/dispatch 路线，相关目录已从仓库移除，此项不再适用。

**替换模式**:
```bash
# 使用sed或类似工具批量替换
sed -i 's/spdlog::error/std::cerr << /g' *.cpp
sed -i 's/spdlog::info/std::cout << /g' *.cpp
```

**或手工替换**: 由于格式化字符串差异，建议逐文件检查。

### 2. 更新头文件include路径

（legacy 路线已移除，此项不再适用）
```cpp
// 需要检查并更新
#include "Base.h"  → #include "base.h"
```

### 3. 更新原CMakeLists.txt

说明：已确认不再维护 legacy 路线，此项不再适用。

## 📊 大厂类似库参考

### Intel
- **Intel IPP** - 商业高性能库
- **Intel ISPC** - SIMD编译器

### Google
- **Abseil** (`absl/numeric/bits.h`) - 位操作工具
  ```cpp
  absl::popcount()
  absl::countl_zero()
  absl::countr_zero()
  ```
- **Highway** - 现代SIMD抽象库 ⭐
  - 设计理念与我们的新版本类似
  - Header-only可选
  - 编译期SIMD选择
  - 支持x86/ARM/RISC-V

### 微软
- **DirectXMath** - 游戏图形库
- **Windows SDK** - 内置位操作函数

### 开源社区
- **xsimd** (QuantStack) - 推荐参考 ⭐
  - 现代C++17设计
  - 类型安全的SIMD封装
  - 编译期优化
- **Boost.SIMD** - 已停止维护，但设计先进
- **simde** - SIMD模拟器，兼容性好

### 设计共识

所有现代库都采用以下设计：
1. ✅ **Header-only** 或可选header-only
2. ✅ **编译期分发** (compile-time dispatch)
3. ✅ **模板化接口** (template-based API)
4. ✅ **零开销抽象** (zero-overhead abstraction)
5. ✅ **标准库集成** (std::ranges, concepts等)

**我们的新设计 (`include/bitcal/`) 完全符合这些最佳实践！**

## 🎯 推荐使用方式

### 新项目 (推荐)
```cpp
#include <bitcal/bitcal.hpp>

bitcal::bit256 a(0xDEADBEEF);
bitcal::bit256 b(0xCAFEBABE);
auto c = a & b;
```

### 旧项目兼容
```cpp
// 已确认不再维护 legacy/dispatch 路线。
// 请直接使用 v2.x header-only 主线：
#include <bitcal/bitcal.hpp>
```

### 最佳实践
1. **新项目直接使用新版本** (`include/bitcal/`)
2. **旧项目逐步迁移** (参考 `MIGRATION_GUIDE.md`)
3. **不建议使用legacy代码** (仅用于过渡期)

## 📝 下一步行动

### 短期
- [ ] 增强文档与示例（持续）

### 中期
- [ ] 添加CI/CD自动化测试
- [ ] 性能基准测试
- [ ] 完整文档

### 长期
- [ ] 持续迭代 header-only 版本

## 🔍 命名规范对比

### Google C++ Style Guide
- 文件名: `lower_with_underscores.cpp`
- 类名: `CamelCase`
- 函数名: `CamelCase` 或 `lower_with_underscores`
- 变量名: `lower_with_underscores`

### LLVM Coding Standards
- 文件名: `UpperCamelCase.cpp` 或 `lower_underscores.cpp`
- 类名: `UpperCamelCase`
- 函数名: `lowerCamelCase`
- 变量名: `UpperCamelCase` 或 `lower_camel_case`

### STL风格
- 文件名: `lower_with_underscores`
- 一切都是小写+下划线

**我们选择**: 文件名使用小写+下划线（符合STL和大多数现代库）

## 总结

### 优点
✅ 目录结构清晰  
✅ 命名规范统一  
✅ 减少外部依赖  
✅ 支持现代C++20  
✅ 符合大厂最佳实践  

### 注意事项
⚠️ 本仓库仅保留 header-only 主线；不再提供 legacy/dispatch 兼容路线  
⚠️ 建议新项目直接使用 `include/bitcal/bitcal.hpp`  

---

**推荐**: 直接使用 `include/bitcal/bitcal.hpp` 新版本！
