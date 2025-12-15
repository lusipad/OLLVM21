# OLLVM21

OLLVM21 是一套面向 LLVM 21.1.5 的混淆补丁集（patch set），用于把若干混淆 Pass 集成进上游 `llvm/llvm-project`（New PM）。

- 上游基线：`llvmorg-21.1.5`（CI 按此版本验证）
- 仓库形态：只包含需要覆盖到上游的文件（`llvm-project/...`），不包含完整 LLVM 源码
- 注入策略：通过 `PassBuilder::registerOptimizerLastEPCallback` 注入，默认仅在非 LTO 阶段（`ThinOrFullLTOPhase::None`）生效

## 变化摘要（相对旧版移植）

- 端口到 LLVM 21.1.5（PassBuilder 注入点与相关 API 变更）
- 修复/增强 annotation 解析逻辑（LLVM 15+，感谢 [@Jiang-Night](https://github.com/Jiang-Night)）
- 增加 lit 回归测试与 GitHub Actions CI（Linux 默认启用；Windows 默认禁用）

## 图例
<details> 
<summary>函数源码</summary>
<img src="resource/fn_source.png"/>
</details>
<details> 
<summary>原版IDA反编译</summary>
<img src="resource/fn_ida.png"/>
</details>
<details> 
<summary>开启fla</summary>
<img src="resource/fn_ida_fla.png"/>
</details>
<details> 
<summary>开启fla和bcf</summary>
<img src="resource/fn_ida_fla_bcf.png"/>
</details>

## 混淆开关

这些开关通过 `-mllvm` 传给 LLVM（例如 `clang -mllvm -fla`）。

- `-bcf`：虚假控制流
  - `-bcf_prob=<1-100>`：混淆概率（默认 70）
  - `-bcf_loop=<n>`：重复次数（默认 2）
- `-fla`：控制流平坦化
- `-sub`：指令替换（add/and/sub/or/xor）
  - `-sub_loop=<n>`：替换次数（默认 1）
- `-sobf`：字符串混淆（仅窄字符；只能命令行启用；不支持 annotation）
- `-split`：基本块分割
  - `-split_num=<n>`：分割数量（默认 3）
- `-ibr`：间接分支
- `-icall`：间接调用（call 寄存器）
- `-igv`：间接全局变量
- `-fncmd`：函数名控制混淆（annotation 已可用，通常不建议再用此方式）

## 示例：功能全开

> 注意：`fla`/`bcf` 可能显著拖慢编译，并可能对少量函数造成兼容性问题。

```bash
clang -O2 \
  -mllvm -fla \
  -mllvm -bcf -mllvm -bcf_prob=80 -mllvm -bcf_loop=3 \
  -mllvm -sobf \
  -mllvm -icall -mllvm -ibr -mllvm -igv \
  -mllvm -sub -mllvm -sub_loop=3 \
  -mllvm -split -mllvm -split_num=5 \
  test.c
```

## annotation 使用说明
```cpp
// 可传入上面的混淆开关名使此函数强制开启对应混淆功能
// 也可以传入no开头加功能命强制关闭对应函数的混淆功能, 例如__attribute__((annotate("nofla nobcf nosplit")))
int __attribute__((annotate("fla bcf igv ibr icall sub split"))) test_ollvm_aa() {
    std::printf("123aa\n");
    return 0;
}
```
## 应用补丁到官方 LLVM

1.下载官方源码 [LLVM 21.1.5](https://github.com/llvm/llvm-project/releases/tag/llvmorg-21.1.5)
2.把本仓库的补丁文件覆盖到上游源码中：`llvm-project/llvm/lib/Passes`、`llvm-project/llvm/test/Transforms/Obfuscation`

> 一键打补丁脚本
```powershell
# Windows PowerShell
.\scripts\apply_patch.ps1 -UpstreamRoot D:\path\to\llvm-project
```
```bash
# Linux/macOS bash
./scripts/apply_patch.sh /path/to/llvm-project
```

3.使用 CMake 生成构建目录并编译

> Ninja 示例（Windows/Linux/macOS 均可）
```bash
cd llvm-project
cmake -S llvm -B build_ninja -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;lld"
cmake --build build_ninja
```

> Visual Studio 示例（可选）
```bash
cd llvm-project
mkdir build_vs2022
cd build_vs2022
cmake -G "Visual Studio 17 2022" -DCMAKE_C_FLAGS=/utf-8 -DCMAKE_CXX_FLAGS=/utf-8 -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_EH=OFF -DLLVM_ENABLE_RTTI=OFF -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_ENABLE_PROJECTS="clang;lld" -A x64 ../llvm
```

> 注意：当前混淆 Pass 默认只在 `ThinOrFullLTOPhase::None`（非 LTO 阶段）注入。

## 测试
> 推荐在打补丁后的官方 llvm-project 源码目录运行
```bash
# 构建完成后
ninja -C build_ninja check-llvm

# 仅跑本项目新增的回归
python llvm/utils/lit/lit.py -sv llvm/test/Transforms/Obfuscation
```

## CI
本项目自带 GitHub Actions（`.github/workflows/ci.yml`），会在 Ubuntu 上拉取官方 `llvmorg-21.1.5`，覆盖本项目补丁并运行：
```bash
ninja -C llvm-upstream/build check-llvm-transforms-obfuscation
```
Windows Job 默认禁用：可在手动触发（workflow_dispatch）时设置 `run_windows=true`，或给 PR 添加 label `ci-windows` 启用。

## 参与贡献
见 `CONTRIBUTING.md`、`SECURITY.md`、`CODE_OF_CONDUCT.md`。

## License
本仓库包含多种许可证：默认以 `LICENSE`（AGPL-3.0-or-later）授权；第三方代码及其许可证声明见 `THIRD_PARTY_NOTICES.md`，对应许可全文位于 `LICENSES/`。

注意：部分混淆 Pass 源文件头部为 AGPL（v3+）。如果你将补丁应用到官方 `llvm-project` 并分发构建产物（如 `clang`/`opt` 等），请自行评估并遵守 AGPL 的要求。

## 实现说明（补丁落点）

- `llvm-project/llvm/lib/Passes/PassBuilder.cpp`：注册命令行开关，并把混淆 Pass 注入到优化流水线末尾
- `llvm-project/llvm/lib/Passes/CMakeLists.txt`：把 `Obfuscation/*.cpp` 加入 `LLVMPasses`
- `llvm-project/llvm/test/Transforms/Obfuscation/`：本项目新增的 lit 回归测试
- 设计/说明文档：`design.md`

## Credits
[LLVM](https://github.com/llvm/llvm-project)

[SsagePass](https://github.com/SsageParuders/SsagePass)

[wwh1004-ollvm16](https://github.com/wwh1004/ollvm-16)
