import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'

const docsDir = process.cwd()
const distDir = path.join(docsDir, '.vitepress', 'dist')

if (!fs.existsSync(path.join(docsDir, 'package.json')) || !fs.existsSync(distDir)) {
  console.error('check-clean-url-artifacts.mjs must be run from docs/ after a build. Run: cd docs && npm run build')
  process.exit(1)
}

const shouldSkip = (absolutePath) => {
  const relativeSegments = path.relative(distDir, absolutePath).split(path.sep)
  return relativeSegments.includes('assets')
}

const htmlFiles = []
const errors = []

const walk = (currentDir) => {
  for (const entry of fs.readdirSync(currentDir, { withFileTypes: true })) {
    if (entry.name === 'assets') continue

    const absolutePath = path.join(currentDir, entry.name)
    if (entry.isDirectory()) {
      walk(absolutePath)
      continue
    }

    if (entry.isFile() && entry.name.endsWith('.html')) {
      if (shouldSkip(absolutePath)) continue
      htmlFiles.push(absolutePath)
    }
  }
}

walk(distDir)

for (const absolutePath of htmlFiles) {
  const relativePath = path.relative(distDir, absolutePath)
  const basename = path.basename(relativePath)

  if (basename === 'index.html' || basename === '404.html') continue

  const stem = basename.slice(0, -'.html'.length)
  const mirrorPath = path.join(path.dirname(absolutePath), stem, 'index.html')
  if (!fs.existsSync(mirrorPath)) {
    errors.push(`Missing clean URL mirror: ${path.relative(distDir, mirrorPath)} (from ${relativePath})`)
    continue
  }

  const source = fs.readFileSync(absolutePath)
  const mirror = fs.readFileSync(mirrorPath)
  if (!source.equals(mirror)) {
    errors.push(`Mirror content drift: ${path.relative(distDir, mirrorPath)} differs from ${relativePath}`)
  }
}

for (const forbiddenPath of ['assets/index.html', '404/index.html', 'index/index.html']) {
  if (fs.existsSync(path.join(distDir, forbiddenPath))) {
    errors.push(`Unexpected mirror generated: ${forbiddenPath}`)
  }
}

if (errors.length > 0) {
  console.error('Clean URL artifact validation failed:')
  for (const error of errors) {
    console.error(`- ${error}`)
  }
  process.exit(1)
}

console.log(`Clean URL artifact validation passed for ${htmlFiles.length} HTML files.`)
