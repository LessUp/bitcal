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
  srcExclude: [
    'README.md',
    'project-status.md',
    'design/**',
    'superpowers/**',
  ],
  title: 'BitCal',
  description: 'Whitepaper-first SIMD bit computation for C++23',

  // 重定向规则：旧链接 → 新链接
  rewrites: {
    // 中文重定向
    'zh/getting-started/:splat*': 'zh/guide/:splat*',
    'zh/api/:splat*': 'zh/reference/:splat*',
    'zh/architecture/:splat*': 'zh/whitepaper/:splat*',
    'zh/performance/index.md': 'zh/whitepaper/performance.md',
    'zh/performance/methodology.md': 'zh/whitepaper/performance.md',
    // 英文重定向
    'en/getting-started/:splat*': 'en/guide/:splat*',
    'en/api/:splat*': 'en/reference/:splat*',
    'en/architecture/:splat*': 'en/whitepaper/:splat*',
    'en/performance/index.md': 'en/whitepaper/performance.md',
    'en/performance/methodology.md': 'en/whitepaper/performance.md',
  },

  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: 'BitCal',
      description: '面向高级开发者的 C++23 SIMD 位计算白皮书站点',
      themeConfig: {
        nav: [
          { text: '学院', link: '/zh/academy/index', activeMatch: '/zh/academy/' },
          { text: '白皮书', link: '/zh/whitepaper/index', activeMatch: '/zh/whitepaper/' },
          { text: '指南', link: '/zh/guide/index', activeMatch: '/zh/guide/' },
          { text: '参考', link: '/zh/reference/index', activeMatch: '/zh/reference/' },
          { text: '研究', link: '/zh/research/index', activeMatch: '/zh/research/' },
          { text: '状态', link: '/zh/status/index', activeMatch: '/zh/status/' },
        ],
        sidebar: {
          '/zh/academy/': [
            {
              text: '学院',
              items: [
                { text: '学习路径', link: '/zh/academy/index' },
                { text: '为什么选择 BitCal', link: '/zh/academy/why-bitcal' },
                { text: '位运算心智模型', link: '/zh/academy/bit-mental-model' },
                { text: 'SIMD 入门', link: '/zh/academy/simd-primer' },
                { text: '术语表', link: '/zh/academy/terminology' },
              ],
            },
          ],
          '/zh/guide/': [
            {
              text: '导读',
              items: [
                { text: '阅读入口', link: '/zh/guide/index' },
                { text: '验证路径', link: '/zh/guide/verification' },
                { text: '迁移姿态', link: '/zh/guide/migration-posture' },
              ],
            },
          ],
          '/zh/whitepaper/': [
            {
              text: '白皮书',
              items: [
                { text: '总览', link: '/zh/whitepaper/index' },
                { text: '公开模型', link: '/zh/whitepaper/public-model' },
                { text: '算法设计', link: '/zh/whitepaper/algorithm-design' },
                { text: '分发与内核', link: '/zh/whitepaper/dispatch-and-kernels' },
                { text: '性能', link: '/zh/whitepaper/performance' },
              ],
            },
          ],
          '/zh/reference/': [
            {
              text: '参考',
              items: [
                { text: '总览', link: '/zh/reference/index' },
                { text: '类型与视图', link: '/zh/reference/types-and-views' },
                { text: '算法契约', link: '/zh/reference/algorithms' },
              ],
            },
          ],
          '/zh/research/': [
            {
              text: '研究',
              items: [
                { text: '总览', link: '/zh/research/index' },
                { text: '演进与比较', link: '/zh/research/evolution-and-comparisons' },
              ],
            },
          ],
          '/zh/status/': [
            {
              text: '状态',
              items: [
                { text: '项目状态', link: '/zh/status/index' },
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
      title: 'BitCal',
      description: 'A C++23 SIMD bit computation whitepaper for advanced developers',
      themeConfig: {
        nav: [
          { text: 'Academy', link: '/en/academy/index', activeMatch: '/en/academy/' },
          { text: 'Whitepaper', link: '/en/whitepaper/index', activeMatch: '/en/whitepaper/' },
          { text: 'Guide', link: '/en/guide/index', activeMatch: '/en/guide/' },
          { text: 'Reference', link: '/en/reference/index', activeMatch: '/en/reference/' },
          { text: 'Research', link: '/en/research/index', activeMatch: '/en/research/' },
          { text: 'Status', link: '/en/status/index', activeMatch: '/en/status/' },
        ],
        sidebar: {
          '/en/academy/': [
            {
              text: 'Academy',
              items: [
                { text: 'Learning Paths', link: '/en/academy/index' },
                { text: 'Why BitCal', link: '/en/academy/why-bitcal' },
                { text: 'Bit Mental Model', link: '/en/academy/bit-mental-model' },
                { text: 'SIMD Primer', link: '/en/academy/simd-primer' },
                { text: 'Terminology', link: '/en/academy/terminology' },
              ],
            },
          ],
          '/en/guide/': [
            {
              text: 'Guide',
              items: [
                { text: 'Reading Entry', link: '/en/guide/index' },
                { text: 'Verification Path', link: '/en/guide/verification' },
                { text: 'Migration Posture', link: '/en/guide/migration-posture' },
              ],
            },
          ],
          '/en/whitepaper/': [
            {
              text: 'Whitepaper',
              items: [
                { text: 'Overview', link: '/en/whitepaper/index' },
                { text: 'Public Model', link: '/en/whitepaper/public-model' },
                { text: 'Algorithm Design', link: '/en/whitepaper/algorithm-design' },
                { text: 'Dispatch and Kernels', link: '/en/whitepaper/dispatch-and-kernels' },
                { text: 'Performance', link: '/en/whitepaper/performance' },
              ],
            },
          ],
          '/en/reference/': [
            {
              text: 'Reference',
              items: [
                { text: 'Overview', link: '/en/reference/index' },
                { text: 'Types and Views', link: '/en/reference/types-and-views' },
                { text: 'Algorithm Contract', link: '/en/reference/algorithms' },
              ],
            },
          ],
          '/en/research/': [
            {
              text: 'Research',
              items: [
                { text: 'Overview', link: '/en/research/index' },
                { text: 'Evolution and Comparisons', link: '/en/research/evolution-and-comparisons' },
              ],
            },
          ],
          '/en/status/': [
            {
              text: 'Status',
              items: [
                { text: 'Project Status', link: '/en/status/index' },
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
    footer: {
      message: 'Whitepaper-first technical documentation for BitCal vNext.',
      copyright: 'Released under MIT',
    },
  },

  vite: {
    plugins: [
      llmstxt({
        ignoreFiles: [
          'README.md',
          'project-status.md',
          'design/**',
          'superpowers/**',
        ],
      }),
    ],
  },

  ignoreDeadLinks: true,
}))
