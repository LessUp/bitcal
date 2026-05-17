import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import { onMounted, watch, ref } from 'vue'
import { useData } from 'vitepress'
import mermaid from 'mermaid'
import './style.css'

import ArchitectureDiagram from './components/ArchitectureDiagram.vue'
import BitcalHero from './components/BitcalHero.vue'
import CitationList from './components/CitationList.vue'
import EvidenceStrip from './components/EvidenceStrip.vue'
import FigureFrame from './components/FigureFrame.vue'
import ReadingModelDiagram from './components/ReadingModelDiagram.vue'
import ReadingPathGrid from './components/ReadingPathGrid.vue'
import { getMermaidConfig } from '../plugins/mermaid-theme'

// 用于跟踪 Mermaid 是否已初始化
let mermaidInitialized = false

/**
 * 初始化或重新配置 Mermaid 主题
 * @param isDark 是否为深色模式
 */
async function initMermaidTheme(isDark: boolean) {
  const config = getMermaidConfig(isDark)

  if (!mermaidInitialized) {
    mermaid.initialize(config)
    mermaidInitialized = true
  } else {
    // 重新初始化以应用新主题
    mermaid.initialize(config)
  }

  // 重新渲染所有 Mermaid 图表
  await renderMermaidDiagrams()
}

/**
 * 重新渲染页面上的所有 Mermaid 图表
 */
async function renderMermaidDiagrams() {
  // 查找所有未渲染或需要重新渲染的 mermaid 图表
  const mermaidElements = document.querySelectorAll('.mermaid')

  for (const element of mermaidElements) {
    const el = element as HTMLElement
    // 获取原始图表代码（存储在 data-original 属性中）
    let graphCode = el.getAttribute('data-original')

    if (!graphCode) {
      // 首次渲染，保存原始代码
      graphCode = el.textContent || ''
      el.setAttribute('data-original', graphCode)
    }

    try {
      // 使用唯一 ID 渲染
      const id = `mermaid-${Math.random().toString(36).substr(2, 9)}`
      const { svg } = await mermaid.render(id, graphCode)
      el.innerHTML = svg
    } catch (error) {
      console.warn('Mermaid render error:', error)
    }
  }
}

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('ArchitectureDiagram', ArchitectureDiagram)
    app.component('BitcalHero', BitcalHero)
    app.component('CitationList', CitationList)
    app.component('EvidenceStrip', EvidenceStrip)
    app.component('FigureFrame', FigureFrame)
    app.component('ReadingModelDiagram', ReadingModelDiagram)
    app.component('ReadingPathGrid', ReadingPathGrid)
  },
  setup() {
    const { isDark } = useData()

    // 组件挂载时初始化 Mermaid
    onMounted(() => {
      initMermaidTheme(isDark.value)
    })

    // 监听主题变化
    watch(isDark, (newIsDark) => {
      initMermaidTheme(newIsDark)
    })
  },
} satisfies Theme
