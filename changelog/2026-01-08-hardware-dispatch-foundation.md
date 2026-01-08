# 2026-01-08

- 修复并补齐 CPU 特性检测相关头文件路径：新增 `src/Utils/CheckCpu.h` 作为兼容入口。
- 新增 `src/Base/Helper.h`，提供 `likely/unlikely` 宏，修复 `EncodeYmm.cpp` 依赖缺失。
- 强化 `CheckCpu.cpp` 在 Windows/MSVC 下的兼容性（使用 `__cpuidex/_xgetbv`），为后续硬件加速分发打基础。
- 统一 x86_64/aarch64 平台宏判断，确保在 Windows（`_M_X64/_M_ARM64`）下可正确启用对应实现。
- 修正多个 `.cpp/.h` 的架构宏，使 MSVC x64 下可编译并进入 AVX/AVX2 分发逻辑。
- 新增本地 `spdlog/spdlog.h` 兼容头（最小实现 `spdlog::error`），避免缺失外部依赖导致无法编译。
- 新增 `bitcal_dispatch.{h,cpp}`：使用函数指针表进行运行时分发（GPR/XMM/YMM），并提供 `namespace bitcal` 的纯函数 API。
- 将 `BitCal` 改为薄封装：不再 `new/delete` 多态实现类，成员函数直接转发到 `namespace bitcal`.
