; RUN: opt -passes='default<O0>' -icall -S < %s | FileCheck %s

define i32 @callee(i32 %x) {
entry:
  %y = add i32 %x, 1
  ret i32 %y
}

define i32 @caller(i32 %x) {
entry:
  %r = call i32 @callee(i32 %x)
  ret i32 %r
}

; CHECK: @caller_IndirectCallees = private global
; CHECK-LABEL: define i32 @caller
; CHECK: Call_callee
