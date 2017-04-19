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
  %13 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str, i64 0, i64 0), i8* %12)
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

.lr.ph42:                                         ; preds = %.lr.ph42.preheader, %67
  %.03340 = phi i32 [ %.1, %67 ], [ 0, %.lr.ph42.preheader ]
  %58 = call i32 @get(%struct.ListObj* %45)
  call void @prepend(%struct.ListObj* %55, i32 %58)
  %59 = call i32 @get(%struct.ListObj* %45)
  %60 = call i32 @getParent(%struct.GraphObj* %44, i32 %59)
  %61 = icmp eq i32 %60, -1
  br i1 %61, label %62, label %67

; <label>:62:                                     ; preds = %.lr.ph42
  %63 = call %struct.ListObj* @copyList(%struct.ListObj* %55)
  %64 = add nsw i32 %.03340, 1
  %65 = sext i32 %.03340 to i64
  %66 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %54, i64 %65
  store %struct.ListObj* %63, %struct.ListObj** %66, align 8
  call void @clear(%struct.ListObj* %55)
  br label %67

; <label>:67:                                     ; preds = %62, %.lr.ph42
  %.1 = phi i32 [ %64, %62 ], [ %.03340, %.lr.ph42 ]
  call void @movePrev(%struct.ListObj* %45)
  %68 = call i32 @index(%struct.ListObj* %45)
  %69 = icmp eq i32 %68, -1
  br i1 %69, label %._crit_edge43.loopexit, label %.lr.ph42

._crit_edge43.loopexit:                           ; preds = %67
  %.1.lcssa = phi i32 [ %.1, %67 ]
  br label %._crit_edge43

._crit_edge43:                                    ; preds = %._crit_edge43.loopexit, %._crit_edge47
  %.033.lcssa = phi i32 [ 0, %._crit_edge47 ], [ %.1.lcssa, %._crit_edge43.loopexit ]
  call void @freeList(%struct.ListObj** nonnull %9)
  %70 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* nonnull %25, i8* getelementptr inbounds ([46 x i8], [46 x i8]* @.str.9, i64 0, i64 0), i32 %.033.lcssa)
  %71 = icmp sgt i32 %.033.lcssa, 0
  br i1 %71, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %._crit_edge43
  %wide.trip.count = zext i32 %.033.lcssa to i64
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %72 = trunc i64 %indvars.iv.next to i32
  %73 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* nonnull %25, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.10, i64 0, i64 0), i32 %72)
  %74 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %54, i64 %indvars.iv
  %75 = load %struct.ListObj*, %struct.ListObj** %74, align 8
  call void @printList(%struct.__sFILE* nonnull %25, %struct.ListObj* %75)
  %fputc38 = call i32 @fputc(i32 10, %struct.__sFILE* nonnull %25)
  call void @freeList(%struct.ListObj** %74)
  %exitcond = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %.lr.ph
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %._crit_edge43
  %76 = call i32 @fclose(%struct.__sFILE* nonnull %17)
  %77 = call i32 @fclose(%struct.__sFILE* nonnull %25)
  call void @free(i8* %53)
  call void @freeList(%struct.ListObj** nonnull %8)
  call void @freeGraph(%struct.GraphObj** nonnull %6)
  call void @freeGraph(%struct.GraphObj** nonnull %7)
  ret i32 0
}

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #1

; Function Attrs: noreturn
declare void @exit(i32) local_unnamed_addr #2

declare %struct.__sFILE* @"\01_fopen"(i8*, i8*) local_unnamed_addr #3

; Function Attrs: nounwind
declare i32 @fscanf(%struct.__sFILE* nocapture, i8* nocapture readonly, ...) local_unnamed_addr #1

; Function Attrs: nounwind
declare i32 @fprintf(%struct.__sFILE* nocapture, i8* nocapture readonly, ...) local_unnamed_addr #1

; Function Attrs: nounwind
declare noalias i8* @calloc(i64, i64) local_unnamed_addr #1

; Function Attrs: nounwind
declare i32 @fclose(%struct.__sFILE* nocapture) local_unnamed_addr #1

; Function Attrs: nounwind
declare void @free(i8* nocapture) local_unnamed_addr #1

; Function Attrs: noinline nounwind ssp uwtable
define noalias %struct.GraphObj* @newGraph(i32) local_unnamed_addr #0 {
  %2 = tail call i8* @malloc(i64 56)
  %3 = bitcast i8* %2 to %struct.GraphObj*
  %4 = sext i32 %0 to i64
  %5 = tail call i8* @calloc(i64 %4, i64 4)
  %6 = getelementptr inbounds i8, i8* %2, i64 8
  %7 = bitcast i8* %6 to i8**
  store i8* %5, i8** %7, align 8
  %8 = tail call i8* @calloc(i64 %4, i64 4)
  %9 = getelementptr inbounds i8, i8* %2, i64 16
  %10 = bitcast i8* %9 to i8**
  store i8* %8, i8** %10, align 8
  %11 = tail call i8* @calloc(i64 %4, i64 4)
  %12 = getelementptr inbounds i8, i8* %2, i64 24
  %13 = bitcast i8* %12 to i8**
  store i8* %11, i8** %13, align 8
  %14 = tail call i8* @calloc(i64 %4, i64 4)
  %15 = getelementptr inbounds i8, i8* %2, i64 32
  %16 = bitcast i8* %15 to i8**
  store i8* %14, i8** %16, align 8
  %17 = getelementptr inbounds i8, i8* %2, i64 40
  %18 = bitcast i8* %17 to i32*
  store i32 0, i32* %18, align 8
  %19 = getelementptr inbounds i8, i8* %2, i64 44
  %20 = bitcast i8* %19 to i32*
  store i32 %0, i32* %20, align 4
  %21 = getelementptr inbounds i8, i8* %2, i64 48
  %22 = bitcast i8* %21 to i32*
  store i32 0, i32* %22, align 8
  %23 = bitcast i8* %2 to %struct.ListObj***
  store %struct.ListObj** null, %struct.ListObj*** %23, align 8
  tail call void @makeNull(%struct.GraphObj* %3)
  tail call void @resetDFS(%struct.GraphObj* %3)
  ret %struct.GraphObj* %3
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) local_unnamed_addr #1

; Function Attrs: noinline nounwind ssp uwtable
define void @makeNull(%struct.GraphObj* nocapture) local_unnamed_addr #0 {
  tail call void @freeAdjacent(%struct.GraphObj* %0)
  %2 = tail call i32 @getSize(%struct.GraphObj* %0)
  %3 = sext i32 %2 to i64
  %4 = tail call i8* @calloc(i64 %3, i64 8)
  %5 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 0
  %6 = bitcast %struct.GraphObj* %0 to i8**
  store i8* %4, i8** %6, align 8
  %7 = tail call i32 @getSize(%struct.GraphObj* %0)
  %8 = icmp sgt i32 %7, 0
  br i1 %8, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %1
  %wide.trip.count = zext i32 %7 to i64
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %9 = tail call %struct.ListObj* @newList()
  %10 = load %struct.ListObj**, %struct.ListObj*** %5, align 8
  %11 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %10, i64 %indvars.iv
  store %struct.ListObj* %9, %struct.ListObj** %11, align 8
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %.lr.ph
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %1
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @resetDFS(%struct.GraphObj* nocapture) local_unnamed_addr #4 {
  %2 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 7
  store i32 0, i32* %2, align 8
  %3 = tail call i32 @getSize(%struct.GraphObj* nonnull %0)
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %1
  %5 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 1
  %6 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 2
  %7 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 3
  %8 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 4
  br label %9

; <label>:9:                                      ; preds = %.lr.ph, %9
  %indvars.iv = phi i64 [ 0, %.lr.ph ], [ %indvars.iv.next, %9 ]
  %10 = load i32*, i32** %5, align 8
  %11 = getelementptr inbounds i32, i32* %10, i64 %indvars.iv
  store i32 -1, i32* %11, align 4
  %12 = load i32*, i32** %6, align 8
  %13 = getelementptr inbounds i32, i32* %12, i64 %indvars.iv
  store i32 -1, i32* %13, align 4
  %14 = load i32*, i32** %7, align 8
  %15 = getelementptr inbounds i32, i32* %14, i64 %indvars.iv
  store i32 -1, i32* %15, align 4
  %16 = load i32*, i32** %8, align 8
  %17 = getelementptr inbounds i32, i32* %16, i64 %indvars.iv
  store i32 1, i32* %17, align 4
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %18 = tail call i32 @getSize(%struct.GraphObj* nonnull %0)
  %19 = sext i32 %18 to i64
  %20 = icmp slt i64 %indvars.iv.next, %19
  br i1 %20, label %9, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %9
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %1
  ret void
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @getSize(%struct.GraphObj* nocapture readonly) local_unnamed_addr #5 {
  %2 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 6
  %3 = load i32, i32* %2, align 4
  ret i32 %3
}

; Function Attrs: noinline nounwind ssp uwtable
define void @freeAdjacent(%struct.GraphObj* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 0
  %3 = load %struct.ListObj**, %struct.ListObj*** %2, align 8
  %4 = icmp eq %struct.ListObj** %3, null
  br i1 %4, label %14, label %.preheader

.preheader:                                       ; preds = %1
  %5 = tail call i32 @getSize(%struct.GraphObj* nonnull %0)
  %6 = icmp sgt i32 %5, 0
  br i1 %6, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %.preheader
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %7 = phi %struct.ListObj** [ %12, %.lr.ph ], [ %3, %.lr.ph.preheader ]
  %8 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %7, i64 %indvars.iv
  tail call void @freeList(%struct.ListObj** %8)
  %indvars.iv.next = add nuw i64 %indvars.iv, 1
  %9 = tail call i32 @getSize(%struct.GraphObj* nonnull %0)
  %10 = sext i32 %9 to i64
  %11 = icmp slt i64 %indvars.iv.next, %10
  %12 = load %struct.ListObj**, %struct.ListObj*** %2, align 8
  br i1 %11, label %.lr.ph, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %.lr.ph
  %.lcssa12 = phi %struct.ListObj** [ %12, %.lr.ph ]
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %.preheader
  %.lcssa = phi %struct.ListObj** [ %3, %.preheader ], [ %.lcssa12, %._crit_edge.loopexit ]
  %13 = bitcast %struct.ListObj** %.lcssa to i8*
  tail call void @free(i8* %13)
  store %struct.ListObj** null, %struct.ListObj*** %2, align 8
  br label %14

; <label>:14:                                     ; preds = %1, %._crit_edge
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @freeGraph(%struct.GraphObj** nocapture) local_unnamed_addr #0 {
  %2 = load %struct.GraphObj*, %struct.GraphObj** %0, align 8
  tail call void @freeAdjacent(%struct.GraphObj* %2)
  %3 = load %struct.GraphObj*, %struct.GraphObj** %0, align 8
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %3, i64 0, i32 1
  %5 = bitcast i32** %4 to i8**
  %6 = load i8*, i8** %5, align 8
  tail call void @free(i8* %6)
  %7 = load %struct.GraphObj*, %struct.GraphObj** %0, align 8
  %8 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %7, i64 0, i32 2
  %9 = bitcast i32** %8 to i8**
  %10 = load i8*, i8** %9, align 8
  tail call void @free(i8* %10)
  %11 = load %struct.GraphObj*, %struct.GraphObj** %0, align 8
  %12 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %11, i64 0, i32 3
  %13 = bitcast i32** %12 to i8**
  %14 = load i8*, i8** %13, align 8
  tail call void @free(i8* %14)
  %15 = load %struct.GraphObj*, %struct.GraphObj** %0, align 8
  %16 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %15, i64 0, i32 4
  %17 = bitcast i32** %16 to i8**
  %18 = load i8*, i8** %17, align 8
  tail call void @free(i8* %18)
  %19 = bitcast %struct.GraphObj** %0 to i8**
  %20 = load i8*, i8** %19, align 8
  tail call void @free(i8* %20)
  store %struct.GraphObj* null, %struct.GraphObj** %0, align 8
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define i32 @getOrder(%struct.GraphObj* nocapture readonly) local_unnamed_addr #4 {
  %2 = tail call i32 @getSize(%struct.GraphObj* %0)
  %3 = icmp slt i32 %2, 1
  br i1 %3, label %._crit_edge17, label %.lr.ph16

.lr.ph16:                                         ; preds = %1
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 0
  br label %5

; <label>:5:                                      ; preds = %._crit_edge, %.lr.ph16
  %indvars.iv = phi i64 [ %indvars.iv.next, %._crit_edge ], [ 1, %.lr.ph16 ]
  %.014 = phi i32 [ %.1.lcssa, %._crit_edge ], [ 0, %.lr.ph16 ]
  %6 = load %struct.ListObj**, %struct.ListObj*** %4, align 8
  %7 = add nsw i64 %indvars.iv, -1
  %8 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %6, i64 %7
  %9 = load %struct.ListObj*, %struct.ListObj** %8, align 8
  tail call void @moveFront(%struct.ListObj* %9)
  %10 = tail call i32 @index(%struct.ListObj* %9)
  %11 = icmp eq i32 %10, -1
  br i1 %11, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %5
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.111 = phi i32 [ %.1., %.lr.ph ], [ %.014, %.lr.ph.preheader ]
  %12 = tail call i32 @get(%struct.ListObj* %9)
  %13 = sext i32 %12 to i64
  %not. = icmp sle i64 %indvars.iv, %13
  %14 = zext i1 %not. to i32
  %.1. = add nsw i32 %14, %.111
  tail call void @moveNext(%struct.ListObj* %9)
  %15 = tail call i32 @index(%struct.ListObj* %9)
  %16 = icmp eq i32 %15, -1
  br i1 %16, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %.lr.ph
  %.1..lcssa = phi i32 [ %.1., %.lr.ph ]
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %5
  %.1.lcssa = phi i32 [ %.014, %5 ], [ %.1..lcssa, %._crit_edge.loopexit ]
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %17 = tail call i32 @getSize(%struct.GraphObj* %0)
  %18 = sext i32 %17 to i64
  %19 = icmp slt i64 %indvars.iv, %18
  br i1 %19, label %5, label %._crit_edge17.loopexit

._crit_edge17.loopexit:                           ; preds = %._crit_edge
  %.1.lcssa.lcssa = phi i32 [ %.1.lcssa, %._crit_edge ]
  br label %._crit_edge17

._crit_edge17:                                    ; preds = %._crit_edge17.loopexit, %1
  %.0.lcssa = phi i32 [ 0, %1 ], [ %.1.lcssa.lcssa, %._crit_edge17.loopexit ]
  ret i32 %.0.lcssa
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @getParent(%struct.GraphObj* nocapture readonly, i32) local_unnamed_addr #5 {
  %3 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 7
  %4 = load i32, i32* %3, align 8
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %13, label %6

; <label>:6:                                      ; preds = %2
  %7 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 1
  %8 = load i32*, i32** %7, align 8
  %9 = add nsw i32 %1, -1
  %10 = sext i32 %9 to i64
  %11 = getelementptr inbounds i32, i32* %8, i64 %10
  %12 = load i32, i32* %11, align 4
  br label %13

; <label>:13:                                     ; preds = %2, %6
  %14 = phi i32 [ %12, %6 ], [ -3, %2 ]
  ret i32 %14
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @getDiscover(%struct.GraphObj* nocapture readonly, i32) local_unnamed_addr #5 {
  %3 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 7
  %4 = load i32, i32* %3, align 8
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %13, label %6

; <label>:6:                                      ; preds = %2
  %7 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 2
  %8 = load i32*, i32** %7, align 8
  %9 = add nsw i32 %1, -1
  %10 = sext i32 %9 to i64
  %11 = getelementptr inbounds i32, i32* %8, i64 %10
  %12 = load i32, i32* %11, align 4
  br label %13

; <label>:13:                                     ; preds = %2, %6
  %14 = phi i32 [ %12, %6 ], [ -3, %2 ]
  ret i32 %14
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @getFinish(%struct.GraphObj* nocapture readonly, i32) local_unnamed_addr #5 {
  %3 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 7
  %4 = load i32, i32* %3, align 8
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %13, label %6

; <label>:6:                                      ; preds = %2
  %7 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 3
  %8 = load i32*, i32** %7, align 8
  %9 = add nsw i32 %1, -1
  %10 = sext i32 %9 to i64
  %11 = getelementptr inbounds i32, i32* %8, i64 %10
  %12 = load i32, i32* %11, align 4
  br label %13

; <label>:13:                                     ; preds = %2, %6
  %14 = phi i32 [ %12, %6 ], [ -3, %2 ]
  ret i32 %14
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @getColor(%struct.GraphObj* nocapture readonly, i32) local_unnamed_addr #5 {
  %3 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 4
  %4 = load i32*, i32** %3, align 8
  %5 = add nsw i32 %1, -1
  %6 = sext i32 %5 to i64
  %7 = getelementptr inbounds i32, i32* %4, i64 %6
  %8 = load i32, i32* %7, align 4
  ret i32 %8
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @setParent(%struct.GraphObj* nocapture readonly, i32, i32) local_unnamed_addr #4 {
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 1
  %5 = load i32*, i32** %4, align 8
  %6 = add nsw i32 %1, -1
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds i32, i32* %5, i64 %7
  store i32 %2, i32* %8, align 4
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @setColor(%struct.GraphObj* nocapture readonly, i32, i32) local_unnamed_addr #4 {
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 4
  %5 = load i32*, i32** %4, align 8
  %6 = add nsw i32 %1, -1
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds i32, i32* %5, i64 %7
  store i32 %2, i32* %8, align 4
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @setDiscover(%struct.GraphObj* nocapture readonly, i32, i32) local_unnamed_addr #4 {
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 2
  %5 = load i32*, i32** %4, align 8
  %6 = add nsw i32 %1, -1
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds i32, i32* %5, i64 %7
  store i32 %2, i32* %8, align 4
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @setFinish(%struct.GraphObj* nocapture readonly, i32, i32) local_unnamed_addr #4 {
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 3
  %5 = load i32*, i32** %4, align 8
  %6 = add nsw i32 %1, -1
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds i32, i32* %5, i64 %7
  store i32 %2, i32* %8, align 4
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @addEdge(%struct.GraphObj* nocapture readonly, i32, i32) local_unnamed_addr #0 {
  tail call void @addArc(%struct.GraphObj* %0, i32 %1, i32 %2)
  tail call void @addArc(%struct.GraphObj* %0, i32 %2, i32 %1)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @addArc(%struct.GraphObj* nocapture readonly, i32, i32) local_unnamed_addr #0 {
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 0
  %5 = load %struct.ListObj**, %struct.ListObj*** %4, align 8
  %6 = add nsw i32 %1, -1
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %5, i64 %7
  %9 = load %struct.ListObj*, %struct.ListObj** %8, align 8
  %10 = tail call i32 @length(%struct.ListObj* %9)
  %11 = icmp eq i32 %10, 0
  br i1 %11, label %12, label %14

; <label>:12:                                     ; preds = %3
  %13 = add nsw i32 %2, -1
  tail call void @append(%struct.ListObj* %9, i32 %13)
  br label %25

; <label>:14:                                     ; preds = %3
  tail call void @moveFront(%struct.ListObj* %9)
  %15 = tail call i32 @index(%struct.ListObj* %9)
  %16 = icmp eq i32 %15, -1
  br i1 %16, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %14
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %21
  %17 = tail call i32 @get(%struct.ListObj* %9)
  %18 = icmp slt i32 %17, %2
  br i1 %18, label %21, label %19

; <label>:19:                                     ; preds = %.lr.ph
  %20 = add nsw i32 %2, -1
  tail call void @insertBefore(%struct.ListObj* %9, i32 %20)
  br label %25

; <label>:21:                                     ; preds = %.lr.ph
  tail call void @moveNext(%struct.ListObj* %9)
  %22 = tail call i32 @index(%struct.ListObj* %9)
  %23 = icmp eq i32 %22, -1
  br i1 %23, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %21
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %14
  %24 = add nsw i32 %2, -1
  tail call void @append(%struct.ListObj* %9, i32 %24)
  br label %25

; <label>:25:                                     ; preds = %._crit_edge, %19, %12
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @DFS(%struct.GraphObj* nocapture, %struct.ListObj* nocapture) local_unnamed_addr #0 {
  %3 = alloca i32, align 4
  %4 = alloca %struct.ListObj*, align 8
  store i32 0, i32* %3, align 4
  %5 = tail call i32 @getSize(%struct.GraphObj* %0)
  %6 = tail call %struct.ListObj* @newList()
  store %struct.ListObj* %6, %struct.ListObj** %4, align 8
  %7 = tail call i32 @length(%struct.ListObj* %1)
  %8 = icmp eq i32 %5, %7
  br i1 %8, label %12, label %9

; <label>:9:                                      ; preds = %2
  %10 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %11 = tail call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %10, i8* getelementptr inbounds ([51 x i8], [51 x i8]* @.str.11, i64 0, i64 0), i32 %5, i32 %7)
  tail call void @exit(i32 1) #7
  unreachable

; <label>:12:                                     ; preds = %2
  tail call void @resetDFS(%struct.GraphObj* %0)
  %13 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 7
  store i32 1, i32* %13, align 8
  tail call void @moveFront(%struct.ListObj* %1)
  %14 = icmp sgt i32 %5, 0
  br i1 %14, label %.lr.ph19.preheader, label %._crit_edge20

.lr.ph19.preheader:                               ; preds = %12
  br label %.lr.ph19

.lr.ph19:                                         ; preds = %.lr.ph19.preheader, %19
  %.017 = phi i32 [ %20, %19 ], [ 0, %.lr.ph19.preheader ]
  %15 = tail call i32 @get(%struct.ListObj* %1)
  tail call void @moveNext(%struct.ListObj* %1)
  %16 = tail call i32 @getColor(%struct.GraphObj* %0, i32 %15)
  %17 = icmp eq i32 %16, 1
  br i1 %17, label %18, label %19

; <label>:18:                                     ; preds = %.lr.ph19
  call void @DFS_visit(%struct.GraphObj* %0, %struct.ListObj* %6, i32 %15, i32* nonnull %3)
  br label %19

; <label>:19:                                     ; preds = %.lr.ph19, %18
  %20 = add nuw nsw i32 %.017, 1
  %exitcond = icmp eq i32 %20, %5
  br i1 %exitcond, label %._crit_edge20.loopexit, label %.lr.ph19

._crit_edge20.loopexit:                           ; preds = %19
  br label %._crit_edge20

._crit_edge20:                                    ; preds = %._crit_edge20.loopexit, %12
  tail call void @clear(%struct.ListObj* %1)
  tail call void @moveFront(%struct.ListObj* %6)
  %21 = tail call i32 @index(%struct.ListObj* %6)
  %22 = icmp eq i32 %21, -1
  br i1 %22, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %._crit_edge20
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %23 = tail call i32 @get(%struct.ListObj* %6)
  tail call void @append(%struct.ListObj* %1, i32 %23)
  tail call void @moveNext(%struct.ListObj* %6)
  %24 = tail call i32 @index(%struct.ListObj* %6)
  %25 = icmp eq i32 %24, -1
  br i1 %25, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %.lr.ph
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %._crit_edge20
  call void @freeList(%struct.ListObj** nonnull %4)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @DFS_visit(%struct.GraphObj* nocapture readonly, %struct.ListObj* nocapture, i32, i32* nocapture) local_unnamed_addr #0 {
  %5 = load i32, i32* %3, align 4
  %6 = add nsw i32 %5, 1
  store i32 %6, i32* %3, align 4
  tail call void @setDiscover(%struct.GraphObj* %0, i32 %2, i32 %6)
  tail call void @setColor(%struct.GraphObj* %0, i32 %2, i32 2)
  %7 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 0
  %8 = load %struct.ListObj**, %struct.ListObj*** %7, align 8
  %9 = add nsw i32 %2, -1
  %10 = sext i32 %9 to i64
  %11 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %8, i64 %10
  %12 = load %struct.ListObj*, %struct.ListObj** %11, align 8
  tail call void @moveFront(%struct.ListObj* %12)
  %13 = tail call i32 @index(%struct.ListObj* %12)
  %14 = icmp eq i32 %13, -1
  br i1 %14, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %4
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %20
  %15 = tail call i32 @get(%struct.ListObj* %12)
  %16 = add nsw i32 %15, 1
  %17 = tail call i32 @getColor(%struct.GraphObj* %0, i32 %16)
  %18 = icmp eq i32 %17, 1
  br i1 %18, label %19, label %20

; <label>:19:                                     ; preds = %.lr.ph
  tail call void @setParent(%struct.GraphObj* %0, i32 %16, i32 %2)
  tail call void @DFS_visit(%struct.GraphObj* %0, %struct.ListObj* %1, i32 %16, i32* %3)
  br label %20

; <label>:20:                                     ; preds = %19, %.lr.ph
  tail call void @moveNext(%struct.ListObj* %12)
  %21 = tail call i32 @index(%struct.ListObj* %12)
  %22 = icmp eq i32 %21, -1
  br i1 %22, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %20
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %4
  tail call void @setColor(%struct.GraphObj* %0, i32 %2, i32 3)
  %23 = load i32, i32* %3, align 4
  %24 = add nsw i32 %23, 1
  store i32 %24, i32* %3, align 4
  tail call void @setFinish(%struct.GraphObj* %0, i32 %2, i32 %24)
  tail call void @prepend(%struct.ListObj* %1, i32 %2)
  tail call void @moveNext(%struct.ListObj* %1)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define noalias %struct.GraphObj* @transpose(%struct.GraphObj* nocapture readonly) local_unnamed_addr #0 {
  %2 = tail call %struct.GraphObj* @copyGraph(%struct.GraphObj* %0)
  tail call void @makeNull(%struct.GraphObj* %2)
  %3 = tail call i32 @getSize(%struct.GraphObj* %2)
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %.lr.ph15, label %._crit_edge16

.lr.ph15:                                         ; preds = %1
  %5 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 0
  br label %6

; <label>:6:                                      ; preds = %.lr.ph15, %._crit_edge
  %indvars.iv = phi i64 [ 0, %.lr.ph15 ], [ %.pre, %._crit_edge ]
  %7 = load %struct.ListObj**, %struct.ListObj*** %5, align 8
  %8 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %7, i64 %indvars.iv
  %9 = load %struct.ListObj*, %struct.ListObj** %8, align 8
  tail call void @moveFront(%struct.ListObj* %9)
  %10 = tail call i32 @index(%struct.ListObj* %9)
  %11 = icmp eq i32 %10, -1
  %.pre = add nuw i64 %indvars.iv, 1
  br i1 %11, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %6
  %12 = trunc i64 %.pre to i32
  br label %13

; <label>:13:                                     ; preds = %.lr.ph, %13
  %14 = tail call i32 @get(%struct.ListObj* %9)
  %15 = add nsw i32 %14, 1
  tail call void @addArc(%struct.GraphObj* %2, i32 %15, i32 %12)
  tail call void @moveNext(%struct.ListObj* %9)
  %16 = tail call i32 @index(%struct.ListObj* %9)
  %17 = icmp eq i32 %16, -1
  br i1 %17, label %._crit_edge.loopexit, label %13

._crit_edge.loopexit:                             ; preds = %13
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %6
  %18 = tail call i32 @getSize(%struct.GraphObj* %2)
  %19 = sext i32 %18 to i64
  %20 = icmp slt i64 %.pre, %19
  br i1 %20, label %6, label %._crit_edge16.loopexit

._crit_edge16.loopexit:                           ; preds = %._crit_edge
  br label %._crit_edge16

._crit_edge16:                                    ; preds = %._crit_edge16.loopexit, %1
  ret %struct.GraphObj* %2
}

; Function Attrs: noinline nounwind ssp uwtable
define noalias %struct.GraphObj* @copyGraph(%struct.GraphObj* nocapture readonly) local_unnamed_addr #0 {
  %2 = tail call i32 @getSize(%struct.GraphObj* %0)
  %3 = tail call %struct.GraphObj* @newGraph(i32 %2)
  %4 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 5
  %5 = load i32, i32* %4, align 8
  %6 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %3, i64 0, i32 5
  store i32 %5, i32* %6, align 8
  %7 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 7
  %8 = load i32, i32* %7, align 8
  %9 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %3, i64 0, i32 7
  store i32 %8, i32* %9, align 8
  %10 = icmp sgt i32 %2, 0
  br i1 %10, label %.lr.ph33, label %._crit_edge34

.lr.ph33:                                         ; preds = %1
  %11 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 1
  %12 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %3, i64 0, i32 1
  %13 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 2
  %14 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %3, i64 0, i32 2
  %15 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 3
  %16 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %3, i64 0, i32 3
  %17 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %0, i64 0, i32 0
  %18 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %3, i64 0, i32 0
  %.pre = load i32*, i32** %12, align 8
  %.pre35 = load i32*, i32** %14, align 8
  %.pre36 = load i32*, i32** %16, align 8
  %.pre37 = load %struct.ListObj**, %struct.ListObj*** %18, align 8
  %wide.trip.count = zext i32 %2 to i64
  br label %19

; <label>:19:                                     ; preds = %._crit_edge, %.lr.ph33
  %indvars.iv = phi i64 [ 0, %.lr.ph33 ], [ %indvars.iv.next, %._crit_edge ]
  %20 = load i32*, i32** %11, align 8
  %21 = getelementptr inbounds i32, i32* %20, i64 %indvars.iv
  %22 = load i32, i32* %21, align 4
  %23 = getelementptr inbounds i32, i32* %.pre, i64 %indvars.iv
  store i32 %22, i32* %23, align 4
  %24 = load i32*, i32** %13, align 8
  %25 = getelementptr inbounds i32, i32* %24, i64 %indvars.iv
  %26 = load i32, i32* %25, align 4
  %27 = getelementptr inbounds i32, i32* %.pre35, i64 %indvars.iv
  store i32 %26, i32* %27, align 4
  %28 = load i32*, i32** %15, align 8
  %29 = getelementptr inbounds i32, i32* %28, i64 %indvars.iv
  %30 = load i32, i32* %29, align 4
  %31 = getelementptr inbounds i32, i32* %.pre36, i64 %indvars.iv
  store i32 %30, i32* %31, align 4
  %32 = load %struct.ListObj**, %struct.ListObj*** %17, align 8
  %33 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %32, i64 %indvars.iv
  %34 = load %struct.ListObj*, %struct.ListObj** %33, align 8
  %35 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %.pre37, i64 %indvars.iv
  %36 = load %struct.ListObj*, %struct.ListObj** %35, align 8
  tail call void @moveFront(%struct.ListObj* %34)
  %37 = tail call i32 @index(%struct.ListObj* %34)
  %38 = icmp eq i32 %37, -1
  br i1 %38, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %19
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %39 = tail call i32 @get(%struct.ListObj* %34)
  tail call void @append(%struct.ListObj* %36, i32 %39)
  tail call void @moveNext(%struct.ListObj* %34)
  %40 = tail call i32 @index(%struct.ListObj* %34)
  %41 = icmp eq i32 %40, -1
  br i1 %41, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %.lr.ph
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %19
  tail call void @moveFront(%struct.ListObj* %36)
  tail call void @moveFront(%struct.ListObj* %34)
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond, label %._crit_edge34.loopexit, label %19

._crit_edge34.loopexit:                           ; preds = %._crit_edge
  br label %._crit_edge34

._crit_edge34:                                    ; preds = %._crit_edge34.loopexit, %1
  ret %struct.GraphObj* %3
}

; Function Attrs: noinline nounwind ssp uwtable
define void @printGraph(%struct.__sFILE* nocapture, %struct.GraphObj* nocapture readonly) local_unnamed_addr #0 {
  tail call void @printGraph_(%struct.__sFILE* %0, %struct.GraphObj* %1, i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.1.16, i64 0, i64 0))
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @printGraph_(%struct.__sFILE* nocapture, %struct.GraphObj* nocapture readonly, i8*) local_unnamed_addr #0 {
  %4 = tail call i32 @getSize(%struct.GraphObj* %1)
  %5 = icmp sgt i32 %4, 0
  br i1 %5, label %.lr.ph19, label %._crit_edge

.lr.ph19:                                         ; preds = %3
  %6 = getelementptr inbounds %struct.GraphObj, %struct.GraphObj* %1, i64 0, i32 0
  br label %7

; <label>:7:                                      ; preds = %.lr.ph19, %.loopexit
  %indvars.iv = phi i64 [ 0, %.lr.ph19 ], [ %indvars.iv.next, %.loopexit ]
  %indvars.iv.next = add nuw i64 %indvars.iv, 1
  %8 = trunc i64 %indvars.iv.next to i32
  %9 = tail call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %0, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.2.17, i64 0, i64 0), i8* %2, i32 %8)
  %10 = load %struct.ListObj**, %struct.ListObj*** %6, align 8
  %11 = getelementptr inbounds %struct.ListObj*, %struct.ListObj** %10, i64 %indvars.iv
  %12 = load %struct.ListObj*, %struct.ListObj** %11, align 8
  %13 = tail call i32 @length(%struct.ListObj* %12)
  %14 = icmp eq i32 %13, 0
  br i1 %14, label %15, label %16

; <label>:15:                                     ; preds = %7
  %fputc17 = tail call i32 @fputc(i32 32, %struct.__sFILE* %0)
  br label %.loopexit

; <label>:16:                                     ; preds = %7
  tail call void @moveFront(%struct.ListObj* %12)
  %17 = tail call i32 @index(%struct.ListObj* %12)
  %18 = icmp eq i32 %17, -1
  br i1 %18, label %.loopexit, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %16
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %19 = tail call i32 @get(%struct.ListObj* %12)
  %20 = add nsw i32 %19, 1
  %21 = tail call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %0, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1.50, i64 0, i64 0), i32 %20)
  tail call void @moveNext(%struct.ListObj* %12)
  %22 = tail call i32 @index(%struct.ListObj* %12)
  %23 = icmp eq i32 %22, -1
  br i1 %23, label %.loopexit.loopexit, label %.lr.ph

.loopexit.loopexit:                               ; preds = %.lr.ph
  br label %.loopexit

.loopexit:                                        ; preds = %.loopexit.loopexit, %16, %15
  %fputc = tail call i32 @fputc(i32 10, %struct.__sFILE* %0)
  %24 = tail call i32 @getSize(%struct.GraphObj* %1)
  %25 = sext i32 %24 to i64
  %26 = icmp slt i64 %indvars.iv.next, %25
  br i1 %26, label %7, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %.loopexit
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %3
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define noalias %struct.ListObj* @newList() local_unnamed_addr #0 {
  %1 = tail call i8* @malloc(i64 16)
  %2 = bitcast i8* %1 to %struct.ListObj*
  %3 = getelementptr inbounds i8, i8* %1, i64 8
  %4 = bitcast i8* %3 to i32*
  store i32 -1, i32* %4, align 8
  %5 = bitcast i8* %1 to %struct.NodeObj**
  store %struct.NodeObj* null, %struct.NodeObj** %5, align 8
  ret %struct.ListObj* %2
}

; Function Attrs: noinline nounwind ssp uwtable
define void @freeList(%struct.ListObj** nocapture) local_unnamed_addr #0 {
  %2 = load %struct.ListObj*, %struct.ListObj** %0, align 8
  tail call void @clear(%struct.ListObj* %2)
  %3 = bitcast %struct.ListObj** %0 to i8**
  %4 = load i8*, i8** %3, align 8
  tail call void @free(i8* %4)
  store %struct.ListObj* null, %struct.ListObj** %0, align 8
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @clear(%struct.ListObj* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %4 = icmp eq %struct.NodeObj* %3, null
  br i1 %4, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %1
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.in89 = phi %struct.NodeObj* [ %7, %.lr.ph ], [ %3, %.lr.ph.preheader ]
  %5 = bitcast %struct.NodeObj* %.in89 to i8*
  %6 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.in89, i64 0, i32 1
  %7 = load %struct.NodeObj*, %struct.NodeObj** %6, align 8
  tail call void @free(i8* %5)
  %8 = icmp eq %struct.NodeObj* %7, null
  br i1 %8, label %._crit_edge.loopexit, label %.lr.ph

._crit_edge.loopexit:                             ; preds = %.lr.ph
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %1
  store %struct.NodeObj* null, %struct.NodeObj** %2, align 8
  %9 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  store i32 -1, i32* %9, align 8
  ret void
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @length(%struct.ListObj* nocapture readonly) local_unnamed_addr #5 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %4 = icmp eq %struct.NodeObj* %3, null
  br i1 %4, label %.loopexit, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %1
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.09 = phi i32 [ %7, %.lr.ph ], [ 0, %.lr.ph.preheader ]
  %.068 = phi %struct.NodeObj* [ %6, %.lr.ph ], [ %3, %.lr.ph.preheader ]
  %5 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.068, i64 0, i32 1
  %6 = load %struct.NodeObj*, %struct.NodeObj** %5, align 8
  %7 = add nuw nsw i32 %.09, 1
  %8 = icmp eq %struct.NodeObj* %6, null
  br i1 %8, label %.loopexit.loopexit, label %.lr.ph

.loopexit.loopexit:                               ; preds = %.lr.ph
  %.lcssa = phi i32 [ %7, %.lr.ph ]
  br label %.loopexit

.loopexit:                                        ; preds = %.loopexit.loopexit, %1
  %.07 = phi i32 [ 0, %1 ], [ %.lcssa, %.loopexit.loopexit ]
  ret i32 %.07
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @index(%struct.ListObj* nocapture readonly) local_unnamed_addr #5 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %3 = load i32, i32* %2, align 8
  ret i32 %3
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @front(%struct.ListObj* nocapture readonly) local_unnamed_addr #5 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %4 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %3, i64 0, i32 2
  %5 = load i32, i32* %4, align 8
  ret i32 %5
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @back(%struct.ListObj* nocapture readonly) local_unnamed_addr #5 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  br label %4

; <label>:4:                                      ; preds = %4, %1
  %.0 = phi %struct.NodeObj* [ %3, %1 ], [ %6, %4 ]
  %5 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0, i64 0, i32 1
  %6 = load %struct.NodeObj*, %struct.NodeObj** %5, align 8
  %7 = icmp eq %struct.NodeObj* %6, null
  br i1 %7, label %8, label %4

; <label>:8:                                      ; preds = %4
  %.0.lcssa = phi %struct.NodeObj* [ %.0, %4 ]
  %9 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0.lcssa, i64 0, i32 2
  %10 = load i32, i32* %9, align 8
  ret i32 %10
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @get(%struct.ListObj* nocapture readonly) local_unnamed_addr #5 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %4 = load i32, i32* %3, align 8
  %.034 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %5 = icmp sgt i32 %4, 0
  br i1 %5, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %1
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %.036 = phi %struct.NodeObj* [ %.03, %.lr.ph ], [ %.034, %.lr.ph.preheader ]
  %.05 = phi i32 [ %6, %.lr.ph ], [ %4, %.lr.ph.preheader ]
  %6 = add nsw i32 %.05, -1
  %7 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.036, i64 0, i32 1
  %.03 = load %struct.NodeObj*, %struct.NodeObj** %7, align 8
  %8 = icmp sgt i32 %.05, 1
  br i1 %8, label %.lr.ph, label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %.lr.ph
  %.03.lcssa12 = phi %struct.NodeObj* [ %.03, %.lr.ph ]
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %1
  %.03.lcssa = phi %struct.NodeObj* [ %.034, %1 ], [ %.03.lcssa12, %._crit_edge.loopexit ]
  %9 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.03.lcssa, i64 0, i32 2
  %10 = load i32, i32* %9, align 8
  ret i32 %10
}

; Function Attrs: noinline norecurse nounwind readonly ssp uwtable
define i32 @equals(%struct.ListObj* nocapture readonly, %struct.ListObj* nocapture readonly) local_unnamed_addr #5 {
  %3 = tail call i32 @length(%struct.ListObj* %0)
  %4 = tail call i32 @length(%struct.ListObj* %1)
  %5 = icmp eq i32 %3, %4
  br i1 %5, label %6, label %.loopexit

; <label>:6:                                      ; preds = %2
  %7 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %.0912 = load %struct.NodeObj*, %struct.NodeObj** %7, align 8
  %8 = icmp eq %struct.NodeObj* %.0912, null
  br i1 %8, label %.loopexit, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %6
  %9 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %1, i64 0, i32 0
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %15
  %.0914 = phi %struct.NodeObj* [ %.09, %15 ], [ %.0912, %.lr.ph.preheader ]
  %.013.in = phi %struct.NodeObj** [ %17, %15 ], [ %9, %.lr.ph.preheader ]
  %.013 = load %struct.NodeObj*, %struct.NodeObj** %.013.in, align 8
  %10 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0914, i64 0, i32 2
  %11 = load i32, i32* %10, align 8
  %12 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.013, i64 0, i32 2
  %13 = load i32, i32* %12, align 8
  %14 = icmp eq i32 %11, %13
  br i1 %14, label %15, label %.loopexit.loopexit

; <label>:15:                                     ; preds = %.lr.ph
  %16 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0914, i64 0, i32 1
  %17 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.013, i64 0, i32 1
  %.09 = load %struct.NodeObj*, %struct.NodeObj** %16, align 8
  %18 = icmp eq %struct.NodeObj* %.09, null
  br i1 %18, label %.loopexit.loopexit, label %.lr.ph

.loopexit.loopexit:                               ; preds = %15, %.lr.ph
  %.010.ph = phi i32 [ 0, %.lr.ph ], [ 1, %15 ]
  br label %.loopexit

.loopexit:                                        ; preds = %.loopexit.loopexit, %6, %2
  %.010 = phi i32 [ 0, %2 ], [ 1, %6 ], [ %.010.ph, %.loopexit.loopexit ]
  ret i32 %.010
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @moveFront(%struct.ListObj* nocapture) local_unnamed_addr #4 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %4 = icmp eq %struct.NodeObj* %3, null
  br i1 %4, label %7, label %5

; <label>:5:                                      ; preds = %1
  %6 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  store i32 0, i32* %6, align 8
  br label %7

; <label>:7:                                      ; preds = %1, %5
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @moveBack(%struct.ListObj* nocapture) local_unnamed_addr #4 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %4 = icmp eq %struct.NodeObj* %3, null
  br i1 %4, label %9, label %5

; <label>:5:                                      ; preds = %1
  %6 = tail call i32 @length(%struct.ListObj* nonnull %0)
  %7 = add nsw i32 %6, -1
  %8 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  store i32 %7, i32* %8, align 8
  br label %9

; <label>:9:                                      ; preds = %1, %5
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @movePrev(%struct.ListObj* nocapture) local_unnamed_addr #4 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %4 = icmp eq %struct.NodeObj* %3, null
  br i1 %4, label %11, label %5

; <label>:5:                                      ; preds = %1
  %6 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %7 = load i32, i32* %6, align 8
  %8 = icmp eq i32 %7, -1
  br i1 %8, label %11, label %9

; <label>:9:                                      ; preds = %5
  %10 = add nsw i32 %7, -1
  store i32 %10, i32* %6, align 8
  br label %11

; <label>:11:                                     ; preds = %1, %5, %9
  ret void
}

; Function Attrs: noinline norecurse nounwind ssp uwtable
define void @moveNext(%struct.ListObj* nocapture) local_unnamed_addr #4 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %3 = load %struct.NodeObj*, %struct.NodeObj** %2, align 8
  %4 = icmp eq %struct.NodeObj* %3, null
  br i1 %4, label %16, label %5

; <label>:5:                                      ; preds = %1
  %6 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %7 = load i32, i32* %6, align 8
  %8 = icmp eq i32 %7, -1
  br i1 %8, label %16, label %9

; <label>:9:                                      ; preds = %5
  %10 = tail call i32 @length(%struct.ListObj* nonnull %0)
  %11 = add nsw i32 %10, -1
  %12 = icmp eq i32 %7, %11
  br i1 %12, label %13, label %14

; <label>:13:                                     ; preds = %9
  store i32 -1, i32* %6, align 8
  br label %16

; <label>:14:                                     ; preds = %9
  %15 = add nsw i32 %7, 1
  store i32 %15, i32* %6, align 8
  br label %16

; <label>:16:                                     ; preds = %1, %5, %14, %13
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @prepend(%struct.ListObj* nocapture, i32) local_unnamed_addr #0 {
  %3 = tail call i8* @malloc(i64 24)
  %4 = bitcast i8* %3 to %struct.NodeObj**
  store %struct.NodeObj* null, %struct.NodeObj** %4, align 8
  %5 = bitcast %struct.ListObj* %0 to i64*
  %6 = load i64, i64* %5, align 8
  %7 = getelementptr inbounds i8, i8* %3, i64 8
  %8 = bitcast i8* %7 to i64*
  store i64 %6, i64* %8, align 8
  %9 = getelementptr inbounds i8, i8* %3, i64 16
  %10 = bitcast i8* %9 to i32*
  store i32 %1, i32* %10, align 8
  %11 = icmp eq i64 %6, 0
  br i1 %11, label %14, label %12

; <label>:12:                                     ; preds = %2
  %13 = inttoptr i64 %6 to i8**
  store i8* %3, i8** %13, align 8
  br label %14

; <label>:14:                                     ; preds = %2, %12
  %15 = bitcast %struct.ListObj* %0 to i8**
  store i8* %3, i8** %15, align 8
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @append(%struct.ListObj* nocapture, i32) local_unnamed_addr #0 {
  %3 = tail call i8* @malloc(i64 24)
  %4 = getelementptr inbounds i8, i8* %3, i64 16
  %5 = bitcast i8* %4 to i32*
  store i32 %1, i32* %5, align 8
  %6 = getelementptr inbounds i8, i8* %3, i64 8
  %7 = bitcast i8* %6 to %struct.NodeObj**
  store %struct.NodeObj* null, %struct.NodeObj** %7, align 8
  %8 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %9 = load %struct.NodeObj*, %struct.NodeObj** %8, align 8
  %10 = icmp eq %struct.NodeObj* %9, null
  br i1 %10, label %11, label %.preheader.preheader

.preheader.preheader:                             ; preds = %2
  br label %.preheader

; <label>:11:                                     ; preds = %2
  %12 = bitcast i8* %3 to %struct.NodeObj**
  store %struct.NodeObj* null, %struct.NodeObj** %12, align 8
  %13 = bitcast %struct.ListObj* %0 to i8**
  store i8* %3, i8** %13, align 8
  br label %20

.preheader:                                       ; preds = %.preheader.preheader, %.preheader
  %.0 = phi %struct.NodeObj* [ %15, %.preheader ], [ %9, %.preheader.preheader ]
  %14 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0, i64 0, i32 1
  %15 = load %struct.NodeObj*, %struct.NodeObj** %14, align 8
  %16 = icmp eq %struct.NodeObj* %15, null
  br i1 %16, label %17, label %.preheader

; <label>:17:                                     ; preds = %.preheader
  %.0.lcssa = phi %struct.NodeObj* [ %.0, %.preheader ]
  %.lcssa = phi %struct.NodeObj** [ %14, %.preheader ]
  %18 = bitcast %struct.NodeObj** %.lcssa to i8**
  store i8* %3, i8** %18, align 8
  %19 = bitcast i8* %3 to %struct.NodeObj**
  store %struct.NodeObj* %.0.lcssa, %struct.NodeObj** %19, align 8
  br label %20

; <label>:20:                                     ; preds = %17, %11
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @insertBefore(%struct.ListObj* nocapture, i32) local_unnamed_addr #0 {
  %3 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %.01920 = load %struct.NodeObj*, %struct.NodeObj** %3, align 8
  %4 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %5 = load i32, i32* %4, align 8
  %6 = icmp sgt i32 %5, 0
  br i1 %6, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %2
  %7 = add i32 %5, -1
  %xtraiter = and i32 %5, 7
  %lcmp.mod = icmp eq i32 %xtraiter, 0
  br i1 %lcmp.mod, label %.lr.ph.prol.loopexit, label %.lr.ph.prol.preheader

.lr.ph.prol.preheader:                            ; preds = %.lr.ph.preheader
  br label %.lr.ph.prol

.lr.ph.prol:                                      ; preds = %.lr.ph.prol, %.lr.ph.prol.preheader
  %.01922.prol = phi %struct.NodeObj* [ %.019.prol, %.lr.ph.prol ], [ %.01920, %.lr.ph.prol.preheader ]
  %.021.prol = phi i32 [ %8, %.lr.ph.prol ], [ 0, %.lr.ph.prol.preheader ]
  %prol.iter = phi i32 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.prol.preheader ]
  %8 = add nuw nsw i32 %.021.prol, 1
  %9 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.01922.prol, i64 0, i32 1
  %.019.prol = load %struct.NodeObj*, %struct.NodeObj** %9, align 8
  %prol.iter.sub = add i32 %prol.iter, -1
  %prol.iter.cmp = icmp eq i32 %prol.iter.sub, 0
  br i1 %prol.iter.cmp, label %.lr.ph.prol.loopexit.unr-lcssa, label %.lr.ph.prol, !llvm.loop !2

.lr.ph.prol.loopexit.unr-lcssa:                   ; preds = %.lr.ph.prol
  %.lcssa = phi i32 [ %8, %.lr.ph.prol ]
  %.019.prol.lcssa = phi %struct.NodeObj* [ %.019.prol, %.lr.ph.prol ]
  br label %.lr.ph.prol.loopexit

.lr.ph.prol.loopexit:                             ; preds = %.lr.ph.preheader, %.lr.ph.prol.loopexit.unr-lcssa
  %.01922.unr = phi %struct.NodeObj* [ %.01920, %.lr.ph.preheader ], [ %.019.prol.lcssa, %.lr.ph.prol.loopexit.unr-lcssa ]
  %.021.unr = phi i32 [ 0, %.lr.ph.preheader ], [ %.lcssa, %.lr.ph.prol.loopexit.unr-lcssa ]
  %.019.lcssa27.unr = phi %struct.NodeObj* [ undef, %.lr.ph.preheader ], [ %.019.prol.lcssa, %.lr.ph.prol.loopexit.unr-lcssa ]
  %10 = icmp ult i32 %7, 7
  br i1 %10, label %._crit_edge.loopexit, label %.lr.ph.preheader.new

.lr.ph.preheader.new:                             ; preds = %.lr.ph.prol.loopexit
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph, %.lr.ph.preheader.new
  %.01922 = phi %struct.NodeObj* [ %.01922.unr, %.lr.ph.preheader.new ], [ %.019.7, %.lr.ph ]
  %.021 = phi i32 [ %.021.unr, %.lr.ph.preheader.new ], [ %18, %.lr.ph ]
  %11 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.01922, i64 0, i32 1
  %.019 = load %struct.NodeObj*, %struct.NodeObj** %11, align 8
  %12 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019, i64 0, i32 1
  %.019.1 = load %struct.NodeObj*, %struct.NodeObj** %12, align 8
  %13 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019.1, i64 0, i32 1
  %.019.2 = load %struct.NodeObj*, %struct.NodeObj** %13, align 8
  %14 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019.2, i64 0, i32 1
  %.019.3 = load %struct.NodeObj*, %struct.NodeObj** %14, align 8
  %15 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019.3, i64 0, i32 1
  %.019.4 = load %struct.NodeObj*, %struct.NodeObj** %15, align 8
  %16 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019.4, i64 0, i32 1
  %.019.5 = load %struct.NodeObj*, %struct.NodeObj** %16, align 8
  %17 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019.5, i64 0, i32 1
  %.019.6 = load %struct.NodeObj*, %struct.NodeObj** %17, align 8
  %18 = add nsw i32 %.021, 8
  %19 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019.6, i64 0, i32 1
  %.019.7 = load %struct.NodeObj*, %struct.NodeObj** %19, align 8
  %20 = icmp slt i32 %18, %5
  br i1 %20, label %.lr.ph, label %._crit_edge.loopexit.unr-lcssa

._crit_edge.loopexit.unr-lcssa:                   ; preds = %.lr.ph
  %.019.7.lcssa = phi %struct.NodeObj* [ %.019.7, %.lr.ph ]
  br label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %.lr.ph.prol.loopexit, %._crit_edge.loopexit.unr-lcssa
  %.019.lcssa27 = phi %struct.NodeObj* [ %.019.lcssa27.unr, %.lr.ph.prol.loopexit ], [ %.019.7.lcssa, %._crit_edge.loopexit.unr-lcssa ]
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %2
  %.019.lcssa = phi %struct.NodeObj* [ %.01920, %2 ], [ %.019.lcssa27, %._crit_edge.loopexit ]
  %21 = tail call i8* @malloc(i64 24)
  %22 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.019.lcssa, i64 0, i32 0
  %23 = bitcast %struct.NodeObj* %.019.lcssa to i64*
  %24 = load i64, i64* %23, align 8
  %25 = bitcast i8* %21 to i64*
  store i64 %24, i64* %25, align 8
  %26 = getelementptr inbounds i8, i8* %21, i64 8
  %27 = bitcast i8* %26 to %struct.NodeObj**
  store %struct.NodeObj* %.019.lcssa, %struct.NodeObj** %27, align 8
  %28 = getelementptr inbounds i8, i8* %21, i64 16
  %29 = bitcast i8* %28 to i32*
  store i32 %1, i32* %29, align 8
  %30 = load %struct.NodeObj*, %struct.NodeObj** %22, align 8
  %31 = icmp eq %struct.NodeObj* %30, null
  br i1 %31, label %35, label %32

; <label>:32:                                     ; preds = %._crit_edge
  %33 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %30, i64 0, i32 1
  %34 = bitcast %struct.NodeObj** %33 to i8**
  store i8* %21, i8** %34, align 8
  br label %35

; <label>:35:                                     ; preds = %._crit_edge, %32
  %36 = bitcast %struct.NodeObj* %.019.lcssa to i8**
  store i8* %21, i8** %36, align 8
  %37 = load %struct.NodeObj*, %struct.NodeObj** %3, align 8
  %38 = icmp eq %struct.NodeObj* %.019.lcssa, %37
  br i1 %38, label %39, label %41

; <label>:39:                                     ; preds = %35
  %40 = bitcast %struct.ListObj* %0 to i8**
  store i8* %21, i8** %40, align 8
  br label %41

; <label>:41:                                     ; preds = %39, %35
  %42 = load i32, i32* %4, align 8
  %43 = add nsw i32 %42, 1
  store i32 %43, i32* %4, align 8
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @insertAfter(%struct.ListObj* nocapture readonly, i32) local_unnamed_addr #0 {
  %3 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %.01415 = load %struct.NodeObj*, %struct.NodeObj** %3, align 8
  %4 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %5 = load i32, i32* %4, align 8
  %6 = icmp sgt i32 %5, 0
  br i1 %6, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %2
  %7 = add i32 %5, -1
  %xtraiter = and i32 %5, 7
  %lcmp.mod = icmp eq i32 %xtraiter, 0
  br i1 %lcmp.mod, label %.lr.ph.prol.loopexit, label %.lr.ph.prol.preheader

.lr.ph.prol.preheader:                            ; preds = %.lr.ph.preheader
  br label %.lr.ph.prol

.lr.ph.prol:                                      ; preds = %.lr.ph.prol, %.lr.ph.prol.preheader
  %.01417.prol = phi %struct.NodeObj* [ %.014.prol, %.lr.ph.prol ], [ %.01415, %.lr.ph.prol.preheader ]
  %.016.prol = phi i32 [ %8, %.lr.ph.prol ], [ 0, %.lr.ph.prol.preheader ]
  %prol.iter = phi i32 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.prol.preheader ]
  %8 = add nuw nsw i32 %.016.prol, 1
  %9 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.01417.prol, i64 0, i32 1
  %.014.prol = load %struct.NodeObj*, %struct.NodeObj** %9, align 8
  %prol.iter.sub = add i32 %prol.iter, -1
  %prol.iter.cmp = icmp eq i32 %prol.iter.sub, 0
  br i1 %prol.iter.cmp, label %.lr.ph.prol.loopexit.unr-lcssa, label %.lr.ph.prol, !llvm.loop !4

.lr.ph.prol.loopexit.unr-lcssa:                   ; preds = %.lr.ph.prol
  %.lcssa = phi i32 [ %8, %.lr.ph.prol ]
  %.014.prol.lcssa = phi %struct.NodeObj* [ %.014.prol, %.lr.ph.prol ]
  br label %.lr.ph.prol.loopexit

.lr.ph.prol.loopexit:                             ; preds = %.lr.ph.preheader, %.lr.ph.prol.loopexit.unr-lcssa
  %.01417.unr = phi %struct.NodeObj* [ %.01415, %.lr.ph.preheader ], [ %.014.prol.lcssa, %.lr.ph.prol.loopexit.unr-lcssa ]
  %.016.unr = phi i32 [ 0, %.lr.ph.preheader ], [ %.lcssa, %.lr.ph.prol.loopexit.unr-lcssa ]
  %.014.lcssa21.unr = phi %struct.NodeObj* [ undef, %.lr.ph.preheader ], [ %.014.prol.lcssa, %.lr.ph.prol.loopexit.unr-lcssa ]
  %10 = icmp ult i32 %7, 7
  br i1 %10, label %._crit_edge.loopexit, label %.lr.ph.preheader.new

.lr.ph.preheader.new:                             ; preds = %.lr.ph.prol.loopexit
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph, %.lr.ph.preheader.new
  %.01417 = phi %struct.NodeObj* [ %.01417.unr, %.lr.ph.preheader.new ], [ %.014.7, %.lr.ph ]
  %.016 = phi i32 [ %.016.unr, %.lr.ph.preheader.new ], [ %18, %.lr.ph ]
  %11 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.01417, i64 0, i32 1
  %.014 = load %struct.NodeObj*, %struct.NodeObj** %11, align 8
  %12 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014, i64 0, i32 1
  %.014.1 = load %struct.NodeObj*, %struct.NodeObj** %12, align 8
  %13 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014.1, i64 0, i32 1
  %.014.2 = load %struct.NodeObj*, %struct.NodeObj** %13, align 8
  %14 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014.2, i64 0, i32 1
  %.014.3 = load %struct.NodeObj*, %struct.NodeObj** %14, align 8
  %15 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014.3, i64 0, i32 1
  %.014.4 = load %struct.NodeObj*, %struct.NodeObj** %15, align 8
  %16 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014.4, i64 0, i32 1
  %.014.5 = load %struct.NodeObj*, %struct.NodeObj** %16, align 8
  %17 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014.5, i64 0, i32 1
  %.014.6 = load %struct.NodeObj*, %struct.NodeObj** %17, align 8
  %18 = add nsw i32 %.016, 8
  %19 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014.6, i64 0, i32 1
  %.014.7 = load %struct.NodeObj*, %struct.NodeObj** %19, align 8
  %20 = icmp slt i32 %18, %5
  br i1 %20, label %.lr.ph, label %._crit_edge.loopexit.unr-lcssa

._crit_edge.loopexit.unr-lcssa:                   ; preds = %.lr.ph
  %.014.7.lcssa = phi %struct.NodeObj* [ %.014.7, %.lr.ph ]
  br label %._crit_edge.loopexit

._crit_edge.loopexit:                             ; preds = %.lr.ph.prol.loopexit, %._crit_edge.loopexit.unr-lcssa
  %.014.lcssa21 = phi %struct.NodeObj* [ %.014.lcssa21.unr, %.lr.ph.prol.loopexit ], [ %.014.7.lcssa, %._crit_edge.loopexit.unr-lcssa ]
  br label %._crit_edge

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %2
  %.014.lcssa = phi %struct.NodeObj* [ %.01415, %2 ], [ %.014.lcssa21, %._crit_edge.loopexit ]
  %21 = tail call i8* @malloc(i64 24)
  %22 = bitcast i8* %21 to %struct.NodeObj**
  store %struct.NodeObj* %.014.lcssa, %struct.NodeObj** %22, align 8
  %23 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.014.lcssa, i64 0, i32 1
  %24 = bitcast %struct.NodeObj** %23 to i64*
  %25 = load i64, i64* %24, align 8
  %26 = getelementptr inbounds i8, i8* %21, i64 8
  %27 = bitcast i8* %26 to i64*
  store i64 %25, i64* %27, align 8
  %28 = getelementptr inbounds i8, i8* %21, i64 16
  %29 = bitcast i8* %28 to i32*
  store i32 %1, i32* %29, align 8
  %30 = load %struct.NodeObj*, %struct.NodeObj** %23, align 8
  %31 = icmp eq %struct.NodeObj* %30, null
  br i1 %31, label %34, label %32

; <label>:32:                                     ; preds = %._crit_edge
  %33 = bitcast %struct.NodeObj* %30 to i8**
  store i8* %21, i8** %33, align 8
  br label %34

; <label>:34:                                     ; preds = %._crit_edge, %32
  %35 = bitcast %struct.NodeObj** %23 to i8**
  store i8* %21, i8** %35, align 8
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @deleteFront(%struct.ListObj* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %3 = load i32, i32* %2, align 8
  %4 = add nsw i32 %3, -1
  store i32 %4, i32* %2, align 8
  %5 = tail call i32 @length(%struct.ListObj* %0)
  %6 = icmp eq i32 %5, 1
  %7 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %8 = load %struct.NodeObj*, %struct.NodeObj** %7, align 8
  br i1 %6, label %9, label %11

; <label>:9:                                      ; preds = %1
  %10 = bitcast %struct.NodeObj* %8 to i8*
  tail call void @free(i8* %10)
  br label %17

; <label>:11:                                     ; preds = %1
  %12 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %8, i64 0, i32 1
  %13 = load %struct.NodeObj*, %struct.NodeObj** %12, align 8
  %14 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %13, i64 0, i32 0
  store %struct.NodeObj* null, %struct.NodeObj** %14, align 8
  %15 = bitcast %struct.ListObj* %0 to i8**
  %16 = load i8*, i8** %15, align 8
  tail call void @free(i8* %16)
  br label %17

; <label>:17:                                     ; preds = %11, %9
  %.sink = phi %struct.NodeObj* [ %13, %11 ], [ null, %9 ]
  store %struct.NodeObj* %.sink, %struct.NodeObj** %7, align 8
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @deleteBack(%struct.ListObj* nocapture) local_unnamed_addr #0 {
  %2 = tail call i32 @length(%struct.ListObj* %0)
  %3 = icmp eq i32 %2, 1
  %4 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %5 = load %struct.NodeObj*, %struct.NodeObj** %4, align 8
  br i1 %3, label %6, label %.preheader.preheader

.preheader.preheader:                             ; preds = %1
  br label %.preheader

; <label>:6:                                      ; preds = %1
  %7 = bitcast %struct.NodeObj* %5 to i8*
  tail call void @free(i8* %7)
  store %struct.NodeObj* null, %struct.NodeObj** %4, align 8
  br label %21

.preheader:                                       ; preds = %.preheader.preheader, %.preheader
  %.0 = phi %struct.NodeObj* [ %9, %.preheader ], [ %5, %.preheader.preheader ]
  %8 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0, i64 0, i32 1
  %9 = load %struct.NodeObj*, %struct.NodeObj** %8, align 8
  %10 = icmp eq %struct.NodeObj* %9, null
  br i1 %10, label %11, label %.preheader

; <label>:11:                                     ; preds = %.preheader
  %.0.lcssa = phi %struct.NodeObj* [ %.0, %.preheader ]
  %12 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0.lcssa, i64 0, i32 0
  %13 = load %struct.NodeObj*, %struct.NodeObj** %12, align 8
  %14 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %13, i64 0, i32 1
  store %struct.NodeObj* null, %struct.NodeObj** %14, align 8
  %15 = bitcast %struct.NodeObj* %.0.lcssa to i8*
  tail call void @free(i8* %15)
  %16 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %17 = load i32, i32* %16, align 8
  %18 = tail call i32 @length(%struct.ListObj* %0)
  %19 = icmp eq i32 %17, %18
  br i1 %19, label %20, label %21

; <label>:20:                                     ; preds = %11
  store i32 -1, i32* %16, align 8
  br label %21

; <label>:21:                                     ; preds = %11, %20, %6
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @delete(%struct.ListObj* nocapture) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 1
  %3 = load i32, i32* %2, align 8
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %5, label %6

; <label>:5:                                      ; preds = %1
  tail call void @deleteFront(%struct.ListObj* nonnull %0)
  br label %27

; <label>:6:                                      ; preds = %1
  %7 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %8 = load %struct.NodeObj*, %struct.NodeObj** %7, align 8
  br label %9

; <label>:9:                                      ; preds = %9, %6
  %10 = phi i32 [ %11, %9 ], [ %3, %6 ]
  %.0 = phi %struct.NodeObj* [ %14, %9 ], [ %8, %6 ]
  %11 = add nsw i32 %10, -1
  store i32 %11, i32* %2, align 8
  %12 = icmp sgt i32 %10, 0
  %13 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0, i64 0, i32 1
  %14 = load %struct.NodeObj*, %struct.NodeObj** %13, align 8
  br i1 %12, label %9, label %15

; <label>:15:                                     ; preds = %9
  %.0.lcssa = phi %struct.NodeObj* [ %.0, %9 ]
  %.lcssa16 = phi %struct.NodeObj** [ %13, %9 ]
  %.lcssa = phi %struct.NodeObj* [ %14, %9 ]
  %16 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0.lcssa, i64 0, i32 0
  %17 = load %struct.NodeObj*, %struct.NodeObj** %16, align 8
  %18 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %17, i64 0, i32 1
  store %struct.NodeObj* %.lcssa, %struct.NodeObj** %18, align 8
  %19 = load %struct.NodeObj*, %struct.NodeObj** %.lcssa16, align 8
  %20 = icmp eq %struct.NodeObj* %19, null
  br i1 %20, label %25, label %21

; <label>:21:                                     ; preds = %15
  %22 = bitcast %struct.NodeObj* %.0.lcssa to i64*
  %23 = load i64, i64* %22, align 8
  %24 = bitcast %struct.NodeObj* %19 to i64*
  store i64 %23, i64* %24, align 8
  br label %25

; <label>:25:                                     ; preds = %15, %21
  %26 = bitcast %struct.NodeObj* %.0.lcssa to i8*
  tail call void @free(i8* %26)
  br label %27

; <label>:27:                                     ; preds = %25, %5
  store i32 -1, i32* %2, align 8
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define void @printList(%struct.__sFILE* nocapture, %struct.ListObj* nocapture readonly) local_unnamed_addr #0 {
  %3 = tail call i32 @length(%struct.ListObj* %1)
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %.loopexit, label %5

; <label>:5:                                      ; preds = %2
  %6 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %1, i64 0, i32 0
  %7 = load %struct.NodeObj*, %struct.NodeObj** %6, align 8
  %8 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %7, i64 0, i32 2
  %9 = load i32, i32* %8, align 8
  %10 = tail call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %0, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.49, i64 0, i64 0), i32 %9)
  %11 = load %struct.NodeObj*, %struct.NodeObj** %6, align 8
  %12 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %11, i64 0, i32 1
  %13 = load %struct.NodeObj*, %struct.NodeObj** %12, align 8
  %14 = icmp eq %struct.NodeObj* %13, null
  br i1 %14, label %.loopexit, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %5
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %15 = phi %struct.NodeObj* [ %20, %.lr.ph ], [ %13, %.lr.ph.preheader ]
  %16 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %15, i64 0, i32 2
  %17 = load i32, i32* %16, align 8
  %18 = tail call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %0, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1.50, i64 0, i64 0), i32 %17)
  %19 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %15, i64 0, i32 1
  %20 = load %struct.NodeObj*, %struct.NodeObj** %19, align 8
  %21 = icmp eq %struct.NodeObj* %20, null
  br i1 %21, label %.loopexit.loopexit, label %.lr.ph

.loopexit.loopexit:                               ; preds = %.lr.ph
  br label %.loopexit

.loopexit:                                        ; preds = %.loopexit.loopexit, %5, %2
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable
define noalias %struct.ListObj* @copyList(%struct.ListObj* nocapture readonly) local_unnamed_addr #0 {
  %2 = tail call %struct.ListObj* @newList()
  %3 = getelementptr inbounds %struct.ListObj, %struct.ListObj* %0, i64 0, i32 0
  %4 = load %struct.NodeObj*, %struct.NodeObj** %3, align 8
  %5 = icmp eq %struct.NodeObj* %4, null
  br i1 %5, label %.loopexit, label %.preheader.preheader

.preheader.preheader:                             ; preds = %1
  br label %.preheader

.preheader:                                       ; preds = %.preheader.preheader, %.preheader
  %.0 = phi %struct.NodeObj* [ %9, %.preheader ], [ %4, %.preheader.preheader ]
  %6 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0, i64 0, i32 2
  %7 = load i32, i32* %6, align 8
  tail call void @append(%struct.ListObj* %2, i32 %7)
  %8 = getelementptr inbounds %struct.NodeObj, %struct.NodeObj* %.0, i64 0, i32 1
  %9 = load %struct.NodeObj*, %struct.NodeObj** %8, align 8
  %10 = icmp eq %struct.NodeObj* %9, null
  br i1 %10, label %.loopexit.loopexit, label %.preheader

.loopexit.loopexit:                               ; preds = %.preheader
  br label %.loopexit

.loopexit:                                        ; preds = %.loopexit.loopexit, %1
  ret %struct.ListObj* %2
}

; Function Attrs: nounwind
declare i64 @fwrite(i8* nocapture, i64, i64, %struct.__sFILE* nocapture) #6

; Function Attrs: nounwind
declare i32 @fputc(i32, %struct.__sFILE* nocapture) #6

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noinline norecurse nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noinline norecurse nounwind readonly ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nounwind }
attributes #7 = { noreturn nounwind }

!llvm.ident = !{!0, !0, !0}
!llvm.module.flags = !{!1}

!0 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!1 = !{i32 1, !"PIC Level", i32 2}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.unroll.disable"}
!4 = distinct !{!4, !3}
