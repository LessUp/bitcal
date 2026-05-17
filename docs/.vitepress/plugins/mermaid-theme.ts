/**
 * Mermaid 主题配置
 * 支持深浅色主题自动切换
 *
 * 注意：颜色值与 styles/variables.css 中的设计令牌对应
 * - primaryColor → --bc-diagram-node
 * - primaryTextColor → --bc-text-primary
 * - lineColor → --bc-diagram-edge
 * - mainBkg → --bc-bg / --bc-bg-elevated
 */

export interface MermaidThemeConfig {
  startOnLoad: boolean
  theme: string
  themeVariables: {
    primaryColor: string
    primaryTextColor: string
    primaryBorderColor: string
    lineColor: string
    mainBkg: string
    nodeBorder: string
    clusterBkg: string
    fontFamily: string
  }
}

/**
 * 获取 Mermaid 配置
 * @param isDark 是否为深色模式
 * @returns Mermaid 配置对象
 */
export function getMermaidConfig(isDark: boolean): MermaidThemeConfig {
  return {
    startOnLoad: true,
    theme: isDark ? 'dark' : 'default',
    themeVariables: isDark
      ? {
          // 深色主题 - 对应 .dark 下的 CSS 变量
          primaryColor: 'var(--bc-diagram-node)', // --bc-blue-400: #60a5fa
          primaryTextColor: 'var(--bc-text-primary)', // --bc-slate-50: #f8fafc
          primaryBorderColor: 'var(--bc-border)', // --bc-slate-600: #475569
          lineColor: 'var(--bc-diagram-edge)', // --bc-slate-400: #94a3b8
          mainBkg: 'var(--bc-bg-elevated)', // --bc-slate-800: #1e293b
          nodeBorder: 'var(--bc-border)', // --bc-slate-600: #475569
          clusterBkg: 'var(--bc-bg-elevated)', // --bc-slate-800: #1e293b
          fontFamily: 'Inter, system-ui, sans-serif',
        }
      : {
          // 浅色主题 - 对应 :root 下的 CSS 变量
          primaryColor: 'var(--bc-diagram-node)', // --bc-blue-500: #3b82f6
          primaryTextColor: 'var(--bc-text-primary)', // --bc-slate-900: #0f172a
          primaryBorderColor: 'var(--bc-border)', // --bc-slate-200: #e2e8f0
          lineColor: 'var(--bc-diagram-edge)', // --bc-slate-500: #64748b
          mainBkg: 'var(--bc-bg)', // #ffffff
          nodeBorder: 'var(--bc-border)', // --bc-slate-200: #e2e8f0
          clusterBkg: 'var(--bc-bg-elevated)', // --bc-slate-50: #f8fafc
          fontFamily: 'Inter, system-ui, sans-serif',
        },
  }
}
