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
@.str.1 = private unnamed_addr constant [2 x i8] c"r\00", align 1
@.str.2 = private unnamed_addr constant [33 x i8] c"Couldn't open \22%s\22 for reading.\0A\00", align 1
@.str.3 = private unnamed_addr constant [2 x i8] c"w\00", align 1
@.str.4 = private unnamed_addr constant [33 x i8] c"Couldn't open \22%s\22 for writing.\0A\00", align 1
@.str.6 = private unnamed_addr constant [6 x i8] c"%d %d\00", align 1
@.str.7 = private unnamed_addr constant [37 x i8] c"Adjacency list representation of G:\0A\00", align 1
@.str.9 = private unnamed_addr constant [46 x i8] c"G contains %d strongly connected components:\0A\00", align 1
@.str.10 = private unnamed_addr constant [15 x i8] c"Component %d: \00", align 1
@__stderrp = external local_unnamed_addr global %struct.__sFILE*, align 8
@.str.11 = private unnamed_addr constant [51 x i8] c"Graph size (%d) and length of S (%d) don't match.\0A\00", align 1
@.str.1.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@.str.2.17 = private unnamed_addr constant [6 x i8] c"%s%d:\00", align 1
@.str.49 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1.50 = private unnamed_addr constant [4 x i8] c" %d\00", align 1

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main(i32, i8** nocapture readonly) local_unnamed_addr #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca %struct.GraphObj*, align 8
  %7 = alloca %struct.GraphObj*, align 8
  %8 = alloca %struct.ListObj*, align 8
  %9 = alloca %struct.ListObj*, align 8
  %10 = icmp eq i32 %0, 3
  br i1 %10, label %14, label %11

; <label>:11:                                     ; preds = %2
  %12 = load i8*, i8** %1, align 8
  %13 = tail call i32 (i8*, i6666, ...) @printf(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str, i64 0, i64 0), i8* %12)
  tail call void @exit(i32 1) #7
  ; unreachable
}