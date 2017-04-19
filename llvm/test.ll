; ModuleID = 'all.ll'
source_filename = "llvm-link"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }
%struct.GraphObj = type { %struct.ListObj**, i32*, i32*, i32*, i32*, i32, i32, i32 }
%struct.ListObj = type { %struct.NodeObj*, i32 }
%struct.NodeObj = type { %struct.NodeObj*, %struct.NodeObj*, i32 }

@.str = private unnamed_addr constant [38 x i8] c"Usage: %s <input file> <output file>\0A\00", align 1
