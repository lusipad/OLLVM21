#include "IndirectCall.h"

using namespace llvm;
using std::vector;

/**
 * @brief 
 * 
 * @param F 
 * @param AM 
 * @return PreservedAnalyses 
 */
PreservedAnalyses IndirectCallPass::run(Function &F, FunctionAnalysisManager &AM){
    // 判断是否需要开启间接调用
    if (toObfuscate(flag, &F, "icall")){
      doIndirctCall(F);
      return PreservedAnalyses::none();
    }
    return PreservedAnalyses::all();
}

bool IndirectCallPass::doIndirctCall(Function &Fn){
    if (Options && Options->skipFunction(Fn.getName())) {
        return false;
    }

    LLVMContext &Ctx = Fn.getContext();

    CalleeNumbering.clear();
    Callees.clear();
    CallSites.clear();

    NumberCallees(Fn);

    if (Callees.empty()) {
        return false;
    }

    uint64_t V = RandomEngine.get_uint64_t();
    IntegerType *intType = Type::getInt32Ty(Ctx);

    unsigned pointerSize = Fn.getEntryBlock().getModule()->getDataLayout().getTypeAllocSize(PointerType::getUnqual(Fn.getContext())); // Soule
    if (pointerSize == 8) {
        intType = Type::getInt64Ty(Ctx);
    }
    ConstantInt *EncKey = ConstantInt::get(intType, V, false);
    ConstantInt *EncKey1 = ConstantInt::get(intType, -V, false);

    Value *MySecret = ConstantInt::get(intType, 0, true);

    ConstantInt *Zero = ConstantInt::get(intType, 0);
    GlobalVariable *Targets = getIndirectCallees(Fn, EncKey1);

    for (CallInst *CI : CallSites) {
        CallBase *CB = CI;

        Function *Callee = CB->getCalledFunction();
        FunctionType *FTy = CB->getFunctionType();
        IRBuilder<> IRB(CB);

        Value *Idx =
            ConstantInt::get(intType, CalleeNumbering[CB->getCalledFunction()]);
        Value *GEP =
            IRB.CreateGEP(Targets->getValueType(), Targets, {Zero, Idx});
        LoadInst *EncDestAddr = IRB.CreateLoad(GEP->getType(), GEP, CI->getName());

        Value *Secret = IRB.CreateAdd(EncKey, MySecret);
        Value *DestAddr =
            IRB.CreateGEP(Type::getInt8Ty(Ctx), EncDestAddr, Secret);

        Value *FnPtr = IRB.CreateBitCast(DestAddr, FTy->getPointerTo());
        FnPtr->setName("Call_" + Callee->getName());
        CB->setCalledOperand(FnPtr);
    }

    return true;
}

/**
 * @brief 
 * 
 * @param F 
 * @param EncKey 
 * @return GlobalVariable* 
 */
GlobalVariable *IndirectCallPass::getIndirectCallees(Function &F, ConstantInt *EncKey){
    std::string GVName(F.getName().str() + "_IndirectCallees");
    GlobalVariable *GV = F.getParent()->getNamedGlobal(GVName);
    if (GV){
        return GV;
    }
    // callee's address
    std::vector<Constant *> Elements;
    auto *PtrTy = PointerType::get(F.getContext(), 0);
    for (auto Callee : Callees){
        Constant *CE = ConstantExpr::getBitCast(Callee, PtrTy);
        CE = ConstantExpr::getGetElementPtr(Type::getInt8Ty(F.getContext()), CE, EncKey);
        Elements.push_back(CE);
    }
    ArrayType *ATy = ArrayType::get(PtrTy, Elements.size());
    Constant *CA = ConstantArray::get(ATy, ArrayRef<Constant *>(Elements));
    GV = new GlobalVariable(*F.getParent(), ATy, false, GlobalValue::LinkageTypes::PrivateLinkage, CA, GVName);
    appendToCompilerUsed(*F.getParent(), {GV});
    return GV;
}

/**
 * @brief 
 * 
 * @param F 
 */
void IndirectCallPass::NumberCallees(Function &F){
    for (auto &BB : F){
        for (auto &I : BB){
            if (auto *CI = dyn_cast<CallInst>(&I)){
                Function *Callee = CI->getCalledFunction();
                if (Callee == nullptr){
                    continue;
                }
                if (Callee->isIntrinsic()){
                    continue;
                }
                CallSites.push_back(CI);
                if (CalleeNumbering.count(Callee) == 0){
                    CalleeNumbering[Callee] = Callees.size();
                    Callees.push_back(Callee);
                }
            }
        }
    }
}

/**
 * @brief 
 * 
 * @param flag 
 * @return IndirectCallPass* 
 */
IndirectCallPass *llvm::createIndirectCall(bool flag){
    return new IndirectCallPass(flag);
}
