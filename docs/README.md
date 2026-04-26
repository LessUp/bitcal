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
- 根 `CHANGELOG.md` / `CHANGELOG.zh-CN.md`：版本与变更记录的唯一对外入口

### 2.3 文档真相

- `docs/` 承担 GitHub Pages 内容来源
- 深度内容的 canonical source 应优先收敛到 Markdown
- 手工维护的 HTML 页面只允许保留在首页/落地页/导航页等确有展示价值的位置
- 对于同一主题，禁止长期并行维护“手写 Markdown + 手写 HTML”两套内容

### 2.4 渲染与派生

- 搜索索引、静态资源、派生 HTML 都应服务于 canonical source
- 若某个 HTML 页面无法明确对应一个 canonical source，则该页面属于清理候选

## 3. 最终信息架构

### 3.1 Home

- 根 `index.html`
- 作用：项目定位、亮点、支持边界、进入文档与源码的导航

### 3.2 Docs Landing

- `docs/index.html`
- 作用：文档总览、受众分流、语言入口、主要文档域导航

### 3.3 Getting Started

- 安装
- 快速开始
- 构建/验证方式

### 3.4 API Reference

- 面向仍然保留的公开 API
- 内容必须与 `openspec/specs/api/`、实现和测试一致

### 3.5 Architecture

- 只保留解释核心 SIMD dispatch、内存布局、平台边界所需的架构文档
- 历史性或重复性设计说明优先迁出主阅读路径

### 3.6 Project Status

- 明确项目处于“稳定收尾 / 可归档维护”姿态
- 说明支持边界、破坏性变更策略、规范与开发流程入口

## 4. 双语策略

### 必须双语

- 仓库入口 README
- Pages 首页与 docs landing
- 安装/快速开始
- 项目状态与维护边界说明

### 优先英文、按需提供中文

- 深度 API 参考
- 深度架构说明
- 仅面向维护者的工程细节

### 不再保留的模式

- 为了“看起来完整”而做的全量镜像翻译
- 中文/英文两套页面长期不同步
- changelog 在 `docs/` 下继续重复镜像

## 5. 清理规则

- 低价值 changelog 镜像：删除
- 与 README 重复的落地文档：合并或删除
- 与 Markdown 同步成本过高的手写 HTML 参考页：删除或改为派生
- 无清晰受众的文档：删除

## 6. 执行顺序

1. 先按本文件确定保留矩阵
2. 再删除重复文档与镜像内容
3. 然后重建首页、landing page 与导航
4. 最后补齐仍需保留的双语入口与维护状态说明
