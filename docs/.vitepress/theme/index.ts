import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './style.css'

import BitcalHero from './components/BitcalHero.vue'
import CitationList from './components/CitationList.vue'
import EvidenceStrip from './components/EvidenceStrip.vue'
import FigureFrame from './components/FigureFrame.vue'
import ReadingPathGrid from './components/ReadingPathGrid.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('BitcalHero', BitcalHero)
    app.component('CitationList', CitationList)
    app.component('EvidenceStrip', EvidenceStrip)
    app.component('FigureFrame', FigureFrame)
    app.component('ReadingPathGrid', ReadingPathGrid)
  },
} satisfies Theme
