; RUN: opt -passes='default<O0>,dce' -sub -S < %s | FileCheck %s

define i32 @sub_test(i32 %a, i32 %b) {
entry:
  %x = and i32 %a, %b
  ret i32 %x
}

; CHECK-LABEL: define i32 @sub_test
; CHECK: xor i32
; CHECK-NOT: and i32 %a, %b
