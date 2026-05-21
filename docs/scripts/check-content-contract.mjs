import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'

const docsDir = process.cwd()
const errors = []

const expect = (condition, message) => {
  if (!condition) errors.push(message)
}

const read = (relativePath) => {
  const absolutePath = path.join(docsDir, relativePath)
  expect(fs.existsSync(absolutePath), `Missing required page: ${relativePath}`)
  return fs.existsSync(absolutePath) ? fs.readFileSync(absolutePath, 'utf8') : ''
}

const exists = (relativePath) => fs.existsSync(path.join(docsDir, relativePath))

if (!exists('package.json') || !exists('.vitepress/config.ts')) {
  console.error('check-content-contract.mjs must be run from docs/. Run: cd docs && npm run check:content-contract')
  process.exit(1)
}

const requiredPageContracts = {
  'en/index.md': [
    '## Whitepaper reading contract',
    '## Public model at a glance',
    '/en/guide/',
    '/en/whitepaper/',
    '/en/performance/',
    '/en/reference/',
    '/en/research/',
    '/en/status/',
  ],
  'zh/index.md': [
    '## 白皮书阅读契约',
    '## 公开模型速览',
    '/zh/guide/',
    '/zh/whitepaper/',
    '/zh/performance/',
    '/zh/reference/',
    '/zh/research/',
    '/zh/status/',
  ],
  'en/guide/index.md': ['## Audience and scope', '## Reading chain', 'Guide → Whitepaper → Performance → Reference → Research → Status'],
  'zh/guide/index.md': ['## 读者与范围', '## 阅读链路', 'Guide → Whitepaper → Performance → Reference → Research → Status'],
  'en/whitepaper/index.md': ['## System architecture spine', '## Public model', '## Algorithm organization', '## Dispatch and support boundary'],
  'zh/whitepaper/index.md': ['## 系统架构主线', '## 公开模型', '## 算法组织', '## 分发与支持边界'],
  'en/whitepaper/public-model.md': ['## Owner / view / algorithm model', '## Stable include seam', '## Contract boundaries'],
  'zh/whitepaper/public-model.md': ['## owner / view / algorithm 模型', '## 稳定 include seam', '## 契约边界'],
  'en/whitepaper/algorithm-design.md': ['## Free algorithm organization', '## Algorithm families', '## Observable semantics first'],
  'zh/whitepaper/algorithm-design.md': ['## 自由算法组织', '## 算法族', '## 先定义可观察语义'],
  'en/whitepaper/dispatch-and-kernels.md': ['## Dispatch boundary', '## Kernel families', '## Support boundary'],
  'zh/whitepaper/dispatch-and-kernels.md': ['## 分发边界', '## 内核族', '## 支持边界'],
  'en/performance/index.md': ['## Baseline snapshot', '## Measurement methodology', '## Claim boundary'],
  'zh/performance/index.md': ['## 基线快照', '## 测量方法学', '## 宣称边界'],
  'en/reference/index.md': [
    '## Contract map',
    '## What Reference freezes',
    '<bitcal/bitcal.hpp>',
    'owner / view / algorithm split',
    'algorithm surface instead of internal kernel layout',
    '## Reading reference correctly',
  ],
  'zh/reference/index.md': [
    '## 契约地图',
    '## Reference 在这里冻结什么',
    '<bitcal/bitcal.hpp>',
    'owner / view / algorithm 拆分',
    '算法表面而不是内部 kernel 布局',
    '## 如何正确阅读 Reference',
  ],
  'en/reference/types-and-views.md': ['## Public type roles', '## Ownership and borrowing contract'],
  'zh/reference/types-and-views.md': ['## 公共类型角色', '## ownership 与 borrowing 契约'],
  'en/reference/algorithms.md': ['## Algorithm surface', '## Contract rules and non-goals'],
  'zh/reference/algorithms.md': ['## 算法表面', '## 契约规则与非目标'],
  'en/research/index.md': ['## Research map', '## Reference set', '## Related systems'],
  'zh/research/index.md': ['## 研究地图', '## 参考资料集', '## 相关系统'],
  'en/research/evolution-and-comparisons.md': [
    '## Citations',
    '## Related systems',
    '## Evolution notes',
    '## Design trade-offs',
    'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
    'https://www.agner.org/optimize/',
    'https://arxiv.org/abs/0901.0166',
    'https://github.com/kimwalisch/libpopcnt',
  ],
  'zh/research/evolution-and-comparisons.md': [
    '## 引用文献',
    '## 相关系统',
    '## 演进说明',
    '## 设计取舍',
    'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
    'https://www.agner.org/optimize/',
    'https://arxiv.org/abs/0901.0166',
    'https://github.com/kimwalisch/libpopcnt',
  ],
  'en/status/index.md': ['## Release posture', '## Support matrix', '## Documentation truth'],
  'zh/status/index.md': ['## 发布姿态', '## 支持矩阵', '## 文档真相'],
}

const deletedLegacyPaths = [
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
  'project-status.md',
]

for (const [relativePath, requiredSnippets] of Object.entries(requiredPageContracts)) {
  const content = read(relativePath)
  for (const snippet of requiredSnippets) {
    expect(content.includes(snippet), `${relativePath} missing required content snippet: ${snippet}`)
  }
}

for (const relativePath of deletedLegacyPaths) {
  expect(!exists(relativePath), `Legacy docs content must be deleted: ${relativePath}`)
}

const whitepaperEn = read('en/whitepaper/index.md')
expect(whitepaperEn.includes('/en/performance/'), 'en/whitepaper/index.md must hand readers off to the performance section')
expect(whitepaperEn.includes('ARM rows stay blank'), 'en/whitepaper/index.md must hand off ARM performance as intentionally blank')

const whitepaperZh = read('zh/whitepaper/index.md')
expect(whitepaperZh.includes('/zh/performance/'), 'zh/whitepaper/index.md must hand readers off to the performance section')
expect(whitepaperZh.includes('ARM 数据先留空'), 'zh/whitepaper/index.md must hand off ARM performance as intentionally blank')

const zhReference = read('zh/reference/index.md')
expect(zhReference.includes("title: '类型与视图'"), 'zh/reference/index.md must localize the Types and Views card title')
expect(zhReference.includes("title: '算法契约'"), 'zh/reference/index.md must localize the Algorithm Contract card title')

const verificationEn = read('en/guide/verification.md')
expect(verificationEn.includes('bitcal_benchmark'), 'en/guide/verification.md must retain the bitcal_benchmark verification path')
expect(verificationEn.includes('benchmark_compare'), 'en/guide/verification.md must explain how benchmark_compare differs from bitcal_benchmark')

const verificationZh = read('zh/guide/verification.md')
expect(verificationZh.includes('bitcal_benchmark'), 'zh/guide/verification.md must retain the bitcal_benchmark verification path')
expect(verificationZh.includes('benchmark_compare'), 'zh/guide/verification.md must explain how benchmark_compare differs from bitcal_benchmark')

const performanceEn = read('en/performance/index.md')
expect(performanceEn.includes('benchmark_compare'), 'en/performance/index.md must retain benchmark_compare reproduction commands')
expect(performanceEn.includes('bitcal_benchmark'), 'en/performance/index.md must distinguish benchmark_compare from bitcal_benchmark')
expect(performanceEn.includes('performanceBaseline'), 'en/performance/index.md must import performanceBaseline data')
expect(!performanceEn.includes("{ operation: 'and<256>'"), 'en/performance/index.md must not hardcode benchmark rows')
expect(performanceEn.includes('ARM rows stay blank'), 'en/performance/index.md must state that ARM rows stay blank until retained evidence exists')
expect(performanceEn.includes('median ns/op'), 'en/performance/index.md must publish the median statistic explicitly')

const performanceZh = read('zh/performance/index.md')
expect(performanceZh.includes('benchmark_compare'), 'zh/performance/index.md must retain benchmark_compare reproduction commands')
expect(performanceZh.includes('bitcal_benchmark'), 'zh/performance/index.md must distinguish benchmark_compare from bitcal_benchmark')
expect(performanceZh.includes('performanceBaseline'), 'zh/performance/index.md must import performanceBaseline data')
expect(!performanceZh.includes("{ operation: 'and<256>'"), 'zh/performance/index.md must not hardcode benchmark rows')
expect(performanceZh.includes('ARM 数据先留空'), 'zh/performance/index.md must state that ARM data stays blank until retained evidence exists')
expect(performanceZh.includes('median ns/op'), 'zh/performance/index.md must publish the median statistic explicitly')

const statusEn = read('en/status/index.md')
expect(statusEn.includes('performance rows stay blank'), 'en/status/index.md must state that ARM performance rows stay blank without retained evidence')

const statusZh = read('zh/status/index.md')
expect(statusZh.includes('性能数据先留空'), 'zh/status/index.md must state that ARM performance data stays blank without retained evidence')

const canonicalLegacyClaimGuards = {
  'en/guide/index.md': ['bitarray', 'C++17'],
  'zh/guide/index.md': ['bitarray', 'C++17'],
  'en/whitepaper/public-model.md': ['bitarray', 'C++17'],
  'zh/whitepaper/public-model.md': ['bitarray', 'C++17'],
  'en/reference/types-and-views.md': ['bitarray', 'C++17'],
  'zh/reference/types-and-views.md': ['bitarray', 'C++17'],
  'en/status/index.md': ['bitarray', 'C++17'],
  'zh/status/index.md': ['bitarray', 'C++17'],
  'en/guide/migration-posture.md': ['bitarray', 'C++17'],
  'zh/guide/migration-posture.md': ['bitarray', 'C++17'],
  'en/research/evolution-and-comparisons.md': ['bitarray', 'C++17'],
  'zh/research/evolution-and-comparisons.md': ['bitarray', 'C++17'],
}

for (const [relativePath, forbiddenSnippets] of Object.entries(canonicalLegacyClaimGuards)) {
  const content = read(relativePath)
  for (const snippet of forbiddenSnippets) {
    expect(!content.includes(snippet), `${relativePath} must not keep expanding legacy claim wording: ${snippet}`)
  }
}

if (errors.length > 0) {
  console.error('Content contract validation failed:')
  for (const error of errors) {
    console.error(`- ${error}`)
  }
  process.exit(1)
}

console.log('Content contract validation passed.')
