# BitCal Documentation System

本文件定义 BitCal 文档系统的最终信息架构、双语策略与 canonical source policy。

## 1. 文档目标

BitCal 文档不再追求“大而全镜像”，而是追求：

- 对外可快速理解项目价值与边界
- 对内可低成本维护
- 与实现、测试、版本和 OpenSpec 保持一致
- 为 GitHub Pages 提供清晰的站点结构

## 2. Canonical Source Policy

### 2.1 规范真相

- 规范与需求真相：`openspec/`
- `specs/` 仅作迁移壳层，不承载活动规范

### 2.2 项目入口

- `README.md` / `README.zh-CN.md`：仓库入口、价值主张、快速开始摘要
- 根 `CHANGELOG.md` / `CHANGELOG.zh-CN.md`：版本与变更记录的唯一 canonical source
- `docs/` 下 release-notes changelog 页面若保留，只能作为 GitHub Pages 导航入口页，并必须从根 changelog 同步/派生

### 2.3 文档真相

- `docs/` 承担 GitHub Pages 内容来源
- 深度内容的 canonical source 应优先收敛到 Markdown
- 手工维护的 HTML 页面只允许保留在首页/落地页/导航页等确有展示价值的位置
- 对于同一主题，禁止长期并行维护“手写 Markdown + 手写 HTML”两套内容

### 2.4 渲染与派生

- 搜索索引、静态资源、派生 HTML 都应服务于 canonical source
- Pages 入口页可以基于 canonical Markdown 生成或同步，但不得自成另一套事实来源
- 若某个 HTML 页面或 release-notes 入口无法明确对应一个 canonical source，则该页面属于清理候选

## 3. 最终信息架构

### 3.1 Home

- 根 `index.html`
- 作用：项目定位、亮点、支持边界、进入文档与源码的导航

### 3.2 Docs Landing

- `docs/index.html`
- 作用：文档总览、受众分流、语言入口、主要文档域导航

### 3.3 Whitepaper IA

- Home
- Academy
- Whitepaper
- Guides
- Reference
- Research
- Project Status

### 3.4 Canonical Content Boundaries

- `Guides` 承担安装、快速开始、构建/验证方式与迁移指引
- `Reference` 面向仍然保留的公开 API，内容必须与 `openspec/specs/api/`、实现和测试一致
- `Whitepaper` 与 `Research` 负责 vNext 架构叙事、设计依据与方法论材料
- `Project Status` 说明支持边界、破坏性变更策略、规范与开发流程入口
- 历史性或重复性设计说明优先迁出主阅读路径

## 4. 双语策略

- 本轮 GitHub Pages 面向主要受众的导航结构按中英文镜像维护
- 深度技术正文以共享结构和共享图示资产降低重复成本，并可按 policy 预先声明受控例外
- 禁止通过手写 HTML 镜像来维持双语“看起来完整”
- release-notes changelog 页若保留，仅作为从根 changelog 同步而来的 Pages 入口，不作为独立 canonical 文档

## 5. 清理规则

- 低价值或失去同步依据的 changelog 手工镜像：删除
- 与 README 重复的落地文档：合并或删除
- 与 Markdown 同步成本过高的手写 HTML 参考页：删除或改为派生
- 无清晰受众的文档：删除

## 6. 执行顺序

1. 先按本文件确定保留矩阵
2. 再删除重复文档与镜像内容
3. 然后重建首页、landing page 与导航
4. 最后补齐仍需保留的双语入口与维护状态说明
