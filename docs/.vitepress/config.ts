import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const zhSidebar = {
  '/zh/academy/': [
    {
      text: '学院',
      items: [
        { text: '概览', link: '/zh/academy/overview' },
      ],
    },
  ],
  '/zh/architecture/': [
    {
      text: '白皮书',
      items: [
        { text: '概览', link: '/zh/architecture/overview' },
        { text: 'vNext 白皮书', link: '/zh/architecture/vnext-whitepaper' },
        { text: '平台支持', link: '/zh/architecture/platform-support' },
        { text: 'SIMD 分发', link: '/zh/architecture/simd-dispatch' },
        { text: '性能基线', link: '/zh/architecture/performance-baseline' },
      ],
    },
  ],
  '/zh/getting-started/': [
    {
      text: '指南',
      items: [
        { text: '安装指南', link: '/zh/getting-started/installation' },
        { text: '快速开始', link: '/zh/getting-started/quickstart' },
        { text: '构建选项', link: '/zh/getting-started/build-options' },
        { text: '迁移指南', link: '/zh/getting-started/migration' },
      ],
    },
  ],
  '/zh/api/': [
    {
      text: '参考',
      items: [
        { text: '总览', link: '/zh/api/overview' },
        { text: '类型', link: '/zh/api/types' },
        { text: '核心操作', link: '/zh/api/core-operations' },
        { text: '位操作', link: '/zh/api/bit-manipulation' },
        { text: '位计数', link: '/zh/api/bit-counting' },
        { text: '移位操作', link: '/zh/api/shift-operations' },
        { text: 'SIMD 后端', link: '/zh/api/simd-backend' },
      ],
    },
  ],
  '/zh/research/': [
    {
      text: '研究',
      items: [
        { text: '概览', link: '/zh/research/overview' },
      ],
    },
  ],
  '/zh/project-status/': [
    {
      text: '项目状态',
      items: [
        { text: '项目状态', link: '/zh/project-status/' },
      ],
    },
  ],
}

const enSidebar = {
  '/en/academy/': [
    {
      text: 'Academy',
      items: [
        { text: 'Overview', link: '/en/academy/overview' },
      ],
    },
  ],
  '/en/architecture/': [
    {
      text: 'Whitepaper',
      items: [
        { text: 'Overview', link: '/en/architecture/overview' },
        { text: 'vNext Whitepaper', link: '/en/architecture/vnext-whitepaper' },
        { text: 'Platform Support', link: '/en/architecture/platform-support' },
        { text: 'SIMD Dispatch', link: '/en/architecture/simd-dispatch' },
        { text: 'Performance Baseline', link: '/en/architecture/performance-baseline' },
      ],
    },
  ],
  '/en/getting-started/': [
    {
      text: 'Guides',
      items: [
        { text: 'Installation', link: '/en/getting-started/installation' },
        { text: 'Quick Start', link: '/en/getting-started/quickstart' },
        { text: 'Build Options', link: '/en/getting-started/build-options' },
        { text: 'Migration Guide', link: '/en/getting-started/migration' },
      ],
    },
  ],
  '/en/api/': [
    {
      text: 'Reference',
      items: [
        { text: 'Overview', link: '/en/api/overview' },
        { text: 'Types', link: '/en/api/types' },
        { text: 'Core Operations', link: '/en/api/core-operations' },
        { text: 'Bit Manipulation', link: '/en/api/bit-manipulation' },
        { text: 'Bit Counting', link: '/en/api/bit-counting' },
        { text: 'Shift Operations', link: '/en/api/shift-operations' },
        { text: 'SIMD Backend', link: '/en/api/simd-backend' },
      ],
    },
  ],
  '/en/research/': [
    {
      text: 'Research',
      items: [
        { text: 'Overview', link: '/en/research/overview' },
      ],
    },
  ],
  '/en/project-status/': [
    {
      text: 'Project Status',
      items: [
        { text: 'Project Status', link: '/en/project-status/' },
      ],
    },
  ],
}

export default withMermaid(defineConfig({
  base,
  title: 'BitCal vNext Docs',
  description: 'BitCal - Header-only C++23 vNext bit library',

  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: 'BitCal vNext 文档',
      description: 'BitCal - 头文件式 C++23 vNext 位运算库',
      themeConfig: {
        nav: [
          { text: '学院', link: '/zh/academy/overview', activeMatch: '/zh/academy/' },
          { text: '白皮书', link: '/zh/architecture/overview', activeMatch: '/zh/architecture/' },
          { text: '指南', link: '/zh/getting-started/installation', activeMatch: '/zh/getting-started/' },
          { text: '参考', link: '/zh/api/overview', activeMatch: '/zh/api/' },
          { text: '研究', link: '/zh/research/overview', activeMatch: '/zh/research/' },
          { text: '项目状态', link: '/zh/project-status/', activeMatch: '/zh/project-status/' },
        ],
        sidebar: zhSidebar,
      },
    },
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      title: 'BitCal vNext Docs',
      description: 'BitCal - Header-only C++23 vNext bit library',
      themeConfig: {
        nav: [
          { text: 'Academy', link: '/en/academy/overview', activeMatch: '/en/academy/' },
          { text: 'Whitepaper', link: '/en/architecture/overview', activeMatch: '/en/architecture/' },
          { text: 'Guides', link: '/en/getting-started/installation', activeMatch: '/en/getting-started/' },
          { text: 'Reference', link: '/en/api/overview', activeMatch: '/en/api/' },
          { text: 'Research', link: '/en/research/overview', activeMatch: '/en/research/' },
          { text: 'Status', link: '/en/project-status/', activeMatch: '/en/project-status/' },
        ],
        sidebar: enSidebar,
      },
    },
  },

  themeConfig: {
    outline: [2, 3],
    search: { provider: 'local' },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/bitcal' },
    ],
  },

  vite: {
    plugins: [llmstxt()],
  },

  ignoreDeadLinks: true,
}))
