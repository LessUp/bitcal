import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import FigureFrame from './components/FigureFrame.vue'
import ReadingPathGrid from './components/ReadingPathGrid.vue'
import './style.css'

const theme: Theme = {
  ...DefaultTheme,
  enhanceApp(ctx) {
    DefaultTheme.enhanceApp?.(ctx)
    ctx.app.component('FigureFrame', FigureFrame)
    ctx.app.component('ReadingPathGrid', ReadingPathGrid)
  },
}

export default theme
