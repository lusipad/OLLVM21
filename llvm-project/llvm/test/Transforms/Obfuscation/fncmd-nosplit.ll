; RUN: opt -passes='default<O0>' -split -fncmd -S < %s | FileCheck %s

define i32 @will_split(i32 %x) {
entry:
  %a = add i32 %x, 1
  %b = add i32 %a, 2
  %c = add i32 %b, 3
  ret i32 %c
}

define i32 @no_split_nosplit_(i32 %x) {
entry:
  %a = add i32 %x, 1
  %b = add i32 %a, 2
  %c = add i32 %b, 3
  ret i32 %c
}

; CHECK-LABEL: define i32 @will_split
; CHECK: entry.split
; CHECK-LABEL: define i32 @no_split_nosplit_
; CHECK-NOT: .split
; CHECK: ret i32
