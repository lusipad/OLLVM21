# 贡献指南

## 仓库定位

本仓库是一个 **补丁集**（patch set）：只包含需要覆盖到官方 `llvm/llvm-project` 的文件，而不是完整的 LLVM 源码镜像。

## 开发流程建议

1. 基于官方 tag 准备源码（当前目标：`llvmorg-21.1.5`）。
2. 将本仓库的内容覆盖到官方源码对应路径（见 `README.md`）。
3. 使用 Ninja 或 VS 生成并构建。
4. 跑回归测试（至少跑本项目的 Obfuscation lit）。

## 回归测试

优先保证以下测试通过：

```bash
python llvm/utils/lit/lit.py -sv llvm/test/Transforms/Obfuscation
```

## 代码风格

- 以 LLVM 代码风格为准（尽量保持与周边代码一致）。
- 修改尽量小、聚焦；不要顺手重构无关代码。
- 新增功能请尽量补 lit 回归测试到 `llvm/test/Transforms/Obfuscation`。

