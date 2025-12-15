; RUN: opt -passes='default<O0>' -igv -S < %s | FileCheck %s

@G = global i32 0, align 4

define i32 @use_g() {
entry:
  %v = load volatile i32, ptr @G, align 4
  ret i32 %v
}

; CHECK: @use_g_IndirectGVars = private global
; CHECK: IndGV1_
