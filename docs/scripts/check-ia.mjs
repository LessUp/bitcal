import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'

const docsDir = process.cwd()
const repoRoot = path.resolve(docsDir, '..')
const activeChangeName = '2026-05-15-pages-whitepaper-overhaul'

const read = (relativePath, baseDir = docsDir) => fs.readFileSync(path.join(baseDir, relativePath), 'utf8')
const exists = (relativePath, baseDir = docsDir) => fs.existsSync(path.join(baseDir, relativePath))

const errors = []
const warnings = []
const expect = (condition, message) => {
  if (!condition) errors.push(message)
}

const fail = (message) => {
  console.error(message)
  process.exit(1)
}

if (!exists('.vitepress/config.ts') || !exists('README.md')) {
  fail('check-ia.mjs must be run from the docs/ directory. Run: cd docs && npm run check:ia')
}

const readBracketedBlock = (content, startMarker, openChar, closeChar) => {
  const start = content.indexOf(startMarker)
  if (start === -1) return null

  let depth = 0
  let started = false

  for (let index = start + startMarker.length - 1; index < content.length; index += 1) {
    const char = content[index]
    if (char === openChar) {
      depth += 1
      started = true
    } else if (char === closeChar) {
      depth -= 1
      if (started && depth === 0) {
        return content.slice(start, index + 1)
      }
    }
  }

  return null
}

const extractLocaleNav = (configContent, locale) => {
  const localeBlock = readBracketedBlock(configContent, `${locale}: {`, '{', '}')
  if (!localeBlock) {
    errors.push(`Missing ${locale} locale block in VitePress config`)
    return { links: [], labels: [] }
  }

  const navBlock = readBracketedBlock(localeBlock, 'nav: [', '[', ']')
  if (!navBlock) {
    errors.push(`Missing ${locale} nav block in VitePress config`)
    return { links: [], labels: [] }
  }

  return {
    links: [...navBlock.matchAll(/link:\s*'([^']+)'/g)].map((match) => match[1]),
    labels: [...navBlock.matchAll(/text:\s*'([^']+)'/g)].map((match) => match[1]),
  }
}

const resolveChangeDir = (changeName) => {
  const candidates = [
    path.join(repoRoot, 'openspec/changes', changeName),
    path.join(repoRoot, 'openspec/changes/archive', changeName),
  ]

  return candidates.find((candidate) => fs.existsSync(candidate)) ?? null
}

const readOptional = (relativePath, baseDir, label) => {
  const targetPath = path.join(baseDir, relativePath)
  if (!fs.existsSync(targetPath)) {
    warnings.push(`Skipped ${label}: missing ${path.relative(repoRoot, targetPath)}`)
    return null
  }

  return fs.readFileSync(targetPath, 'utf8')
}

const requiredSections = ['Guide', 'Whitepaper', 'Performance', 'Reference', 'Research', 'Status']
const localeNavExpectations = {
  zh: [
    '/zh/guide/index',
    '/zh/whitepaper/index',
    '/zh/performance/index',
    '/zh/reference/index',
    '/zh/research/index',
    '/zh/status/index',
  ],
  en: [
    '/en/guide/index',
    '/en/whitepaper/index',
    '/en/performance/index',
    '/en/reference/index',
    '/en/research/index',
    '/en/status/index',
  ],
}

const filesToCheckForOldPaths = [
  'index.md',
  'project-status.md',
  'en/index.md',
  'zh/index.md',
  'en/status/index.md',
  'zh/status/index.md',
  'en/guide/index.md',
  'zh/guide/index.md',
  'en/research/index.md',
  'zh/research/index.md',
  'en/reference/index.md',
  'zh/reference/index.md',
  'en/getting-started/migration.md',
  'zh/getting-started/migration.md',
  'en/api/overview.md',
  'zh/api/overview.md',
]

const config = read('.vitepress/config.ts')
const readme = read('README.md')
const docsIndex = read('index.md')
const changeDir = resolveChangeDir(activeChangeName)
const proposal = changeDir ? readOptional('proposal.md', changeDir, 'proposal') : null
const design = changeDir ? readOptional('design.md', changeDir, 'design') : null
const tasks = changeDir ? readOptional('tasks.md', changeDir, 'tasks') : null
const spec = changeDir ? readOptional('specs/project-documentation-site/spec.md', changeDir, 'spec') : null
const academyCompatPages = {
  en: readOptional('en/academy/index.md', docsDir, 'en academy compatibility page'),
  zh: readOptional('zh/academy/index.md', docsDir, 'zh academy compatibility page'),
}
const navTextExpectations = {
  zh: ['导读', '白皮书', '性能', '参考', '研究', '状态'],
  en: ['Guide', 'Whitepaper', 'Performance', 'Reference', 'Research', 'Status'],
}

const srcExcludeBlock = readBracketedBlock(config, 'srcExclude: [', '[', ']')
const llmsIgnoreFilesBlock = readBracketedBlock(config, 'ignoreFiles: [', '[', ']')

expect(srcExcludeBlock?.includes("'project-status.md'"), 'srcExclude must exclude project-status.md')
expect(!srcExcludeBlock?.includes("'status.md'"), 'srcExclude still references nonexistent status.md')
expect(llmsIgnoreFilesBlock?.includes("'project-status.md'"), 'llmstxt ignoreFiles must exclude project-status.md')
expect(!llmsIgnoreFilesBlock?.includes("'status.md'"), 'llmstxt ignoreFiles still reference nonexistent status.md')

for (const [locale, expectedLinks] of Object.entries(localeNavExpectations)) {
  const nav = extractLocaleNav(config, locale)
  expect(
    JSON.stringify(nav.links) === JSON.stringify(expectedLinks),
    `${locale} nav links drifted: expected ${expectedLinks.join(' -> ')}, got ${nav.links.join(' -> ')}`,
  )
  expect(
    JSON.stringify(nav.labels) === JSON.stringify(navTextExpectations[locale]),
    `${locale} nav labels drifted: expected ${navTextExpectations[locale].join(' -> ')}, got ${nav.labels.join(' -> ')}`,
  )
}

for (const locale of ['zh', 'en']) {
  for (const section of ['guide', 'whitepaper', 'performance', 'reference', 'research', 'status']) {
    expect(config.includes(`'/${locale}/${section}/': [`), `Missing ${locale} sidebar section /${locale}/${section}/`)
  }
}

expect(!config.includes('/academy/'), 'Config still depends on academy routes')
expect(!config.includes('Academy'), 'Config still exposes Academy as a primary IA section')
expect(!config.includes('学院'), 'Config still exposes 学院 as a primary IA section')
expect(!config.includes('/project-status/'), 'Config still depends on project-status routes')
expect(!config.includes('/architecture/'), 'Config still depends on architecture routes')

for (const locale of ['en', 'zh']) {
  for (const section of ['guide', 'whitepaper', 'performance', 'reference', 'research', 'status']) {
    expect(exists(`${locale}/${section}/index.md`), `Missing localized section landing: ${locale}/${section}/index.md`)
  }
}

expect(readme.includes('### 3.3 Guide'), 'docs/README.md missing Guide IA section')
expect(readme.includes('### 3.4 Whitepaper'), 'docs/README.md missing Whitepaper IA section')
expect(readme.includes('### 3.5 Performance'), 'docs/README.md missing Performance IA section')
expect(readme.includes('### 3.6 Reference'), 'docs/README.md missing Reference IA section')
expect(readme.includes('### 3.7 Research'), 'docs/README.md missing Research IA section')
expect(readme.includes('### 3.8 Status'), 'docs/README.md missing Status IA section')
expect(!readme.includes('Project Status'), 'docs/README.md still names the primary section Project Status')

expect(!docsIndex.includes('router.go('), 'docs/index.md still behaves like a pure redirect shell')
expect(!docsIndex.includes('useRouter'), 'docs/index.md still imports router-based redirect logic')
expect(!docsIndex.includes('navigator.language'), 'docs/index.md still auto-redirects instead of acting as docs landing')
expect(docsIndex.includes('/zh/') && docsIndex.includes('/en/'), 'docs/index.md missing bilingual language entry links')
for (const locale of ['en', 'zh']) {
  for (const section of ['guide', 'whitepaper', 'performance', 'reference', 'research', 'status']) {
    expect(docsIndex.includes(`/${locale}/${section}/index`), `docs/index.md missing ${locale} landing link for ${section}`)
  }
}
expect(
  docsIndex.includes('Docs landing') || docsIndex.includes('文档总览') || docsIndex.includes('Documentation overview'),
  'docs/index.md missing docs landing overview copy',
)

const iaPhrase = requiredSections.join(' / ')
for (const [name, content] of Object.entries({ proposal, design, tasks, spec })) {
  if (content === null) continue
  expect(content.includes(iaPhrase), `${name} missing canonical IA phrase: ${iaPhrase}`)
  expect(!content.includes('Academy'), `${name} still references Academy as a primary IA section`)
  expect(!content.includes('Project Status'), `${name} still references Project Status as a primary IA section`)
}

for (const [locale, content] of Object.entries(academyCompatPages)) {
  if (content === null) continue
  expect(content.includes(locale === 'en' ? 'Compatibility note' : '兼容说明'), `${locale}/academy/index.md missing compatibility framing`)
  expect(!content.includes('<ReadingPathGrid'), `${locale}/academy/index.md still renders as a primary landing page`)
  expect(!content.includes(locale === 'en' ? '## Learning Paths' : '## 学习路径'), `${locale}/academy/index.md still promotes academy as a learning hub`)
  for (const section of ['guide', 'whitepaper', 'performance', 'reference', 'research', 'status']) {
    expect(content.includes(`/${locale}/${section}/`), `${locale}/academy/index.md missing compatibility link to ${section}`)
  }
}

for (const relativePath of filesToCheckForOldPaths) {
  const content = readOptional(relativePath, docsDir, `old path scan: ${relativePath}`)
  if (content === null) continue
  expect(!content.includes('/academy/'), `${relativePath} still links to academy paths`)
  expect(!content.includes('/project-status/'), `${relativePath} still links to project-status paths`)
  expect(!content.includes('/architecture/'), `${relativePath} still links to architecture paths`)
}

if (!changeDir) {
  warnings.push(`Skipped OpenSpec content checks: active or archived change not found for ${activeChangeName}`)
}

if (warnings.length > 0) {
  for (const warning of warnings) {
    console.warn(`IA validation warning: ${warning}`)
  }
}

if (errors.length > 0) {
  console.error('IA validation failed:')
  for (const error of errors) {
    console.error(`- ${error}`)
  }
  process.exit(1)
}

console.log('IA validation passed.')
