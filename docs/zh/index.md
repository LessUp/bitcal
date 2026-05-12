---
layout: home
---

<div class="home-header">
  <div class="home-header-left">
    <div class="home-logo">BC</div>
    <div>
      <span class="home-title">BitCal</span>
      <span class="home-subtitle">C++17 SIMD 位操作库</span>
    </div>
  </div>
  <div class="home-nav">
    <a href="./getting-started/installation">指南</a>
    <a href="https://github.com/LessUp/bitcal">GitHub</a>
    <a href="../en/">English</a>
  </div>
</div>

<div class="home-intro-row">
  <div class="home-intro">
    BitCal 是一个头文件式 C++17 位操作库，支持编译时 SIMD 分发。零构建步骤，零依赖，极致性能。
  </div>
  <div class="home-stats">
    <span><strong>头文件式</strong></span>
    <span><strong>C++17</strong></span>
    <span><strong>SIMD</strong></span>
  </div>
</div>

## 功能特性

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">⚡ 头文件式</div>
    <div class="feature-card-desc">
      单一头文件入口。无需构建系统。只需复制头文件并编译。
    </div>
    <div class="feature-tags">
      <a href="./getting-started/installation" class="feature-tag">安装指南</a>
      <a href="./getting-started/quickstart" class="feature-tag">快速开始</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🚀 SIMD 优化</div>
    <div class="feature-card-desc">
      自动分发到 x86-64 上的 SSE2/AVX/AVX2 和 ARM64 上的 NEON。
    </div>
    <div class="feature-tags">
      <a href="./architecture/simd-dispatch" class="feature-tag">SIMD 分发</a>
      <a href="./architecture/platform-support" class="feature-tag">平台支持</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🎯 固定位宽</div>
    <div class="feature-card-desc">
      bit64、bit128、bit256、bit512、bit1024，以及 64 位的自定义倍数。
    </div>
    <div class="feature-tags">
      <a href="./api/types" class="feature-tag">类型</a>
      <a href="./api/core-operations" class="feature-tag">操作</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🔧 丰富 API</div>
    <div class="feature-card-desc">
      位运算、移位、popcount、CLZ/CTZ、位反转、单比特操作。
    </div>
    <div class="feature-tags">
      <a href="./api/bit-counting" class="feature-tag">位计数</a>
      <a href="./api/bit-manipulation" class="feature-tag">位操作</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🌍 跨平台</div>
    <div class="feature-card-desc">
      Linux、Windows、macOS，支持 x86-64 和 ARM，需要 C++17 或更高版本。
    </div>
    <div class="feature-tags">
      <a href="./architecture/platform-support" class="feature-tag">平台支持</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🛡️ 类型安全</div>
    <div class="feature-card-desc">
      编译时位宽验证（Bits % 64 == 0），无运行时开销。
    </div>
    <div class="feature-tags">
      <a href="./api/types" class="feature-tag">类型系统</a>
    </div>
  </div>
</div>

<div class="quick-start">
  <div class="quick-start-title">快速开始</div>
  <div class="quick-start-content">
    <div class="command-block">
      <code>cp -r include/bitcal /your/project/include/</code>
    </div>
    然后引入并编译：<code>#include &lt;bitcal/bitcal.hpp&gt;</code>
  </div>
</div>