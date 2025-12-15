# OLLVM21 项目设计文档

## 项目概述

OLLVM21（Obfuscator-LLVM 21）是一个基于 LLVM 21.1.5 的代码混淆工具，提供了多种代码保护技术，用于增强软件的安全性和抗逆向工程能力。该项目在原有 LLVM 架构上添加了多种混淆 pass，使开发者能够通过命令行参数或注解方式对 C/C++ 代码进行混淆处理。

## 项目结构

项目主要由以下部分组成：

```
llvm-project/llvm/lib/Passes/Obfuscation/
├── BogusControlFlow.cpp/.h       # 虚假控制流
├── CryptoUtils.cpp/.h            # 加密工具
├── Flattening.cpp/.h             # 控制流平坦化
├── IPObfuscationContext.cpp/.h   # 过程间混淆上下文
├── IndirectBranch.cpp/.h         # 间接分支
├── IndirectCall.cpp/.h           # 间接调用
├── IndirectGlobalVariable.cpp/.h # 间接全局变量
├── ObfuscationOptions.cpp/.h     # 混淆选项
├── SplitBasicBlock.cpp/.h        # 基本块分割
├── StringEncryption.cpp/.h       # 字符串加密
├── Substitution.cpp/.h           # 指令替换
└── Utils.cpp/.h                  # 工具函数
```

## 模块划分及功能

### 1. 核心混淆技术模块

#### 1.1 虚假控制流 (BogusControlFlow)
- **功能**：向基本块中添加复杂但从不执行的控制流路径，增加静态分析难度
- **参数**：
  - `bcf_prob`：混淆概率（1-100，默认 70）
  - `bcf_loop`：虚假控制流重复次数（默认 2）
- **原理**：通过添加不会执行的代码分支，改变原始代码的结构，但保持功能不变

#### 1.2 控制流平坦化 (Flattening)
- **功能**：将函数的控制流结构改为单层 switch-case 结构，破坏原始控制流
- **参数**：`fla`
- **原理**：使用分发器（dispatcher）和状态变量重构代码，所有基本块都从同一 switch 语句分发

#### 1.3 指令替换 (Substitution)
- **功能**：用等效但更复杂的指令序列替换简单指令
- **参数**：
  - `sub`：启用指令替换
  - `sub_loop`：指令替换次数（默认 1）
- **支持指令**：add/and/sub/or/xor

#### 1.4 字符串混淆 (StringEncryption)
- **功能**：加密代码中的字符串常量
- **参数**：`sobf`
- **限制**：仅支持窄字符，只能在命令行中启用，不支持 annotation

#### 1.5 基本块分割 (SplitBasicBlock)
- **功能**：将原基本块分割成多个小块
- **参数**：
  - `split`：启用基本块分割
  - `split_num`：分割数量（默认 3）
- **原理**：增加控制流复杂度，配合其他混淆技术使用效果更佳

### 2. 间接访问模块

#### 2.1 间接分支 (IndirectBranch)
- **功能**：将直接跳转指令替换为间接跳转
- **参数**：`ibr`
- **原理**：使用函数指针或表驱动方式替代直接跳转，增加静态分析难度

#### 2.2 间接调用 (IndirectCall)
- **功能**：将直接函数调用转换为间接调用
- **参数**：`icall`
- **原理**：通过函数指针实现函数调用，隐藏调用关系

#### 2.3 间接全局变量 (IndirectGlobalVariable)
- **功能**：将全局变量访问改为间接访问
- **参数**：`igv`
- **原理**：通过指针或索引等方式间接访问全局变量

### 3. 辅助模块

#### 3.1 加密工具 (CryptoUtils)
- **功能**：为混淆技术提供加密原语和随机数生成
- **应用**：用于字符串加密、控制流混淆中的条件计算等

#### 3.2 混淆选项 (ObfuscationOptions)
- **功能**：管理混淆配置，包括启用/禁用功能、函数过滤等
- **实现**：支持通过 YAML 配置文件和命令行参数控制混淆行为

#### 3.3 工具函数 (Utils)
- **功能**：提供通用工具函数和辅助方法
- **应用**：函数名控制混淆开关、注解解析等

## 设计思路

### 1. 集成设计

OLLVM21 采用模块化设计，将混淆功能以 LLVM Pass 的形式实现，并在 PassBuilder 中注册。这种设计允许：
- 各混淆 Pass 独立开发和维护
- 灵活组合不同混淆技术
- 与原生 LLVM 优化流程无缝集成

关键集成点是在`PassBuilder.cpp`中注册 Pipeline 回调，实现混淆 Pass 的按序执行。

### 2. 混淆应用流程

混淆应用遵循特定顺序，以最大化混淆效果：

1. 首先进行字符串加密 (StringEncryption)，为后续基本块混淆做准备
2. 然后应用间接调用 (IndirectCall)，修改函数调用方式
3. 接着进行基本块分割 (SplitBasicBlock)，为后续混淆创造更多基本块
4. 应用控制流平坦化 (Flattening)，彻底改变函数控制流结构
5. 执行指令替换 (Substitution)，替换简单指令
6. 添加虚假控制流 (BogusControlFlow)，进一步复杂化控制流
7. 最后应用间接分支 (IndirectBranch) 和间接全局变量 (IndirectGlobalVariable)

这种顺序设计确保了各混淆技术能够协同工作，而不会相互干扰。

### 3. 用户界面设计

OLLVM21 提供两种混淆控制方式：

1. **命令行控制**：通过`-mllvm`传递参数，如：
   ```
   -mllvm -fla -mllvm -bcf -mllvm -bcf_prob=80 -mllvm -bcf_loop=3
   ```

2. **注解控制**：通过`__attribute__((annotate()))`在源代码中精确控制特定函数的混淆：
   ```cpp
   int __attribute__((annotate("fla bcf igv ibr icall sub split"))) test_function() {...}
   ```

这种双层控制设计为用户提供了全局和细粒度的混淆控制能力。

## 安全考量

OLLVM21 的设计目标是增强软件安全性，主要针对以下几种攻击：

1. **静态分析**：通过复杂化控制流和指令替换，使静态反编译和分析变得困难
2. **动态分析**：间接访问和控制流混淆增加了调试和跟踪的难度
3. **模式识别**：通过指令替换和虚假控制流，破坏常见编译器生成的代码模式
4. **字符串提取**：字符串加密防止简单的字符串提取和分析

## 性能影响

混淆技术不可避免地会带来性能开销，OLLVM21 在设计中考虑了性能影响：

1. 控制流平坦化 (fla) 和虚假控制流 (bcf) 会显著增加代码大小和运行时间
2. 指令替换 (sub) 和基本块分割 (split) 对性能影响相对较小
3. 间接访问技术 (ibr/icall/igv) 主要影响指令缓存效率
4. 字符串加密 (sobf) 主要影响启动时间

用户可以根据安全需求和性能限制，选择适当的混淆组合。

## 扩展性

OLLVM21 的模块化设计使其具有良好的扩展性：

1. 新混淆技术可以作为独立 Pass 开发，并在 PassBuilder 中注册
2. 现有 Pass 可以通过参数进行调整和增强
3. 混淆选项框架 (ObfuscationOptions) 支持添加新的配置项
4. 加密工具 (CryptoUtils) 可以集成更多加密算法

## 兼容性

OLLVM21 基于 LLVM 21.1.5 开发，主要考虑的兼容性包括：

1. 编译器兼容性：与 Clang 紧密集成
2. 平台兼容性：支持 LLVM 支持的所有平台
3. 语言兼容性：主要针对 C/C++，理论上支持所有 LLVM 前端语言
4. 项目构建兼容性：提供 CMake 配置，支持 Visual Studio 等 IDE
