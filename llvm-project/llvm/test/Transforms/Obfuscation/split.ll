; RUN: opt -passes='default<O0>' -split -split_num=3 -S < %s | FileCheck %s

define i32 @split_test(i32 %x) {
entry:
  %a = add i32 %x, 1
  %b = add i32 %a, 2
  %c = add i32 %b, 3
  ret i32 %c
}

; CHECK: entry.split:
