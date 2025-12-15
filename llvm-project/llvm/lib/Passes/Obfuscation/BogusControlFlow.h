#ifndef _BOGUSCONTROLFLOW_H_
#define _BOGUSCONTROLFLOW_H_

/**
 * @file BogusControlFlow.h
 * @brief 虚假控制流混淆头文件
 * 
 * 本文件定义了实现虚假控制流混淆的LLVM Pass类及相关接口。
 * 虚假控制流混淆是一种代码保护技术，通过在原有代码中插入不影响程序语义的
 * 冗余分支与基本块，增加程序控制流的复杂度，使反编译和静态分析变得更加困难。
 */

// LLVM 标准库
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"           // 处理 LLVM 模块
#include "llvm/IR/Function.h"         // 处理 LLVM 函数
#include "llvm/IR/BasicBlock.h"       // 处理 LLVM 基本块
#include "llvm/IR/Instructions.h"     // 处理 LLVM 指令
#include "llvm/IR/InstrTypes.h"       // LLVM 指令类型定义
#include "llvm/IR/Constants.h"        // 处理 LLVM 常量
#include "llvm/IR/Type.h"             // LLVM 类型系统
#include "llvm/ADT/Statistic.h"       // LLVM 统计功能
#include "llvm/IR/GlobalValue.h"      // 处理全局变量
#include "llvm/IR/LLVMContext.h"      // LLVM 上下文
#include "llvm/Transforms/Utils/Cloning.h"          // 克隆功能
#include "llvm/Transforms/Utils/BasicBlockUtils.h"  // 基本块工具
#include "llvm/CodeGen/ISDOpcodes.h"  // 指令操作码
#include "llvm/Support/raw_ostream.h" // 输出流
#include "llvm/Support/Debug.h"       // 调试支持
#include "llvm/Support/CommandLine.h" // 命令行参数处理
#include "llvm/Transforms/IPO.h"      // 过程间优化
#include "llvm/IR/IRBuilder.h"        // IR 构建工具
#include "llvm/IR/NoFolder.h"         // 禁止自动折叠常量
#include "llvm/Support/TargetSelect.h" // 目标选择
#include "llvm/Transforms/Utils/Local.h" // 局部变换工具

// 系统库
#include <list>
#include <memory>

// 项目自定义库
#include "CryptoUtils.h"  // 加密工具，用于生成随机数等
#include "Utils.h"        // 混淆通用工具

using namespace std;
using namespace llvm;

namespace llvm{ 
    /**
     * @class BogusControlFlowPass
     * @brief 虚假控制流混淆Pass
     * 
     * 这个Pass可以对LLVM IR进行虚假控制流混淆，通过以下步骤实现：
     * 1. 选择基本块
     * 2. 分割选中的基本块
     * 3. 创建修改后的基本块副本
     * 4. 插入永真条件的分支，一个分支指向原始代码，另一个指向混淆代码
     * 5. 将永真条件替换为复杂但等价的表达式
     */
    class BogusControlFlowPass : public PassInfoMixin<BogusControlFlowPass>{
        public:
            bool flag; // 控制是否启用混淆的标志

            /**
             * @brief 构造函数
             * @param flag 是否启用混淆的标志
             */
            BogusControlFlowPass(bool flag){
                this->flag = flag;
            }

            /**
             * @brief Pass的主要入口点
             * @param F 要处理的函数
             * @param AM 函数分析管理器
             * @return 保留的分析结果
             * 
             * 这个函数检查混淆参数，并决定是否对函数进行混淆处理
             */
            PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

            /**
             * @brief 对函数进行虚假控制流混淆
             * @param F 要混淆的函数
             * 
             * 这个函数遍历函数中的所有基本块，根据概率决定是否对其进行混淆
             */
            void bogus(Function &F);

            /**
             * @brief 为单个基本块添加虚假控制流
             * @param basicBlock 要处理的基本块
             * @param F 所属函数
             * 
             * 这个函数通过分割基本块、创建克隆块、添加条件跳转来实现混淆
             */
            void addBogusFlow(BasicBlock *basicBlock, Function &F);

            /**
             * @brief 完成混淆的最终处理
             * @param M 包含函数的模块
             * @param F 要处理的函数
             * @return 处理是否成功
             * 
             * 这个函数主要用于将所有永真条件替换为更复杂但等价的表达式
             */
            bool doF(Module &M, Function &F);

            /**
             * @brief 检查Pass是否必须
             * @return 始终返回true
             */
            static bool isRequired() { return true; }
    };

    /**
     * @brief 创建虚假控制流Pass的工厂函数
     * @param flag 是否启用混淆的标志
     * @return 新创建的BogusControlFlowPass对象指针
     */
    BogusControlFlowPass *createBogusControlFlow(bool flag);
}

#endif