import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

export default withMermaid(defineConfig({
  base,
  title: 'BitCal Docs',
  description: 'BitCal - Header-only SIMD bit manipulation for C++17',

  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: 'BitCal 文档',
      description: 'BitCal - C++17 头文件式 SIMD 位操作库',
      themeConfig: {
        nav: [
          { text: '指南', link: '/zh/getting-started/installation', activeMatch: '/zh/getting-started/' },
          { text: '架构', link: '/zh/architecture/overview', activeMatch: '/zh/architecture/' },
          { text: 'API', link: '/en/api/types', activeMatch: '/en/api/' },
        ],
        sidebar: {
          '/zh/getting-started/': [
            {
              text: '指南',
              items: [
                { text: '安装指南', link: '/zh/getting-started/installation' },
                { text: '快速开始', link: '/zh/getting-started/quickstart' },
                { text: '构建选项', link: '/zh/getting-started/build-options' },
              ],
            },
          ],
          '/zh/architecture/': [
            {
              text: '架构',
              items: [
                { text: '概览', link: '/zh/architecture/overview' },
                { text: '平台支持', link: '/zh/architecture/platform-support' },
                { text: 'SIMD 分发', link: '/zh/architecture/simd-dispatch' },
              ],
            },
          ],
        },
      },
    },
    en: {
      label: 'English',
      lang: 'en-US',
      link: '/en/',
      title: 'BitCal Docs',
      description: 'BitCal - Header-only SIMD bit manipulation for C++17',
      themeConfig: {
        nav: [
          { text: 'Guides', link: '/en/getting-started/installation', activeMatch: '/en/getting-started/' },
          { text: 'Architecture', link: '/en/architecture/overview', activeMatch: '/en/architecture/' },
          { text: 'API', link: '/en/api/types', activeMatch: '/en/api/' },
        ],
        sidebar: {
          '/en/getting-started/': [
            {
              text: 'Guides',
              items: [
                { text: 'Installation', link: '/en/getting-started/installation' },
                { text: 'Quick Start', link: '/en/getting-started/quickstart' },
                { text: 'Build Options', link: '/en/getting-started/build-options' },
              ],
            },
          ],
          '/en/architecture/': [
            {
              text: 'Architecture',
              items: [
                { text: 'Overview', link: '/en/architecture/overview' },
                { text: 'Platform Support', link: '/en/architecture/platform-support' },
                { text: 'SIMD Dispatch', link: '/en/architecture/simd-dispatch' },
              ],
            },
          ],
          '/en/api/': [
            {
              text: 'API Reference',
              items: [
                { text: 'Types', link: '/en/api/types' },
                { text: 'Core Operations', link: '/en/api/core-operations' },
                { text: 'Bit Manipulation', link: '/en/api/bit-manipulation' },
                { text: 'Bit Counting', link: '/en/api/bit-counting' },
                { text: 'Shift Operations', link: '/en/api/shift-operations' },
                { text: 'SIMD Backend', link: '/en/api/simd-backend' },
              ],
            },
          ],
        },
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
