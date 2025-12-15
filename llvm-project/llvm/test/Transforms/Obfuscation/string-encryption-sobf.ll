; RUN: opt -passes='default<O0>' -sobf -S < %s | FileCheck %s

@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1

define ptr @use_str() {
entry:
  ret ptr @.str
}

; CHECK: @EncryptedStringTable = private constant
; CHECK: define private void @goron_decrypt_string_0(
