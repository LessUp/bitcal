import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'
import {
  compatPageExcludes,
  llmsDerivedPageExcludes,
} from './page-exclusions.js'

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
  cleanUrls: true,


  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: 'BitCal',
      description: '面向高级开发者的 C++23 SIMD 位计算白皮书站点',
      themeConfig: {
        nav: [
          { text: '导读', link: '/zh/guide/', activeMatch: '/zh/guide/' },
          { text: '白皮书', link: '/zh/whitepaper/', activeMatch: '/zh/whitepaper/' },
          { text: '性能', link: '/zh/performance/', activeMatch: '/zh/performance/' },
          { text: '参考', link: '/zh/reference/', activeMatch: '/zh/reference/' },
          { text: '研究', link: '/zh/research/', activeMatch: '/zh/research/' },
          { text: '状态', link: '/zh/status/', activeMatch: '/zh/status/' },
        ],
        sidebar: {
          '/zh/guide/': [
            {
              text: '导读',
              items: [
                { text: '阅读入口', link: '/zh/guide/' },
                { text: '验证路径', link: '/zh/guide/verification' },
                { text: '迁移姿态', link: '/zh/guide/migration-posture' },
              ],
            },
          ],
          '/zh/whitepaper/': [
            {
              text: '白皮书',
              items: [
                { text: '总览', link: '/zh/whitepaper/' },
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
                { text: '总览', link: '/zh/performance/' },
              ],
            },
          ],
          '/zh/reference/': [
            {
              text: '参考',
              items: [
                { text: '总览', link: '/zh/reference/' },
                { text: '类型与视图', link: '/zh/reference/types-and-views' },
                { text: '算法契约', link: '/zh/reference/algorithms' },
              ],
            },
          ],
          '/zh/research/': [
            {
              text: '研究',
              items: [
                { text: '总览', link: '/zh/research/' },
                { text: '演进与比较', link: '/zh/research/evolution-and-comparisons' },
              ],
            },
          ],
          '/zh/status/': [
            {
              text: '状态',
              items: [
                { text: '总览', link: '/zh/status/' },
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
          { text: 'Guide', link: '/en/guide/', activeMatch: '/en/guide/' },
          { text: 'Whitepaper', link: '/en/whitepaper/', activeMatch: '/en/whitepaper/' },
          { text: 'Performance', link: '/en/performance/', activeMatch: '/en/performance/' },
          { text: 'Reference', link: '/en/reference/', activeMatch: '/en/reference/' },
          { text: 'Research', link: '/en/research/', activeMatch: '/en/research/' },
          { text: 'Status', link: '/en/status/', activeMatch: '/en/status/' },
        ],
        sidebar: {
          '/en/guide/': [
            {
              text: 'Guide',
              items: [
                { text: 'Reading Entry', link: '/en/guide/' },
                { text: 'Verification Path', link: '/en/guide/verification' },
                { text: 'Migration Posture', link: '/en/guide/migration-posture' },
              ],
            },
          ],
          '/en/whitepaper/': [
            {
              text: 'Whitepaper',
              items: [
                { text: 'Overview', link: '/en/whitepaper/' },
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
                { text: 'Overview', link: '/en/performance/' },
              ],
            },
          ],
          '/en/reference/': [
            {
              text: 'Reference',
              items: [
                { text: 'Overview', link: '/en/reference/' },
                { text: 'Types and Views', link: '/en/reference/types-and-views' },
                { text: 'Algorithm Contract', link: '/en/reference/algorithms' },
              ],
            },
          ],
          '/en/research/': [
            {
              text: 'Research',
              items: [
                { text: 'Overview', link: '/en/research/' },
                { text: 'Evolution and Comparisons', link: '/en/research/evolution-and-comparisons' },
              ],
            },
          ],
          '/en/status/': [
            {
              text: 'Status',
              items: [
                { text: 'Overview', link: '/en/status/' },
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
          ...compatPageExcludes,
          ...llmsDerivedPageExcludes,
        ],
      }),
    ],
  },

  ignoreDeadLinks: true,
}))
