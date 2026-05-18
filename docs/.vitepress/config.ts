import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const legacyAcademySourceExcludes = [
  'en/academy/bit-mental-model.md',
  'en/academy/overview.md',
  'en/academy/simd-primer.md',
  'en/academy/terminology.md',
  'en/academy/why-bitcal.md',
  'zh/academy/bit-mental-model.md',
  'zh/academy/overview.md',
  'zh/academy/simd-primer.md',
  'zh/academy/terminology.md',
  'zh/academy/why-bitcal.md',
]

const llmsCompatibilityExcludes = [
  'en/academy/index.md',
  'zh/academy/index.md',
  'en/project-status/index.md',
  'zh/project-status/index.md',
  'en/getting-started/migration.md',
  'zh/getting-started/migration.md',
  'en/api/overview.md',
  'zh/api/overview.md',
  'en/whitepaper/performance.md',
  'zh/whitepaper/performance.md',
  'en/release-notes/changelog.md',
  'zh/release-notes/changelog.md',
]

export default withMermaid(defineConfig({
  base,
  srcExclude: [
    'README.md',
    'project-status.md',
    'design/**',
    'superpowers/**',
    ...legacyAcademySourceExcludes,
  ],
  title: 'BitCal',
  description: 'Whitepaper-first SIMD bit computation for C++23',

  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: 'BitCal',
      description: '面向高级开发者的 C++23 SIMD 位计算白皮书站点',
      themeConfig: {
        nav: [
          { text: '导读', link: '/zh/guide/index', activeMatch: '/zh/guide/' },
          { text: '白皮书', link: '/zh/whitepaper/index', activeMatch: '/zh/whitepaper/' },
          { text: '性能', link: '/zh/performance/index', activeMatch: '/zh/performance/' },
          { text: '参考', link: '/zh/reference/index', activeMatch: '/zh/reference/' },
          { text: '研究', link: '/zh/research/index', activeMatch: '/zh/research/' },
          { text: '状态', link: '/zh/status/index', activeMatch: '/zh/status/' },
        ],
        sidebar: {
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
              ],
            },
          ],
          '/zh/performance/': [
            {
              text: '性能',
              items: [
                { text: '总览', link: '/zh/performance/index' },
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
                { text: '总览', link: '/zh/status/index' },
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
          { text: 'Guide', link: '/en/guide/index', activeMatch: '/en/guide/' },
          { text: 'Whitepaper', link: '/en/whitepaper/index', activeMatch: '/en/whitepaper/' },
          { text: 'Performance', link: '/en/performance/index', activeMatch: '/en/performance/' },
          { text: 'Reference', link: '/en/reference/index', activeMatch: '/en/reference/' },
          { text: 'Research', link: '/en/research/index', activeMatch: '/en/research/' },
          { text: 'Status', link: '/en/status/index', activeMatch: '/en/status/' },
        ],
        sidebar: {
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
              ],
            },
          ],
          '/en/performance/': [
            {
              text: 'Performance',
              items: [
                { text: 'Overview', link: '/en/performance/index' },
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
                { text: 'Overview', link: '/en/status/index' },
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
          ...legacyAcademySourceExcludes,
          ...llmsCompatibilityExcludes,
        ],
      }),
    ],
  },

  ignoreDeadLinks: true,
}))
