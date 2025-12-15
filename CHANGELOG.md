# Changelog

本项目的版本与变更主要围绕上游 LLVM tag（如 `llvmorg-21.1.5`）。格式参考 Keep a Changelog。

## [Unreleased]

### Added

- 新增 GitHub Actions CI：Ubuntu 拉取 `llvmorg-21.1.5`、应用补丁并运行 `check-llvm-transforms-obfuscation`
- 新增 Windows CI（默认禁用）：仅在 `workflow_dispatch` 设置 `run_windows=true` 或 PR label `ci-windows` 时启用
- 新增一键打补丁脚本：`scripts/apply_patch.sh`、`scripts/apply_patch.ps1`
- 新增 Obfuscation 回归测试：`llvm-project/llvm/test/Transforms/Obfuscation/*.ll`
- 补全文档与协作文件：`CONTRIBUTING.md`、`SECURITY.md`、Issue/PR 模板等

### Changed

- 适配 LLVM 21.1.5（New PM / PassBuilder 注入点与相关 API 变更）
- 混淆 Pass 注入到 `OptimizerLastEPCallback`，并默认仅在 `ThinOrFullLTOPhase::None`（非 LTO 阶段）生效

### Fixed

- 修复/增强 annotation 读取逻辑以兼容 LLVM 15+（见 `llvm-project/llvm/lib/Passes/Obfuscation/Utils.cpp`）

