; RUN: opt -passes='default<O0>' -fla -S < %s | FileCheck %s

@.ann.nofla = private unnamed_addr constant [6 x i8] c"nofla\00", section "llvm.metadata"
@.ann.file = private unnamed_addr constant [1 x i8] c"\00", section "llvm.metadata"

@llvm.global.annotations = appending global [1 x { ptr, ptr, ptr, i32 }] [
  { ptr, ptr, ptr, i32 } { ptr @annotated_nofla, ptr getelementptr inbounds ([6 x i8], ptr @.ann.nofla, i32 0, i32 0), ptr getelementptr inbounds ([1 x i8], ptr @.ann.file, i32 0, i32 0), i32 0 }
], section "llvm.metadata"

define i32 @should_flatten(i32 %x) {
entry:
  %cmp = icmp sgt i32 %x, 0
  br i1 %cmp, label %t, label %f

t:
  %a = add i32 %x, 1
  br label %exit

f:
  %b = sub i32 0, %x
  br label %exit

exit:
  %phi = phi i32 [ %a, %t ], [ %b, %f ]
  ret i32 %phi
}

define i32 @annotated_nofla(i32 %x) {
entry:
  %cmp = icmp sgt i32 %x, 0
  br i1 %cmp, label %t, label %f

t:
  %a = add i32 %x, 1
  br label %exit

f:
  %b = sub i32 0, %x
  br label %exit

exit:
  %phi = phi i32 [ %a, %t ], [ %b, %f ]
  ret i32 %phi
}

; CHECK-LABEL: define i32 @should_flatten
; CHECK: dispatchBB:
; CHECK-LABEL: define i32 @annotated_nofla
; CHECK-NOT: dispatchBB
; CHECK: ret i32
