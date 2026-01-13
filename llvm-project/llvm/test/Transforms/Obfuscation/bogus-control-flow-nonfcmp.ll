; RUN: opt -passes='default<O0>' -bcf -bcf_prob=100 -bcf_loop=1 -verify -S < %s | FileCheck %s

define i32 @bcf_arg_cond(i1 %c) {
entry:
  br i1 %c, label %t, label %f

t:
  ret i32 1

f:
  ret i32 0
}

; CHECK: alteredBB
