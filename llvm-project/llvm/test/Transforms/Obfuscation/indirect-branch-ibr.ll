; RUN: opt -passes='default<O0>' -ibr -S < %s | FileCheck %s

define i32 @cond(i32 %x) {
entry:
  %cmp = icmp sgt i32 %x, 0
  br i1 %cmp, label %t, label %f

t:
  ret i32 1

f:
  ret i32 0
}

; CHECK: @cond_IndirectBrTargets = private global
; CHECK: indirectbr
