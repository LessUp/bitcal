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

if (!fs.existsSync(path.join(docsDir, 'package.json')) || !fs.existsSync(path.join(docsDir, '.vitepress', 'config.ts'))) {
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
  'en/academy/index.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/guide/',
    '/en/whitepaper/',
    '/en/performance/',
    '/en/reference/',
    '/en/research/',
    '/en/status/',
  ],
  'zh/academy/index.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/guide/',
    '/zh/whitepaper/',
    '/zh/performance/',
    '/zh/reference/',
    '/zh/research/',
    '/zh/status/',
  ],
  'en/project-status/index.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/status/',
    '/en/guide/',
  ],
  'zh/project-status/index.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/status/',
    '/zh/guide/',
  ],
  'en/getting-started/migration.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/getting-started/migration',
    '/en/guide/migration-posture',
  ],
  'zh/getting-started/migration.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/getting-started/migration',
    '/zh/guide/migration-posture',
  ],
  'en/getting-started/verification.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/getting-started/verification',
    '/en/guide/verification',
  ],
  'zh/getting-started/verification.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/getting-started/verification',
    '/zh/guide/verification',
  ],
  'en/getting-started/migration-posture.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/getting-started/migration-posture',
    '/en/guide/migration-posture',
  ],
  'zh/getting-started/migration-posture.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/getting-started/migration-posture',
    '/zh/guide/migration-posture',
  ],
  'en/api/overview.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/api/overview',
    '/en/reference/',
  ],
  'zh/api/overview.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/api/overview',
    '/zh/reference/',
  ],
  'en/api/types-and-views.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/api/types-and-views',
    '/en/reference/types-and-views',
  ],
  'zh/api/types-and-views.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/api/types-and-views',
    '/zh/reference/types-and-views',
  ],
  'en/api/algorithms.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/api/algorithms',
    '/en/reference/algorithms',
  ],
  'zh/api/algorithms.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/api/algorithms',
    '/zh/reference/algorithms',
  ],
  'en/architecture/index.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/architecture/',
    '/en/whitepaper/',
  ],
  'zh/architecture/index.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/architecture/',
    '/zh/whitepaper/',
  ],
  'en/architecture/vnext-whitepaper.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/architecture/vnext-whitepaper',
    '/en/whitepaper/',
  ],
  'zh/architecture/vnext-whitepaper.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/architecture/vnext-whitepaper',
    '/zh/whitepaper/',
  ],
  'en/architecture/platform-support.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/architecture/platform-support',
    '/en/status/',
  ],
  'zh/architecture/platform-support.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/architecture/platform-support',
    '/zh/status/',
  ],
  'en/architecture/performance-baseline.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/architecture/performance-baseline',
    '/en/performance/',
  ],
  'zh/architecture/performance-baseline.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/architecture/performance-baseline',
    '/zh/performance/',
  ],
  'en/performance/methodology.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/performance/methodology',
    '/en/performance/',
  ],
  'zh/performance/methodology.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/performance/methodology',
    '/zh/performance/',
  ],
  'en/whitepaper/performance.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'compatibility entry',
    '/en/performance/',
    '/en/whitepaper/',
  ],
  'zh/whitepaper/performance.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容入口',
    '/zh/performance/',
    '/zh/whitepaper/',
  ],
  'en/academy/overview.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/academy/overview',
    '/en/guide/',
  ],
  'zh/academy/overview.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/academy/overview',
    '/zh/guide/',
  ],
  'en/academy/why-bitcal.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/academy/why-bitcal',
    '/en/guide/',
  ],
  'zh/academy/why-bitcal.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/academy/why-bitcal',
    '/zh/guide/',
  ],
  'en/academy/bit-mental-model.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/academy/bit-mental-model',
    '/en/whitepaper/public-model',
  ],
  'zh/academy/bit-mental-model.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/academy/bit-mental-model',
    '/zh/whitepaper/public-model',
  ],
  'en/academy/simd-primer.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/academy/simd-primer',
    '/en/whitepaper/dispatch-and-kernels',
  ],
  'zh/academy/simd-primer.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/academy/simd-primer',
    '/zh/whitepaper/dispatch-and-kernels',
  ],
  'en/academy/terminology.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    'Compatibility note',
    '/en/academy/terminology',
    '/en/reference/',
  ],
  'zh/academy/terminology.md': [
    'search: false',
    'sidebar: false',
    'outline: false',
    '兼容说明',
    '/zh/academy/terminology',
    '/zh/reference/',
  ],
}

for (const [relativePath, requiredSnippets] of Object.entries(requiredPageContracts)) {
  const content = read(relativePath)
  for (const snippet of requiredSnippets) {
    expect(content.includes(snippet), `${relativePath} missing required content snippet: ${snippet}`)
  }
}

const whitepaperEn = read('en/whitepaper/index.md')
expect(whitepaperEn.includes('/en/performance/'), 'en/whitepaper/index.md must hand readers off to the performance section')
const whitepaperZh = read('zh/whitepaper/index.md')
expect(whitepaperZh.includes('/zh/performance/'), 'zh/whitepaper/index.md must hand readers off to the performance section')

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

const performanceZh = read('zh/performance/index.md')
expect(performanceZh.includes('benchmark_compare'), 'zh/performance/index.md must retain benchmark_compare reproduction commands')
expect(performanceZh.includes('bitcal_benchmark'), 'zh/performance/index.md must distinguish benchmark_compare from bitcal_benchmark')

const whitepaperPerformanceEn = read('en/whitepaper/performance.md')
expect(whitepaperPerformanceEn.includes('compatibility entry'), 'en/whitepaper/performance.md must frame itself as a compatibility entry')
expect(whitepaperPerformanceEn.includes('/en/performance/'), 'en/whitepaper/performance.md must link to the primary performance section')
expect(whitepaperPerformanceEn.includes('/en/whitepaper/'), 'en/whitepaper/performance.md must link back to the whitepaper entry')

const whitepaperPerformanceZh = read('zh/whitepaper/performance.md')
expect(whitepaperPerformanceZh.includes('兼容入口'), 'zh/whitepaper/performance.md must frame itself as a compatibility entry')
expect(whitepaperPerformanceZh.includes('/zh/performance/'), 'zh/whitepaper/performance.md must link to the primary performance section')
expect(whitepaperPerformanceZh.includes('/zh/whitepaper/'), 'zh/whitepaper/performance.md must link back to the whitepaper entry')

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
