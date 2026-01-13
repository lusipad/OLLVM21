; RUN: opt -passes='default<O0>' -split -split_num=3 -S < %s | FileCheck %s

define i32 @split_ok(i32 %x) {
entry:
  %a = add i32 %x, 1
  %b = add i32 %a, 2
  %c = add i32 %b, 3
  ret i32 %c
}

define i32 @split_has_phi(i1 %cond, i32 %a, i32 %b) {
entry:
  br i1 %cond, label %t, label %f

t:
  br label %merge

f:
  br label %merge

merge:
  %v = phi i32 [ %a, %t ], [ %b, %f ]
  ret i32 %v
}

; CHECK-LABEL: define i32 @split_ok
; CHECK: entry.split
; CHECK-LABEL: define i32 @split_has_phi
; CHECK-NOT: .split
; CHECK: ret i32
