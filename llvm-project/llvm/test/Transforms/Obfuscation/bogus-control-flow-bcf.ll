; RUN: opt -passes='default<O0>' -bcf -bcf_prob=100 -bcf_loop=1 -S < %s | FileCheck %s

define i32 @bcf_test(i32 %x) {
entry:
  %a = add i32 %x, 1
  ret i32 %a
}

; CHECK: alteredBB
; CHECK: originalBBpart2
