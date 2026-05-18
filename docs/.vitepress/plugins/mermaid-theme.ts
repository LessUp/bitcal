/**
 * Mermaid 主题配置
 * 支持深浅色主题自动切换，并与 BitCal whitepaper tokens 对齐。
 */

export interface MermaidThemeConfig {
  startOnLoad: boolean
  theme: string
  darkMode: boolean
  themeCSS: string
  themeVariables: {
    primaryColor: string
    primaryTextColor: string
    primaryBorderColor: string
    secondaryColor: string
    tertiaryColor: string
    lineColor: string
    mainBkg: string
    nodeBorder: string
    clusterBkg: string
    clusterBorder: string
    edgeLabelBackground: string
    fontFamily: string
  }
}

export function getMermaidConfig(isDark: boolean): MermaidThemeConfig {
  return {
    startOnLoad: true,
    theme: 'base',
    darkMode: isDark,
    themeCSS: `
      .node rect,
      .node circle,
      .node ellipse,
      .node polygon,
      .node path {
        fill: var(--bc-mermaid-node-fill) !important;
        stroke: var(--bc-mermaid-node-border) !important;
      }

      .cluster rect {
        fill: var(--bc-mermaid-cluster-fill) !important;
        stroke: var(--bc-rule-strong) !important;
      }

      .label,
      .label text,
      .cluster text,
      .nodeLabel,
      .edgeLabel,
      .edgeLabel p {
        fill: var(--bc-mermaid-label) !important;
        color: var(--bc-mermaid-label) !important;
      }

      .edgeLabel rect,
      .labelBkg {
        fill: var(--bc-paper-pane) !important;
        opacity: 1 !important;
      }

      .flowchart-link,
      .edgePath .path,
      marker path {
        stroke: var(--bc-mermaid-edge) !important;
        fill: var(--bc-mermaid-edge) !important;
      }
    `,
    themeVariables: {
      primaryColor: 'var(--bc-mermaid-node-fill)',
      primaryTextColor: 'var(--bc-mermaid-label)',
      primaryBorderColor: 'var(--bc-mermaid-node-border)',
      secondaryColor: 'var(--bc-figure-pane)',
      tertiaryColor: 'var(--bc-paper-elevated)',
      lineColor: 'var(--bc-mermaid-edge)',
      mainBkg: 'transparent',
      nodeBorder: 'var(--bc-mermaid-node-border)',
      clusterBkg: 'var(--bc-mermaid-cluster-fill)',
      clusterBorder: 'var(--bc-rule-strong)',
      edgeLabelBackground: 'var(--bc-paper-pane)',
      fontFamily: 'Inter, IBM Plex Sans, system-ui, sans-serif',
    },
  }
}
