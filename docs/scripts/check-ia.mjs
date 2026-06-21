import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'

const docsDir = process.cwd()

const read = (relativePath, baseDir = docsDir) => fs.readFileSync(path.join(baseDir, relativePath), 'utf8')
const exists = (relativePath, baseDir = docsDir) => fs.existsSync(path.join(baseDir, relativePath))
const readOptional = (relativePath, baseDir) => {
  const targetPath = path.join(baseDir, relativePath)
  if (!fs.existsSync(targetPath)) {
    return null
  }
  return fs.readFileSync(targetPath, 'utf8')
}

const errors = []
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

const localeNavExpectations = {
  zh: [
    '/zh/guide/',
    '/zh/whitepaper/',
    '/zh/performance/',
    '/zh/reference/',
    '/zh/research/',
    '/zh/status/',
  ],
  en: [
    '/en/guide/',
    '/en/whitepaper/',
    '/en/performance/',
    '/en/reference/',
    '/en/research/',
    '/en/status/',
  ],
}

const navTextExpectations = {
  zh: ['导读', '白皮书', '性能', '参考', '研究', '状态'],
  en: ['Guide', 'Whitepaper', 'Performance', 'Reference', 'Research', 'Status'],
}

const retainedSectionLandingPaths = [
  'en/guide/index.md',
  'zh/guide/index.md',
  'en/whitepaper/index.md',
  'zh/whitepaper/index.md',
  'en/performance/index.md',
  'zh/performance/index.md',
  'en/reference/index.md',
  'zh/reference/index.md',
  'en/research/index.md',
  'zh/research/index.md',
  'en/status/index.md',
  'zh/status/index.md',
]

const legacyPathsToRemove = [
  'project-status.md',
  'en/academy',
  'zh/academy',
  'en/api',
  'zh/api',
  'en/architecture',
  'zh/architecture',
  'en/getting-started',
  'zh/getting-started',
  'en/project-status',
  'zh/project-status',
  'en/release-notes',
  'zh/release-notes',
  'en/performance/methodology.md',
  'zh/performance/methodology.md',
  'en/whitepaper/performance.md',
  'zh/whitepaper/performance.md',
  'design',
  '~',
]

const filesToCheckForOldPaths = [
  'index.md',
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
]

const config = read('.vitepress/config.ts')
const readme = read('README.md')
const docsIndex = read('index.md')
const srcExcludeBlock = readBracketedBlock(config, 'srcExclude: [', '[', ']')
const llmsIgnoreFilesBlock = readBracketedBlock(config, 'ignoreFiles: [', '[', ']')

expect(config.includes('cleanUrls: true'), 'VitePress config must enable cleanUrls for GitHub Pages-safe directory routes')
expect(!config.includes('page-exclusions'), 'VitePress config must not import legacy page-exclusion bookkeeping after the aggressive prune')
expect(!config.includes('sync-changelog'), 'VitePress config must not depend on changelog sync helpers after the aggressive prune')
expect(!srcExcludeBlock?.includes("'project-status.md'"), 'srcExclude must not keep excluding deleted project-status.md')
expect(!llmsIgnoreFilesBlock?.includes("'project-status.md'"), 'llmstxt ignoreFiles must not keep excluding deleted project-status.md')

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

for (const relativePath of retainedSectionLandingPaths) {
  expect(exists(relativePath), `Missing retained section landing: ${relativePath}`)
}

for (const relativePath of legacyPathsToRemove) {
  expect(!exists(relativePath), `Legacy docs path must be removed: ${relativePath}`)
}

expect(readme.includes('根 `CHANGELOG.md` / `CHANGELOG.zh-CN.md`：版本与变更记录的唯一对外入口'), 'docs/README.md must keep root changelog files as the only changelog entry')
expect(readme.includes('compatibility-only 页面与旧路由应直接删除'), 'docs/README.md must state that compatibility-only pages and old routes should be deleted')
expect(readme.includes('`docs/` 只承载主 IA 的 Pages 内容源'), 'docs/README.md must state that docs/ serves only the primary IA')
expect(!readme.includes('旧的 `project-status` 仅允许作为兼容 alias'), 'docs/README.md must no longer frame project-status as a retained alias')
expect(!readme.includes('academy、旧 architecture、`project-status` 只允许作为兼容入口'), 'docs/README.md must no longer keep compatibility-only sections as retained entries')
expect(!readme.includes('changelog 在 `docs/` 下继续重复镜像'), 'docs/README.md must remove wording that tolerates docs changelog mirrors')

expect(!docsIndex.includes('Legacy academy paths remain compatibility-only aliases'), 'docs/index.md must not mention retained legacy academy aliases')
expect(docsIndex.includes('maintained primary IA') || docsIndex.includes('主 IA'), 'docs/index.md must describe the retained primary IA')

for (const relativePath of filesToCheckForOldPaths) {
  const content = readOptional(relativePath, docsDir)
  if (content === null) continue
  expect(!content.includes('/academy/'), `${relativePath} still links to academy paths`)
  expect(!content.includes('/project-status/'), `${relativePath} still links to project-status paths`)
  expect(!content.includes('/architecture/'), `${relativePath} still links to architecture paths`)
  expect(!content.includes('/release-notes/'), `${relativePath} still links to release-notes paths`)
  expect(!content.includes('/getting-started/'), `${relativePath} still links to getting-started paths`)
  expect(!content.includes('/api/'), `${relativePath} still links to api compatibility paths`)
}

if (errors.length > 0) {
  console.error('IA validation failed:')
  for (const error of errors) {
    console.error(`- ${error}`)
  }
  process.exit(1)
}

console.log('IA validation passed.')
