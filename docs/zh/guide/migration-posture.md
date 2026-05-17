# 迁移姿态

BitCal vNext 不再把旧公开模型的兼容性当作第一目标。当前方向明确接受 breaking change，以换取更清晰的公开契约和更可验证的性能叙事。

## 当前迁移立场

- `<bitcal/bitcal.hpp>` 仍然是唯一稳定 include seam
- 公共模型正在收敛到 `bit_block`、`bit_view`、`const_bit_view` 与自由算法
- 旧 `bitarray` 叙事应当被视为历史背景，而不是未来设计中心

## 评估建议

如果你现在评估 BitCal，应当评估正在被 OpenSpec 与验证路径共同收敛的方向，而不是先前留下来的旧文案。
