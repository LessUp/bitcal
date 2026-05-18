import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'

const docsDir = process.cwd()
const themeDir = path.join(docsDir, '.vitepress', 'theme')

const read = (...segments) => fs.readFileSync(path.join(themeDir, ...segments), 'utf8')
const readDocs = (...segments) => fs.readFileSync(path.join(docsDir, ...segments), 'utf8')
const exists = (...segments) => fs.existsSync(path.join(themeDir, ...segments))

const failMessages = []
const expect = (condition, message) => {
  if (!condition) failMessages.push(message)
}

if (!fs.existsSync(path.join(themeDir, 'index.ts'))) {
  console.error('check-theme-contract.mjs must be run from docs/. Run: cd docs && npm run check:theme-contract')
  process.exit(1)
}

const indexTs = read('index.ts')
const styleCss = read('style.css')
const variablesCss = read('styles', 'variables.css')
const mermaidThemeTs = read(path.join('..', 'plugins'), 'mermaid-theme.ts')
const packageJson = readDocs('package.json')

const requiredComponents = [
  'BitcalHero',
  'ReadingPathGrid',
  'EvidenceStrip',
  'FigureFrame',
  'CitationList',
]

for (const component of requiredComponents) {
  expect(indexTs.includes(`app.component('${component}'`), `Theme component is not registered: ${component}`)
}

const requiredStyleImports = [
  "@import './styles/variables.css';",
  "@import './styles/base.css';",
  "@import './styles/prose.css';",
  "@import './styles/components.css';",
  "@import './styles/figures.css';",
]

for (const statement of requiredStyleImports) {
  expect(styleCss.includes(statement), `Missing theme stylesheet import: ${statement}`)
}

const requiredTokens = [
  '--bc-paper-bg',
  '--bc-paper-pane',
  '--bc-paper-elevated',
  '--bc-ink-strong',
  '--bc-ink-soft',
  '--bc-rule-strong',
  '--bc-accent-cyan',
  '--bc-figure-bg',
  '--bc-figure-grid',
  '--bc-figure-ring',
  '--bc-mermaid-node-fill',
  '--bc-mermaid-cluster-fill',
  '--bc-code-bg',
]

for (const token of requiredTokens) {
  expect(variablesCss.includes(`:root`) && variablesCss.includes(token), `Missing light token: ${token}`)
  expect(variablesCss.includes('.dark') && variablesCss.includes(token), `Missing dark token: ${token}`)
}

expect(mermaidThemeTs.includes('themeCSS'), 'Mermaid config must inject themeCSS for figure-aware styling')
expect(mermaidThemeTs.includes('tertiaryColor'), 'Mermaid config must define tertiaryColor')
expect(mermaidThemeTs.includes('clusterBorder'), 'Mermaid config must define clusterBorder')
expect(indexTs.includes('getMermaidConfig'), 'Theme index must use getMermaidConfig')
expect(indexTs.includes("document.documentElement.style.setProperty('--bc-mermaid-mode'"), 'Theme index must set the active Mermaid mode token')

const svgComponents = ['ArchitectureDiagram.vue', 'ReadingModelDiagram.vue']
for (const file of svgComponents) {
  const content = read('components', file)
  expect(/data-(fill|stroke|node|slot|accent)=/.test(content), `${file} must use semantic SVG data-* hooks`)
}

const figureFrame = read('components', 'FigureFrame.vue')
expect(figureFrame.includes('data-figure-kind'), 'FigureFrame must expose a data-figure-kind hook')
expect(figureFrame.includes('data-figure-tone'), 'FigureFrame must expose a data-figure-tone hook')

const hero = read('components', 'BitcalHero.vue')
expect(hero.includes('data-hero-tone'), 'BitcalHero must expose a data-hero-tone hook')

const readingPath = read('components', 'ReadingPathGrid.vue')
expect(readingPath.includes('detail?: string'), 'ReadingPathGrid items must support detail copy')
expect(readingPath.includes('data-reading-tone'), 'ReadingPathGrid must expose a data-reading-tone hook')

const evidenceStrip = read('components', 'EvidenceStrip.vue')
expect(evidenceStrip.includes('data-evidence-tone'), 'EvidenceStrip must expose a data-evidence-tone hook')

const citationList = read('components', 'CitationList.vue')
expect(citationList.includes('data-citation-tone'), 'CitationList must expose a data-citation-tone hook')

expect(packageJson.includes('"check:theme-contract"'), 'docs/package.json must expose check:theme-contract')

if (failMessages.length > 0) {
  console.error('Theme contract validation failed:')
  for (const message of failMessages) {
    console.error(`- ${message}`)
  }
  process.exit(1)
}

console.log('Theme contract validation passed.')
