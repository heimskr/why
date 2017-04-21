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
  unreachable

; <label>:14:                                     ; preds = %2
  %15 = getelementptr inbounds i8*, i8** %1, i64 1
  %16 = load i8*, i8** %15, align 8
  %17 = tail call %struct.__sFILE* @"\01_fopen"(i8* %16, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i64 0, i64 0)) #6
  %18 = icmp eq %struct.__sFILE* %17, null
  br i1 %18, label %19, label %22
}



define i32* @langref_gep_test(%struct.ST* %s) {
  %t1 = getelementptr %struct.ST, %struct.ST* %s, i32 1                        ; yields %struct.ST*:%t1
  %t2 = getelementptr %struct.ST, %struct.ST* %t1, i32 0, i32 2                ; yields %struct.RT*:%t2
  %t3 = getelementptr %struct.RT, %struct.RT* %t2, i32 0, i32 1                ; yields [10 x [20 x i32]]*:%t3
  %t4 = getelementptr [10 x [20 x i32]], [10 x [20 x i32]]* %t3, i32 0, i32 5  ; yields [20 x i32]*:%t4
  %t5 = getelementptr [20 x i32], [20 x i32]* %t4, i32 0, i32 13               ; yields i32*:%t5
  %arrayidx = getelementptr inbounds %struct.ST, %struct.ST* %s, i64 1, i32 2, i32 1, i64 5, i64 13
  ; yields [12 x i8]*:aptr
  %aptr = getelementptr {i32, [12 x i8]}, {i32, [12 x i8]}* %saptr, i64 0, i32 1
  ; yields i8*:vptr
  %vptr = getelementptr {i32, <2 x i8>}, {i32, <2 x i8>}* %svptr, i64 0, i32 1, i32 1
  ; yields i8*:eptr
  %eptr = getelementptr [12 x i8], [12 x i8]* %aptr, i64 0, i32 1
  ; yields i32*:iptr
  %iptr = getelementptr [10 x i32], [10 x i32]* @arr, i16 0, i16 0
  ; ret i32* %t5
}