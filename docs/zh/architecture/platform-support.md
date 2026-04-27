# 平台兼容性

## 操作系统

| OS | 状态 | 说明 |
|----|------|------|
| Linux | ✅ 主要目标 | GCC/Clang 完整支持 |
| Windows | ✅ 支持 | MSVC 2017+ |
| macOS | ✅ 支持 | Apple Clang |
| FreeBSD/OpenBSD | ✅ 支持 | GCC/Clang |

## 处理器架构

| 架构 | 指令集 | 状态 |
|------|--------|------|
| x86-64 | SSE2 / AVX / AVX2 / AVX-512 | ✅ 完整支持 |
| ARM64 (AArch64) | NEON | ✅ 完整支持 |
| ARM32 (ARMv7-A) | NEON | ✅ 支持 |
| 其他 | — | ⚠️ 标量实现 |

## 编译器

| 编译器 | 最低版本 | 说明 |
|--------|----------|------|
| GCC | 7+ | 推荐 9+ |
| Clang | 6+ | 推荐 10+ |
| MSVC | 2017 (19.14+) | v2.1 修复了 SSE2 宏检测 |
| Apple Clang | — | 随 Xcode 提供 |

## ARM 设备支持

- Raspberry Pi 3/4/5
- NVIDIA Jetson series
- Apple Silicon (M1/M2/M3/M4)
- AWS Graviton
- 移动设备处理器

## MSVC 注意事项

MSVC 在 x64 模式下保证 SSE2 可用，但不定义 `__SSE2__` 宏。BitCal v2.1 通过检测 `_M_X64` 解决了这个问题。

要启用 AVX2，需在编译时添加 `/arch:AVX2` 标志。CMake 构建系统会自动检测并添加。

## AVX-512 功能分级

| 功能 | 指令集 | 支持状态 |
|------|--------|----------|
| 基本位运算 | AVX-512F | ✅ 完整支持 |
| 512 位位移 | AVX-512F | ✅ 完整支持（v2.2 优化） |
| Popcount | AVX-512VPOPCNTDQ | ⚠️ 需要特定 CPU |

### VPOPCNTDQ 兼容性

如果 CPU 支持 AVX-512 但不支持 VPOPCNTDQ（如早期 Skylake-X），popcount 会回退到标量实现。支持 VPOPCNTDQ 的 CPU：
- Intel: Ice Lake 及更新
- AMD: Zen 4 及更新
