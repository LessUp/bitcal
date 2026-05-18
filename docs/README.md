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
- route alias 与语言分流页只能服务主站结构，不能再成为另一套主路径真相

## 3. 最终信息架构

最终主站模型统一为 `Home / Docs Landing / Guide / Whitepaper / Performance / Reference / Research / Status`。

### 3.1 Home

- 根 `index.html`
- 作用：项目定位、亮点、支持边界、进入文档与源码的导航

### 3.2 Docs Landing

- `docs/index.html`
- 作用：文档总览、受众分流、语言入口、主要文档域导航

### 3.3 Guide

- 面向首次评估者的阅读入口
- 只保留安装、验证路径、迁移姿态等高价值导读
- 不把深度架构内容塞进入门路径

### 3.4 Whitepaper

- 承载 BitCal vNext 的系统设计主线
- 解释公开模型、算法组织、分发边界、支持姿态与限制
- 应当优先服务高级开发者的架构审阅场景

### 3.5 Performance

- 性能页必须独立成 section，而不是继续挂靠在旧白皮书命名之下
- 必须区分 baseline、方法学和公开宣称边界
- 不允许把局部 benchmark 当作无条件产品承诺

### 3.6 Reference

- 面向仍然保留的公开契约
- 内容必须与 `openspec/`、实现和测试一致
- 优先解释类型、视图与算法契约，而不是复制源码结构

### 3.7 Research

- 收纳参考文献、相关项目、演进思考与方法学说明
- 目的是提升论证深度，而不是制造装饰性“学术感”

### 3.8 Status

- 明确项目当前姿态、支持边界、breaking-change 策略与规范入口
- 状态页必须比 README 更严格，而不是更宽松
- 旧的 `project-status` 仅允许作为兼容 alias，不再作为主路径命名

## 4. 双语策略

### 必须双语

- 仓库入口 README
- Pages 首页与 docs landing
- Guide / Whitepaper / Performance / Reference / Research / Status 的主入口页
- 状态与维护边界说明

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
- academy、旧 architecture、`project-status` 只允许作为兼容入口，不能继续占据主导航或主阅读路径

## 6. 执行顺序

1. 先按本文件确定保留矩阵
2. 再删除重复文档与镜像内容
3. 然后重建首页、landing page 与导航
4. 最后补齐仍需保留的双语入口与维护状态说明
