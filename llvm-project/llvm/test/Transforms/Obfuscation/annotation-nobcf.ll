; RUN: opt -passes='default<O0>' -bcf -bcf_prob=100 -bcf_loop=1 -S < %s | FileCheck %s

@.ann.nobcf = private unnamed_addr constant [6 x i8] c"nobcf\00", section "llvm.metadata"
@.ann.file = private unnamed_addr constant [1 x i8] c"\00", section "llvm.metadata"

@llvm.global.annotations = appending global [1 x { ptr, ptr, ptr, i32 }] [
  { ptr, ptr, ptr, i32 } { ptr @annotated_nobcf, ptr getelementptr inbounds ([6 x i8], ptr @.ann.nobcf, i32 0, i32 0), ptr getelementptr inbounds ([1 x i8], ptr @.ann.file, i32 0, i32 0), i32 0 }
], section "llvm.metadata"

define i32 @should_bcf(i32 %x) {
entry:
  %a = add i32 %x, 1
  ret i32 %a
}

define i32 @annotated_nobcf(i32 %x) {
entry:
  %a = add i32 %x, 1
  ret i32 %a
}

; CHECK-LABEL: define i32 @should_bcf
; CHECK: alteredBB
; CHECK: originalBBpart2
; CHECK-LABEL: define i32 @annotated_nobcf
; CHECK-NOT: alteredBB
; CHECK: ret i32
