# 类型与视图

参考面正在收敛成一个明确 ownership model，而不是便利接口的罗列。

## 角色表

| 表面 | 所有权 | 预期用途 |
| --- | --- | --- |
| `bit_block<Bits>` | owning | 承载定位宽存储与显式布局 |
| `bit_view` | borrowed mutable | 在不复制的前提下操作已有存储 |
| `const_bit_view` | borrowed read-only | 读取或把已有存储送入算法 |

## 参考规则

- 先解释角色，再解释语法
- include 指引保持简单，只使用 `<bitcal/bitcal.hpp>`
- detail 头文件属于实现材料，不是受支持入口

## 为什么本页存在

如果读者分不清谁拥有存储、谁只是观察存储，后面的契约都会变得模糊。本页必须先去掉这个歧义。
