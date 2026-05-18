import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'

const docsDir = process.cwd()
const repoRoot = path.resolve(docsDir, '..')

const read = (relativePath, baseDir = docsDir) => fs.readFileSync(path.join(baseDir, relativePath), 'utf8')
const exists = (relativePath, baseDir = docsDir) => fs.existsSync(path.join(baseDir, relativePath))

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

const openspecFiles = [
  'proposal.md',
  'design.md',
  'tasks.md',
  'specs/project-documentation-site/spec.md',
]

const errors = []
const expect = (condition, message) => {
  if (!condition) errors.push(message)
}

const config = read('.vitepress/config.ts')
const readme = read('README.md')
const docsIndex = read('index.md')
const proposal = read('openspec/changes/2026-05-15-pages-whitepaper-overhaul/proposal.md', repoRoot)
const design = read('openspec/changes/2026-05-15-pages-whitepaper-overhaul/design.md', repoRoot)
const tasks = read('openspec/changes/2026-05-15-pages-whitepaper-overhaul/tasks.md', repoRoot)
const spec = read('openspec/changes/2026-05-15-pages-whitepaper-overhaul/specs/project-documentation-site/spec.md', repoRoot)
const academyCompatPages = {
  en: read('en/academy/index.md'),
  zh: read('zh/academy/index.md'),
}
const navTextExpectations = {
  zh: ['导读', '白皮书', '性能', '参考', '研究', '状态'],
  en: ['Guide', 'Whitepaper', 'Performance', 'Reference', 'Research', 'Status'],
}

for (const [locale, links] of Object.entries(localeNavExpectations)) {
  let previousIndex = -1
  for (const link of links) {
    const quotedLink = `link: '${link}'`
    const index = config.indexOf(quotedLink)
    expect(index !== -1, `Missing ${locale} nav link: ${link}`)
    expect(index > previousIndex, `${locale} nav order drifted before ${link}`)
    previousIndex = index
  }
}

for (const [locale, labels] of Object.entries(navTextExpectations)) {
  let previousIndex = -1
  for (const label of labels) {
    const quotedLabel = `text: '${label}'`
    const index = config.indexOf(quotedLabel)
    expect(index !== -1, `Missing ${locale} nav label: ${label}`)
    expect(index > previousIndex, `${locale} nav label order drifted before ${label}`)
    previousIndex = index
  }
}

for (const locale of ['zh', 'en']) {
  for (const section of ['guide', 'whitepaper', 'performance', 'reference', 'research', 'status']) {
    expect(config.includes(`'/${locale}/${section}/': [`), `Missing ${locale} sidebar section /${locale}/${section}/`)
  }
}

expect(!config.includes('/academy/'), 'Config still depends on academy routes')
expect(!config.includes('Academy'), 'Config still exposes Academy as a primary IA section')
expect(!config.includes('学院'), 'Config still exposes 学院 as a primary IA section')
expect(!config.includes('project-status'), 'Config still depends on project-status routes')
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
  expect(content.includes(iaPhrase), `${name} missing canonical IA phrase: ${iaPhrase}`)
  expect(!content.includes('Academy'), `${name} still references Academy as a primary IA section`)
  expect(!content.includes('Project Status'), `${name} still references Project Status as a primary IA section`)
}

for (const [locale, content] of Object.entries(academyCompatPages)) {
  expect(content.includes(locale === 'en' ? 'Compatibility note' : '兼容说明'), `${locale}/academy/index.md missing compatibility framing`)
  expect(!content.includes('<ReadingPathGrid'), `${locale}/academy/index.md still renders as a primary landing page`)
  expect(!content.includes(locale === 'en' ? '## Learning Paths' : '## 学习路径'), `${locale}/academy/index.md still promotes academy as a learning hub`)
  for (const section of ['guide', 'whitepaper', 'performance', 'reference', 'research', 'status']) {
    expect(content.includes(`/${locale}/${section}/`), `${locale}/academy/index.md missing compatibility link to ${section}`)
  }
}

for (const relativePath of filesToCheckForOldPaths) {
  const content = read(relativePath)
  expect(!content.includes('/academy/'), `${relativePath} still links to academy paths`)
  expect(!content.includes('/project-status/'), `${relativePath} still links to project-status paths`)
  expect(!content.includes('/architecture/'), `${relativePath} still links to architecture paths`)
}

if (errors.length > 0) {
  console.error('IA validation failed:')
  for (const error of errors) {
    console.error(`- ${error}`)
  }
  process.exit(1)
}

console.log('IA validation passed.')
