---
layout: home
---

<div class="home-header">
  <div class="home-header-left">
    <div class="home-logo">BC</div>
    <div>
      <span class="home-title">BitCal</span>
      <span class="home-subtitle">C++23 vNext Bit Library</span>
    </div>
  </div>
  <div class="home-nav">
    <a href="./getting-started/installation">Guides</a>
    <a href="https://github.com/LessUp/bitcal">GitHub</a>
    <a href="../zh/">中文</a>
  </div>
</div>

<div class="home-intro-row">
  <div class="home-intro">
    BitCal is a header-only C++23 bit manipulation library in active vNext redesign. The current public model is built around bit blocks, bit views, and free algorithms with an x86-64-first performance posture.
  </div>
  <div class="home-stats">
    <span><strong>Header-only</strong></span>
    <span><strong>C++23</strong></span>
    <span><strong>x86-64 First</strong></span>
  </div>
</div>

## vNext references

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">📘 Technical Whitepaper</div>
    <div class="feature-card-desc">
      Architecture, public model, backend layering, correctness posture, and reproducibility guidance for BitCal vNext.
    </div>
    <div class="feature-tags">
      <a href="./architecture/vnext-whitepaper" class="feature-tag">Whitepaper</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">📈 Performance Baseline</div>
    <div class="feature-card-desc">
      Current reproducible benchmark baseline for the vNext public model, including methodology notes and sample numbers.
    </div>
    <div class="feature-tags">
      <a href="./architecture/performance-baseline" class="feature-tag">Baseline Metrics</a>
    </div>
  </div>
</div>

## Features

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">⚡ Header-Only</div>
    <div class="feature-card-desc">
      Single include seam. No build system needed. Just copy headers and compile.
    </div>
    <div class="feature-tags">
      <a href="./getting-started/installation" class="feature-tag">Installation</a>
      <a href="./getting-started/quickstart" class="feature-tag">Quick Start</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🚀 SIMD Optimized</div>
    <div class="feature-card-desc">
      Automatic dispatch to SSE2/AVX/AVX2 on x86-64 and NEON on ARM64.
    </div>
    <div class="feature-tags">
      <a href="./architecture/simd-dispatch" class="feature-tag">SIMD Dispatch</a>
      <a href="./architecture/platform-support" class="feature-tag">Platforms</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🎯 Fixed Widths</div>
    <div class="feature-card-desc">
      bit64, bit128, bit256, bit512, bit1024, plus custom multiples of 64 bits.
    </div>
    <div class="feature-tags">
      <a href="./api/types" class="feature-tag">Types</a>
      <a href="./api/core-operations" class="feature-tag">Operations</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🔧 Rich API</div>
    <div class="feature-card-desc">
      Bitwise ops, shifts, popcount, CLZ/CTZ, bit reversal, single-bit manipulation.
    </div>
    <div class="feature-tags">
      <a href="./api/bit-counting" class="feature-tag">Bit Counting</a>
      <a href="./api/bit-manipulation" class="feature-tag">Manipulation</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🌍 Cross-Platform</div>
    <div class="feature-card-desc">
      Linux, Windows, macOS on x86-64 and ARM with C++17 or later.
    </div>
    <div class="feature-tags">
      <a href="./architecture/platform-support" class="feature-tag">Platform Support</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🛡️ Type-Safe</div>
    <div class="feature-card-desc">
      Compile-time bit-width validation (Bits % 64 == 0) with no runtime overhead.
    </div>
    <div class="feature-tags">
      <a href="./api/types" class="feature-tag">Type System</a>
    </div>
  </div>
</div>

<div class="quick-start">
  <div class="quick-start-title">Quick Start</div>
  <div class="quick-start-content">
    <div class="command-block">
      <code>cp -r include/bitcal /your/project/include/</code>
    </div>
    Then include and compile: <code>#include &lt;bitcal/bitcal.hpp&gt;</code>
  </div>
</div>
