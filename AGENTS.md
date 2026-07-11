# BitCal Agent Operating Manual

本文件是 BitCal 仓库内 AI/自动化协作者的执行手册。

## 1. 项目定位

BitCal 是一个 C++23、header-only、实验性质的 SIMD 位运算练习库。

默认目标：
- 小巧、可读、可快速迭代
- 对外描述必须与实现一致
- 优先删复杂度，不堆流程

## 2. 仓库真相

### 2.1 契约真相

- 代码真相：`include/bitcal/`
- 行为真相：`tests/test_bitcal.cpp`
- 使用入口：`README.md`（中文为准）
- 变更记录：根 `CHANGELOG.md`（中文为准）

### 2.2 代码与后端范围

- 当前公开后端仅保留：`scalar`、`avx2`
- 不保留无调用的“预留后端”实现文件
- 不保留死宏生成层

### 2.3 目录职责

- 核心库：`include/bitcal/*.hpp`
- 测试：`tests/test_bitcal.cpp`
- 示例：`examples/*.cpp`
- 基准：`benchmarks/*.cpp`
- 构建：`CMakeLists.txt` 与 `cmake/`

## 3. 工作方式

- 代码先行，小步提交
- 改动围绕一个目标闭环：实现 + 测试 + 文档
- 不引入大型流程门禁；保持可回滚边界

## 4. 实现约束

### 4.1 API 与行为

- 保持 `<bitcal/bitcal.hpp>` 作为稳定入口
- 公开 API 变化必须同步 README 与测试
- 禁止“文档已支持、实现未支持”的漂移

### 4.2 测试

- 只用现有轻量宏测试框架
- 新行为优先补到 `tests/test_bitcal.cpp`
- 删除不再支持行为的测试

### 4.3 工程化

- BitCal 是 header-only；CI/CD 只保留最小可信路径
- 不保留低价值 workflow
- 版本号单一事实源：`include/bitcal/config.hpp`

## 5. 文档约束

- 不维护 Markdown/HTML 双轨镜像
- README / CHANGELOG 单语（中文为准），不保留 `.zh-CN` 镜像
- 不保留独立文档站点（已删 `docs/`）；README 承载全部对外说明

## 6. AI 工具链约束

- `AGENTS.md` 是唯一跨代理规则源（已合并原 `CLAUDE.md` 与 `.github/copilot-instructions.md`）
- 与维护者统一中文沟通
- 阶段性大改优先 review；发现漂移先收敛一致性再扩展

原则：只保留真能降低成本的资产。

## 7. 已验证本地命令

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build --config Release -j"$(nproc)"
ctest --test-dir build --output-on-failure -C Release
```

## 8. 禁止事项

- 不制造无调用死代码
- 不保留过时后端声明
- 不把实验仓库治理成重流程系统
- 不引入与体量不匹配的基础设施

## 9. 完成标准

- API/测试/README/CHANGELOG 一致
- 代码路径可解释，无摆设层
- CI 保持最小且稳定
- 新增复杂度有明确收益
