# Third-party notices / 第三方声明

本仓库是一个针对 LLVM 的补丁集（patch set），包含若干来自第三方项目的源文件或衍生修改。各文件的版权与许可信息以其文件头部声明为准。

## 项目整体许可（默认）

除非某个文件头部另有说明，本仓库自有代码按 `LICENSE`（AGPL-3.0-or-later）授权。

## LLVM Project

- 来源：https://github.com/llvm/llvm-project（tag: `llvmorg-21.1.5`）
- 许可：`Apache-2.0 WITH LLVM-exception`
- 涉及文件（本仓库内）：
  - `llvm-project/llvm/lib/Passes/PassBuilder.cpp`
- 许可文本：`LICENSES/Apache-2.0_WITH_LLVM-exception.txt`

## TypeART（CallSite 兼容头）

- 来源：https://github.com/tudasc/TypeART
- 许可：`BSD-3-Clause`（文件内也包含来自 LLVM 的 `Apache-2.0 WITH LLVM-exception` 片段，详见文件头部）
- 涉及文件：
  - `llvm-project/llvm/lib/Passes/Obfuscation/compat/CallSite.h`
- 许可文本：`LICENSES/BSD-3-Clause.txt`、`LICENSES/Apache-2.0_WITH_LLVM-exception.txt`

## University of Illinois/NCSA（旧 LLVM 风格许可）

- 许可：University of Illinois/NCSA Open Source License
- 涉及文件：
  - `llvm-project/llvm/lib/Passes/Obfuscation/CryptoUtils.cpp`
  - `llvm-project/llvm/lib/Passes/Obfuscation/CryptoUtils.h`
  - `llvm-project/llvm/lib/Passes/Obfuscation/Substitution.cpp`
  - `llvm-project/llvm/lib/Passes/Obfuscation/Substitution.h`
- 许可文本：`LICENSES/NCSA.txt`

## AGPL 组件（以文件头为准）

以下文件头部明确声明为 GNU Affero General Public License v3 或更高版本（v3+）：

- `llvm-project/llvm/lib/Passes/Obfuscation/BogusControlFlow.cpp`（头部注明“Relicensed … to AGPL”）
- `llvm-project/llvm/lib/Passes/Obfuscation/IndirectBranch.cpp`
- `llvm-project/llvm/lib/Passes/Obfuscation/IndirectGlobalVariable.cpp`
- `llvm-project/llvm/lib/Passes/Obfuscation/SplitBasicBlock.cpp`

注意：如果你将本补丁应用到官方 `llvm-project` 并分发构建产物（如 `clang`/`opt` 等），请自行评估并遵守 AGPL 的要求（例如提供对应源码等）。本文档仅用于整理许可信息，并不构成法律建议。

