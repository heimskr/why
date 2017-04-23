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

; <label>:19:                                     ; preds = %14
  %20 = load i8*, i8** %15, align 8
  %21 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([33 x i8], [33 x i8]* @.str.2, i64 0, i64 0), i8* %20)
  tail call void @exit(i32 1) #7
  unreachable

; <label>:22:                                     ; preds = %14
  %23 = getelementptr inbounds i8*, i8** %1, i64 2
  %24 = load i8*, i8** %23, align 8
  %25 = tail call %struct.__sFILE* @"\01_fopen"(i8* %24, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.3, i64 0, i64 0)) #6
  %26 = icmp eq %struct.__sFILE* %25, null
  br i1 %26, label %27, label %30

; <label>:27:                                     ; preds = %22
  %28 = load i8*, i8** %23, align 8
  %29 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([33 x i8], [33 x i8]* @.str.4, i64 0, i64 0), i8* %28)
  tail call void @exit(i32 1) #7
  unreachable

; <label>:30:                                     ; preds = %22
  %31 = call i32 (%struct.__sFILE*, i8*, ...) @fscanf(%struct.__sFILE* nonnull %17, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.49, i64 0, i64 0), i32* nonnull %3)
  %32 = load i32, i32* %3, align 4
  %33 = call %struct.GraphObj* @newGraph(i32 %32)
  store %struct.GraphObj* %33, %struct.GraphObj** %6, align 8
  %34 = call i32 (%struct.__sFILE*, i8*, ...) @fscanf(%struct.__sFILE* nonnull %17, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.6, i64 0, i64 0), i32* nonnull %4, i32* nonnull %5)
  %35 = icmp eq i32 %34, 2
  br i1 %35, label %.lr.ph49.preheader, label %._crit_edge50

.lr.ph49.preheader:                               ; preds = %30
  br label %.lr.ph49

.lr.ph49:                                         ; preds = %.lr.ph49.preheader, %40
  %36 = load i32, i32* %4, align 4
  %37 = load i32, i32* %5, align 4
  %38 = or i32 %37, %36
  %39 = icmp eq i32 %38, 0
  br i1 %39, label %._crit_edge50.loopexit, label %40

; <label>:40:                                     ; preds = %.lr.ph49
  call void @addArc(%struct.GraphObj* %33, i32 %36, i32 %37)
  %41 = call i32 (%struct.__sFILE*, i8*, ...) @fscanf(%struct.__sFILE* nonnull %17, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.6, i64 0, i64 0), i32* nonnull %4, i32* nonnull %5)
  %42 = icmp eq i32 %41, 2
  br i1 %42, label %.lr.ph49, label %._crit_edge50.loopexit

._crit_edge50.loopexit:                           ; preds = %.lr.ph49, %40
  br label %._crit_edge50

._crit_edge50:                                    ; preds = %._crit_edge50.loopexit, %30
  %43 = call i64 @fwrite(i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.7, i64 0, i64 0), i64 36, i64 1, %struct.__sFILE* nonnull %25)
  call void @printGraph(%struct.__sFILE* nonnull %25, %struct.GraphObj* %33)
  %fputc = call i32 @fputc(i32 10, %struct.__sFILE* nonnull %25)
  %44 = call %struct.GraphObj* @transpose(%struct.GraphObj* %33)
  store %struct.GraphObj* %44, %struct.GraphObj** %7, align 8
  %45 = call %struct.ListObj* @newList()
  store %struct.ListObj* %45, %struct.ListObj** %8, align 8
  %46 = load i32, i32* %3, align 4
  %47 = icmp slt i32 %46, 1
  br i1 %47, label %._crit_edge47, label %.lr.ph46.preheader

.lr.ph46.preheader:                               ; preds = %._crit_edge50
  br label %.lr.ph46

.lr.ph46:                                         ; preds = %.lr.ph46.preheader, %.lr.ph46
  %.03544 = phi i32 [ %48, %.lr.ph46 ], [ 1, %.lr.ph46.preheader ]
  call void @append(%struct.ListObj* %45, i32 %.03544)
  %48 = add nuw nsw i32 %.03544, 1
  %49 = load i32, i32* %3, align 4
  %50 = icmp slt i32 %.03544, %49
  br i1 %50, label %.lr.ph46, label %._crit_edge47.loopexit

._crit_edge47.loopexit:                           ; preds = %.lr.ph46
  br label %._crit_edge47

._crit_edge47:                                    ; preds = %._crit_edge47.loopexit, %._crit_edge50
  call void @DFS(%struct.GraphObj* %33, %struct.ListObj* %45)
  call void @DFS(%struct.GraphObj* %44, %struct.ListObj* %45)
  %51 = load i32, i32* %3, align 4
  %52 = sext i32 %51 to i64
  %53 = call i8* @calloc(i64 %52, i64 8)
  %54 = bitcast i8* %53 to %struct.ListObj**
  %55 = call %struct.ListObj* @newList()
  store %struct.ListObj* %55, %struct.ListObj** %9, align 8
  call void @moveBack(%struct.ListObj* %45)
  %56 = call i32 @index(%struct.ListObj* %45)
  %57 = icmp eq i32 %56, -1
  br i1 %57, label %._crit_edge43, label %.lr.ph42.preheader

.lr.ph42.preheader:                               ; preds = %._crit_edge47
  br label %.lr.ph42
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

define void @switches() {
  switch i32 %583, label %594 [
    i32 0, label %584
    i32 1, label %584
  ]

  switch i32 %130, label %211 [
    i32 0, label %131
    i32 1, label %132
    i32 2, label %161
  ]

  switch i32 %255, label %315 [
    i32 0, label %256
    i32 1, label %304
  ]

  switch i32 %15, label %28 [
    i32 16, label %16
    i32 48, label %16
    i32 17, label %17
    i32 1, label %17
    i32 8, label %18
    i32 24, label %19
    i32 56, label %20
    i32 25, label %21
    i32 9, label %21
    i32 20, label %22
    i32 52, label %22
    i32 21, label %23
    i32 5, label %23
    i32 12, label %24
    i32 28, label %25
    i32 60, label %26
    i32 29, label %27
    i32 13, label %27
  ]

  switch i32 %74, label %78 [
    i32 0, label %75
    i32 1, label %76
    i32 2, label %77
  ]
}