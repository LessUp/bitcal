import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'
import {
  compatPageExcludes,
  legacyAcademySourceExcludes,
  llmsDerivedPageExcludes,
} from '../.vitepress/page-exclusions.js'

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
const routeCompatExpectations = {
  'en/getting-started/migration.md': {
    targetRoute: '/en/getting-started/migration',
    canonicalRoute: '/en/guide/migration-posture',
  },
  'zh/getting-started/migration.md': {
    targetRoute: '/zh/getting-started/migration',
    canonicalRoute: '/zh/guide/migration-posture',
  },
  'en/api/overview.md': {
    targetRoute: '/en/api/overview',
    canonicalRoute: '/en/reference/index',
  },
  'zh/api/overview.md': {
    targetRoute: '/zh/api/overview',
    canonicalRoute: '/zh/reference/index',
  },
}

const config = read('.vitepress/config.ts')
const readme = read('README.md')
const docsIndex = read('index.md')
const changeDir = resolveChangeDir(activeChangeName)
const proposal = changeDir ? readOptional('proposal.md', changeDir, 'proposal') : null
const design = changeDir ? readOptional('design.md', changeDir, 'design') : null
const tasks = changeDir ? readOptional('tasks.md', changeDir, 'tasks') : null
const spec = changeDir ? readOptional('specs/project-documentation-site/spec.md', changeDir, 'spec') : null
const navTextExpectations = {
  zh: ['导读', '白皮书', '性能', '参考', '研究', '状态'],
  en: ['Guide', 'Whitepaper', 'Performance', 'Reference', 'Research', 'Status'],
}
const compatibilityPageLabels = {
  'en/academy/index.md': 'en academy compatibility page',
  'zh/academy/index.md': 'zh academy compatibility page',
  'en/project-status/index.md': 'en project-status compatibility page',
  'zh/project-status/index.md': 'zh project-status compatibility page',
  'en/getting-started/migration.md': 'en guide migration compatibility page',
  'zh/getting-started/migration.md': 'zh guide migration compatibility page',
  'en/api/overview.md': 'en reference compatibility page',
  'zh/api/overview.md': 'zh reference compatibility page',
  'en/whitepaper/performance.md': 'en whitepaper performance compatibility page',
  'zh/whitepaper/performance.md': 'zh whitepaper performance compatibility page',
}
const compatibilityPages = Object.fromEntries(
  compatPageExcludes.map((relativePath) => [
    relativePath,
    readOptional(relativePath, docsDir, compatibilityPageLabels[relativePath] ?? `compatibility page: ${relativePath}`),
  ]),
)

const srcExcludeBlock = readBracketedBlock(config, 'srcExclude: [', '[', ']')
const llmsIgnoreFilesBlock = readBracketedBlock(config, 'ignoreFiles: [', '[', ']')
const pageExclusionsImportMatch = config.match(/import\s*\{[\s\S]*?\}\s*from\s*['"]\.\/page-exclusions\.js['"]/)
const pageExclusionsImportBlock = pageExclusionsImportMatch?.[0] ?? null
const configWithoutExcludeWiring = [
  pageExclusionsImportBlock,
  srcExcludeBlock,
  llmsIgnoreFilesBlock,
].reduce((current, block) => block ? current.replace(block, '') : current, config)

const rewritesBlock = readBracketedBlock(config, 'rewrites: {', '{', '}')

expect(pageExclusionsImportMatch, 'VitePress config must import shared page excludes from ./page-exclusions.js')
expect(pageExclusionsImportBlock?.includes('legacyAcademySourceExcludes'), 'VitePress config import must include legacyAcademySourceExcludes')
expect(pageExclusionsImportBlock?.includes('compatPageExcludes'), 'VitePress config import must include compatPageExcludes')
expect(pageExclusionsImportBlock?.includes('llmsDerivedPageExcludes'), 'VitePress config import must include llmsDerivedPageExcludes')
expect(!config.includes('const legacyAcademySourceExcludes = ['), 'VitePress config must not inline legacy academy excludes')
expect(!config.includes('const llmsCompatibilityExcludes = ['), 'VitePress config must not inline combined llms compatibility excludes')
expect(srcExcludeBlock?.includes("'project-status.md'"), 'srcExclude must exclude project-status.md')
expect(srcExcludeBlock?.includes('...legacyAcademySourceExcludes'), 'srcExclude must spread legacy academy excludes')
expect(!srcExcludeBlock?.includes("'status.md'"), 'srcExclude still references nonexistent status.md')
expect(llmsIgnoreFilesBlock?.includes("'project-status.md'"), 'llmstxt ignoreFiles must exclude project-status.md')
expect(llmsIgnoreFilesBlock?.includes('...legacyAcademySourceExcludes'), 'llmstxt ignoreFiles must spread legacy academy excludes')
expect(llmsIgnoreFilesBlock?.includes('...compatPageExcludes'), 'llmstxt ignoreFiles must spread compatibility-page excludes')
expect(llmsIgnoreFilesBlock?.includes('...llmsDerivedPageExcludes'), 'llmstxt ignoreFiles must spread derived-page llms excludes')
expect(!llmsIgnoreFilesBlock?.includes('...llmsCompatibilityExcludes'), 'llmstxt ignoreFiles must not use the old combined compatibility exclude list')
expect(!llmsIgnoreFilesBlock?.includes("'status.md'"), 'llmstxt ignoreFiles still reference nonexistent status.md')
for (const legacyPath of legacyAcademySourceExcludes) {
  expect(llmsIgnoreFilesBlock?.includes(`...legacyAcademySourceExcludes`) || srcExcludeBlock?.includes(`...legacyAcademySourceExcludes`), `shared legacy academy exclude list must cover ${legacyPath}`)
}
for (const relativePath of compatPageExcludes) {
  expect(llmsIgnoreFilesBlock?.includes('...compatPageExcludes'), `llms compatibility-page exclude wiring must cover ${relativePath}`)
}
for (const relativePath of llmsDerivedPageExcludes) {
  expect(llmsIgnoreFilesBlock?.includes('...llmsDerivedPageExcludes'), `llms derived-page exclude wiring must cover ${relativePath}`)
}

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

expect(!configWithoutExcludeWiring.includes('/academy/'), 'Config still depends on academy routes')
expect(!configWithoutExcludeWiring.includes('Academy'), 'Config still exposes Academy as a primary IA section')
expect(!configWithoutExcludeWiring.includes('学院'), 'Config still exposes 学院 as a primary IA section')
expect(!configWithoutExcludeWiring.includes('/project-status/'), 'Config still depends on project-status routes')
expect(!configWithoutExcludeWiring.includes('/architecture/'), 'Config still depends on architecture routes')
expect(!rewritesBlock?.includes("'en/getting-started/:splat*'"), 'Compatibility route /en/getting-started/* must stay buildable at its own legacy path')
expect(!rewritesBlock?.includes("'zh/getting-started/:splat*'"), 'Compatibility route /zh/getting-started/* must stay buildable at its own legacy path')
expect(!rewritesBlock?.includes("'en/api/:splat*'"), 'Compatibility route /en/api/* must stay buildable at its own legacy path')
expect(!rewritesBlock?.includes("'zh/api/:splat*'"), 'Compatibility route /zh/api/* must stay buildable at its own legacy path')

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

for (const [relativePath, content] of Object.entries(compatibilityPages).filter(([relativePath]) => relativePath.includes('/academy/'))) {
  if (content === null) continue
  const isEnglish = relativePath.startsWith('en/')
  expect(content.includes(isEnglish ? 'Compatibility note' : '兼容说明'), `${relativePath} missing compatibility framing`)
  expect(!content.includes('<ReadingPathGrid'), `${relativePath} still renders as a primary landing page`)
  expect(!content.includes(isEnglish ? '## Learning Paths' : '## 学习路径'), `${relativePath} still promotes academy as a learning hub`)
  for (const section of ['guide', 'whitepaper', 'performance', 'reference', 'research', 'status']) {
    expect(content.includes(`/${isEnglish ? 'en' : 'zh'}/${section}/`), `${relativePath} missing compatibility link to ${section}`)
  }
}

for (const [relativePath, content] of Object.entries(compatibilityPages)) {
  if (content === null) continue
  expect(content.includes('search: false'), `${relativePath} must disable local search exposure`)
  expect(content.includes('sidebar: false'), `${relativePath} must disable sidebar exposure`)
  expect(content.includes('outline: false'), `${relativePath} must disable outline exposure`)
}

for (const [relativePath, expectation] of Object.entries(routeCompatExpectations)) {
  const content = compatibilityPages[relativePath]
  if (content === null) continue
  expect(exists(relativePath), `${relativePath} must keep an on-disk compatibility source file`)
  expect(content.includes(expectation.targetRoute), `${relativePath} must identify its retained legacy route ${expectation.targetRoute}`)
  expect(content.includes(expectation.canonicalRoute), `${relativePath} must hand readers off to ${expectation.canonicalRoute}`)
}

for (const relativePath of ['en/project-status/index.md', 'zh/project-status/index.md']) {
  const content = compatibilityPages[relativePath]
  if (content === null) continue
  expect(!content.includes('layout: home'), `${relativePath} must not render as a home landing page`)
  expect(!content.includes('router.go('), `${relativePath} must not use a client-side redirect shell`)
  expect(!content.includes('useRouter'), `${relativePath} must not depend on router redirect logic`)
  expect(
    content.includes(relativePath.startsWith('en/') ? 'Compatibility note' : '兼容说明'),
    `${relativePath} must explain that project-status is compatibility-only`,
  )
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
