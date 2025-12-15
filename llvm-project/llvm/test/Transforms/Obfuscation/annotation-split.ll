; RUN: opt -passes='default<O0>' -S < %s | FileCheck %s

@.ann = private unnamed_addr constant [6 x i8] c"split\00", section "llvm.metadata"
@.ann.file = private unnamed_addr constant [1 x i8] c"\00", section "llvm.metadata"

@llvm.global.annotations = appending global [1 x { ptr, ptr, ptr, i32 }] [
  { ptr, ptr, ptr, i32 } { ptr @annotated_split, ptr getelementptr inbounds ([6 x i8], ptr @.ann, i32 0, i32 0), ptr getelementptr inbounds ([1 x i8], ptr @.ann.file, i32 0, i32 0), i32 0 }
], section "llvm.metadata"

define i32 @annotated_split(i32 %x) {
entry:
  %a = add i32 %x, 1
  %b = add i32 %a, 2
  %c = add i32 %b, 3
  ret i32 %c
}

; CHECK: entry.split:
