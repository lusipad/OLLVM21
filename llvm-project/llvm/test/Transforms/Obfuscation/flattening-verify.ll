; RUN: opt -passes='default<O0>' -fla -verify -S < %s | FileCheck %s

define i32 @fla_verify(i32 %x) {
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

; CHECK: dispatchBB:
; CHECK: switch
