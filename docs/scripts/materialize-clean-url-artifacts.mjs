import fs from 'node:fs'
import path from 'node:path'
import process from 'node:process'
import { fileURLToPath } from 'node:url'

const isSkippableHtml = (absolutePath, outDir) => {
  const relativePath = path.relative(outDir, absolutePath)
  const segments = relativePath.split(path.sep)
  const basename = path.basename(relativePath)

  return segments.includes('assets') || basename === 'index.html' || basename === '404.html'
}

export function materializeCleanUrlArtifacts(outDir) {
  const htmlFiles = []

  const walk = (currentDir) => {
    for (const entry of fs.readdirSync(currentDir, { withFileTypes: true })) {
      if (entry.name === 'assets') continue

      const absolutePath = path.join(currentDir, entry.name)
      if (entry.isDirectory()) {
        walk(absolutePath)
        continue
      }

      if (entry.isFile() && entry.name.endsWith('.html')) {
        htmlFiles.push(absolutePath)
      }
    }
  }

  walk(outDir)

  let mirrorCount = 0
  for (const absolutePath of htmlFiles) {
    if (isSkippableHtml(absolutePath, outDir)) continue

    const basename = path.basename(absolutePath)
    const stem = basename.slice(0, -'.html'.length)
    const mirrorDir = path.join(path.dirname(absolutePath), stem)
    const mirrorPath = path.join(mirrorDir, 'index.html')

    fs.mkdirSync(mirrorDir, { recursive: true })
    fs.copyFileSync(absolutePath, mirrorPath)
    mirrorCount += 1
  }

  return mirrorCount
}

const modulePath = fileURLToPath(import.meta.url)
const invokedPath = process.argv[1] ? path.resolve(process.argv[1]) : ''

if (modulePath === invokedPath) {
  const outDirArg = process.argv[2]
  const outDir = outDirArg ? path.resolve(process.cwd(), outDirArg) : path.join(process.cwd(), '.vitepress', 'dist')

  if (!fs.existsSync(outDir)) {
    console.error(`materialize-clean-url-artifacts.mjs could not find build output: ${outDir}`)
    process.exit(1)
  }

  const mirrorCount = materializeCleanUrlArtifacts(outDir)
  console.log(`Materialized ${mirrorCount} clean URL artifact mirrors in ${outDir}`)
}
