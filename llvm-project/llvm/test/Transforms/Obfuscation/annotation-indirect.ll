; RUN: opt -passes='default<O0>' -S < %s | FileCheck %s

@G = global i32 0, align 4

@.ann.icall = private unnamed_addr constant [6 x i8] c"icall\00", section "llvm.metadata"
@.ann.ibr = private unnamed_addr constant [4 x i8] c"ibr\00", section "llvm.metadata"
@.ann.igv = private unnamed_addr constant [4 x i8] c"igv\00", section "llvm.metadata"
@.ann.file = private unnamed_addr constant [1 x i8] c"\00", section "llvm.metadata"

@llvm.global.annotations = appending global [3 x { ptr, ptr, ptr, i32 }] [
  { ptr, ptr, ptr, i32 } { ptr @anno_icall, ptr getelementptr inbounds ([6 x i8], ptr @.ann.icall, i32 0, i32 0), ptr getelementptr inbounds ([1 x i8], ptr @.ann.file, i32 0, i32 0), i32 0 },
  { ptr, ptr, ptr, i32 } { ptr @anno_ibr, ptr getelementptr inbounds ([4 x i8], ptr @.ann.ibr, i32 0, i32 0), ptr getelementptr inbounds ([1 x i8], ptr @.ann.file, i32 0, i32 0), i32 0 },
  { ptr, ptr, ptr, i32 } { ptr @anno_igv, ptr getelementptr inbounds ([4 x i8], ptr @.ann.igv, i32 0, i32 0), ptr getelementptr inbounds ([1 x i8], ptr @.ann.file, i32 0, i32 0), i32 0 }
], section "llvm.metadata"

define i32 @callee(i32 %x) {
entry:
  %y = add i32 %x, 1
  ret i32 %y
}

define i32 @anno_icall(i32 %x) {
entry:
  %r = call i32 @callee(i32 %x)
  ret i32 %r
}

define i32 @anno_ibr(i32 %x) {
entry:
  %cmp = icmp sgt i32 %x, 0
  br i1 %cmp, label %t, label %f

t:
  ret i32 1

f:
  ret i32 0
}

define i32 @anno_igv() {
entry:
  %v = load i32, ptr @G, align 4
  ret i32 %v
}

; CHECK: @anno_icall_IndirectCallees = private global
; CHECK-LABEL: define i32 @anno_icall
; CHECK: Call_callee
; CHECK: @anno_ibr_IndirectBrTargets = private global
; CHECK-LABEL: define i32 @anno_ibr
; CHECK: indirectbr
; CHECK: @anno_igv_IndirectGVars = private global
; CHECK-LABEL: define i32 @anno_igv
; CHECK: IndGV1_
