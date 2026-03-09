# GitHub Pages 优化 (2026-03-10)

## 文档站首页重写

- **gitbook/README.md** — 重写为专业着陆页：CI/Docs/License/C++17/Header-only 徽章、架构分层图、核心特性表格、x86 + ARM 性能数据、位宽×后端兼容矩阵、平台兼容性表、适用场景列表

## 工作流优化

- **docs.yml** — 扩展路径触发范围（`README.md`、`README.zh-CN.md`、`MIGRATION_GUIDE.md`），添加 sparse-checkout 仅拉取 `gitbook/` 目录加速构建

## 工程改进

- **README.md / README.zh-CN.md** — 统一添加 CI 徽章，徽章顺序调整为 CI → Docs → License
- **.gitignore** — 添加 `node_modules/`、`_site/`、`_book/`（honkit 构建产物）
