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
    '/en/guide/index',
    '/en/whitepaper/index',
    '/en/performance/index',
    '/en/reference/index',
    '/en/research/index',
    '/en/status/index',
  ],
  'zh/index.md': [
    '## 白皮书阅读契约',
    '## 公开模型速览',
    '/zh/guide/index',
    '/zh/whitepaper/index',
    '/zh/performance/index',
    '/zh/reference/index',
    '/zh/research/index',
    '/zh/status/index',
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
  'en/reference/index.md': ['## Contract map', '## Reading reference correctly'],
  'zh/reference/index.md': ['## 契约地图', '## 如何正确阅读 Reference'],
  'en/reference/types-and-views.md': ['## Public type roles', '## Ownership and borrowing contract'],
  'zh/reference/types-and-views.md': ['## 公共类型角色', '## ownership 与 borrowing 契约'],
  'en/reference/algorithms.md': ['## Algorithm surface', '## Contract rules and non-goals'],
  'zh/reference/algorithms.md': ['## 算法表面', '## 契约规则与非目标'],
  'en/research/index.md': ['## Research map', '## Reference set', '## Related systems'],
  'zh/research/index.md': ['## 研究地图', '## 参考资料集', '## 相关系统'],
  'en/research/evolution-and-comparisons.md': ['## Citations', '## Related systems', '## Evolution notes', '## Design trade-offs'],
  'zh/research/evolution-and-comparisons.md': ['## 引用文献', '## 相关系统', '## 演进说明', '## 设计取舍'],
  'en/status/index.md': ['## Release posture', '## Support matrix', '## Documentation truth'],
  'zh/status/index.md': ['## 发布姿态', '## 支持矩阵', '## 文档真相'],
}

for (const [relativePath, requiredSnippets] of Object.entries(requiredPageContracts)) {
  const content = read(relativePath)
  for (const snippet of requiredSnippets) {
    expect(content.includes(snippet), `${relativePath} missing required content snippet: ${snippet}`)
  }
}

const whitepaperEn = read('en/whitepaper/index.md')
expect(whitepaperEn.includes('/en/performance/index'), 'en/whitepaper/index.md must hand readers off to the performance section')
const whitepaperZh = read('zh/whitepaper/index.md')
expect(whitepaperZh.includes('/zh/performance/index'), 'zh/whitepaper/index.md must hand readers off to the performance section')

if (errors.length > 0) {
  console.error('Content contract validation failed:')
  for (const error of errors) {
    console.error(`- ${error}`)
  }
  process.exit(1)
}

console.log('Content contract validation passed.')
