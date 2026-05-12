#!/usr/bin/env node
/**
 * Sync CHANGELOG.md to docs/en/release-notes/changelog.md
 * Sync CHANGELOG.zh-CN.md to docs/zh/release-notes/changelog.md
 *
 * This script copies the content from the root CHANGELOG files to the docs site,
 * with only formatting changes (title format).
 *
 * Run from the docs directory: node scripts/sync-changelog.mjs
 */

import { readFileSync, writeFileSync, existsSync, mkdirSync } from "fs";
import { dirname, join } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const docsDir = join(__dirname, "..");
const rootDir = join(docsDir, "..");

// English changelog
const enSourcePath = join(rootDir, "CHANGELOG.md");
const enTargetPath = join(docsDir, "en/release-notes/changelog.md");

// Chinese changelog
const zhSourcePath = join(rootDir, "CHANGELOG.zh-CN.md");
const zhTargetPath = join(docsDir, "zh/release-notes/changelog.md");

const EN_HEADER = `# Changelog

This page documents the changes in each BitCal release.

`;

const ZH_HEADER = `# 更新日志

本页记录 BitCal 每个版本的变更。

`;

function syncChangelog(sourcePath, targetPath, header) {
  if (!existsSync(sourcePath)) {
    console.log(`Source not found: ${sourcePath}, skipping`);
    return;
  }

  // Ensure target directory exists
  const targetDir = dirname(targetPath);
  if (!existsSync(targetDir)) {
    mkdirSync(targetDir, { recursive: true });
  }

  // Read the source file
  let content = readFileSync(sourcePath, "utf-8");

  // Remove the HTML comment block at the top
  content = content.replace(/<!--[\s\S]*?-->\n*/g, "");

  // Remove the "# Changelog" or "# 更新日志" title (we'll add our own header)
  content = content.replace(/^# (Changelog|更新日志)\n+/i, "");

  // Convert title format: ## [0.69] - 2025-12-29 -> ## 0.69 (2025-12-29)
  content = content.replace(
    /^## \[([^\]]+)\] - (\d{4}-\d{1,2}-\d{1,2})/gm,
    "## $1 ($2)"
  );

  // Remove subsection headers like ### Added, ### Changed, ### Fixed
  content = content.replace(/^### (Added|Changed|Fixed|Improved|Tools|SDK|新增|变更|修复|改进|工具)\n+/gm, "");

  // Write the target file
  writeFileSync(targetPath, header + content.trim() + "\n");

  console.log(`Synced changelog to ${targetPath}`);
}

// Sync English changelog
syncChangelog(enSourcePath, enTargetPath, EN_HEADER);

// Sync Chinese changelog
syncChangelog(zhSourcePath, zhTargetPath, ZH_HEADER);
