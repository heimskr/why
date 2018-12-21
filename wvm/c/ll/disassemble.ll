; ModuleID = 'disassemble.c'
source_filename = "disassemble.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@.str = private unnamed_addr constant [8 x i8] c"%sNOP%s\00", align 1
@.str.1 = private unnamed_addr constant [5 x i8] c"\1B[2m\00", align 1
@.str.2 = private unnamed_addr constant [5 x i8] c"\1B[0m\00", align 1
@.str.3 = private unnamed_addr constant [4 x i8] c"???\00", align 1
@.str.4 = private unnamed_addr constant [2 x i8] c"+\00", align 1
@.str.5 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@.str.6 = private unnamed_addr constant [22 x i8] c"%s$%s%s %s*%s %s$%s%s\00", align 1
@.str.7 = private unnamed_addr constant [12 x i8] c"\1B[38;5;226m\00", align 1
@.str.8 = private unnamed_addr constant [5 x i8] c"\1B[1m\00", align 1
@.str.9 = private unnamed_addr constant [3 x i8] c"<<\00", align 1
@.str.10 = private unnamed_addr constant [4 x i8] c">>>\00", align 1
@.str.11 = private unnamed_addr constant [3 x i8] c">>\00", align 1
@.str.12 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@.str.13 = private unnamed_addr constant [2 x i8] c"&\00", align 1
@.str.14 = private unnamed_addr constant [3 x i8] c"~&\00", align 1
@.str.15 = private unnamed_addr constant [3 x i8] c"~|\00", align 1
@.str.16 = private unnamed_addr constant [3 x i8] c"~x\00", align 1
@.str.17 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@.str.18 = private unnamed_addr constant [3 x i8] c"&&\00", align 1
@.str.19 = private unnamed_addr constant [4 x i8] c"~&&\00", align 1
@.str.20 = private unnamed_addr constant [4 x i8] c"~||\00", align 1
@.str.21 = private unnamed_addr constant [4 x i8] c"~xx\00", align 1
@.str.22 = private unnamed_addr constant [3 x i8] c"xx\00", align 1
@.str.23 = private unnamed_addr constant [23 x i8] c"%s$%s%s %s->%s %s$%s%s\00", align 1
@.str.24 = private unnamed_addr constant [2 x i8] c"|\00", align 1
@.str.25 = private unnamed_addr constant [3 x i8] c"||\00", align 1
@.str.26 = private unnamed_addr constant [28 x i8] c"%s~%s%s$%s%s %s->%s %s$%s%s\00", align 1
@.str.27 = private unnamed_addr constant [13 x i8] c"%s!%s%s$%s%s\00", align 1
@.str.28 = private unnamed_addr constant [28 x i8] c"%s!%s%s$%s%s %s->%s %s$%s%s\00", align 1
@.str.29 = private unnamed_addr constant [11 x i8] c"%s$%s%s %s\00", align 1
@.str.30 = private unnamed_addr constant [2 x i8] c"<\00", align 1
@.str.31 = private unnamed_addr constant [3 x i8] c"<=\00", align 1
@.str.32 = private unnamed_addr constant [3 x i8] c"==\00", align 1
@.str.33 = private unnamed_addr constant [26 x i8] c"%s %s$%s%s %s->%s %s$%s%s\00", align 1
@.str.34 = private unnamed_addr constant [4 x i8] c" /u\00", align 1
@.str.35 = private unnamed_addr constant [4 x i8] c"%s:\00", align 1
@.str.36 = private unnamed_addr constant [2 x i8] c":\00", align 1
@.str.37 = private unnamed_addr constant [13 x i8] c"%s%s %s$%s%s\00", align 1
@.str.38 = private unnamed_addr constant [16 x i8] c" %sif%s %s$%s%s\00", align 1
@.str.39 = private unnamed_addr constant [6 x i8] c"\1B[31m\00", align 1
@.str.40 = private unnamed_addr constant [29 x i8] c"[%s$%s%s] %s->%s [%s$%s%s]%s\00", align 1
@.str.41 = private unnamed_addr constant [4 x i8] c" /b\00", align 1
@.str.42 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@.str.43 = private unnamed_addr constant [27 x i8] c"[%s$%s%s] %s->%s %s$%s%s%s\00", align 1
@.str.44 = private unnamed_addr constant [27 x i8] c"%s$%s%s %s->%s [%s$%s%s]%s\00", align 1
@.str.45 = private unnamed_addr constant [10 x i8] c"[ %s$%s%s\00", align 1
@.str.46 = private unnamed_addr constant [12 x i8] c"  %s$%s%s ]\00", align 1
@.str.47 = private unnamed_addr constant [4 x i8] c"<%s\00", align 1
@.str.48 = private unnamed_addr constant [6 x i8] c"\1B[36m\00", align 1
@.str.49 = private unnamed_addr constant [14 x i8] c"print%s %s$%s\00", align 1
@.str.50 = private unnamed_addr constant [5 x i8] c"halt\00", align 1
@.str.51 = private unnamed_addr constant [5 x i8] c"eval\00", align 1
@.str.52 = private unnamed_addr constant [12 x i8] c"prc%s %s$%s\00", align 1
@.str.53 = private unnamed_addr constant [12 x i8] c"prd%s %s$%s\00", align 1
@.str.54 = private unnamed_addr constant [12 x i8] c"prx%s %s$%s\00", align 1
@.str.55 = private unnamed_addr constant [4 x i8] c"%s>\00", align 1
@.str.56 = private unnamed_addr constant [21 x i8] c"%s(Unknown R-type)%s\00", align 1
@.str.57 = private unnamed_addr constant [24 x i8] c"%s$%s%s %s%s=%s %s$%s%s\00", align 1
@.str.58 = private unnamed_addr constant [38 x i8] c"%s$%s%s %s%s%s %s$%s%s %s->%s %s$%s%s\00", align 1
@.str.59 = private unnamed_addr constant [21 x i8] c"%s$%s%s %s*%s %s%d%s\00", align 1
@.str.60 = private unnamed_addr constant [12 x i8] c"\1B[38;5;164m\00", align 1
@.str.61 = private unnamed_addr constant [31 x i8] c"%slui:%s %s%d%s %s->%s %s$%s%s\00", align 1
@.str.62 = private unnamed_addr constant [24 x i8] c"[%s%d%s] %s->%s %s$%s%s\00", align 1
@.str.63 = private unnamed_addr constant [26 x i8] c"[%s%d%s] %s->%s [%s$%s%s]\00", align 1
@.str.64 = private unnamed_addr constant [24 x i8] c"%s$%s%s %s->%s [%s%d%s]\00", align 1
@.str.65 = private unnamed_addr constant [22 x i8] c"%s%d%s %s->%s %s$%s%s\00", align 1
@.str.66 = private unnamed_addr constant [2 x i8] c">\00", align 1
@.str.67 = private unnamed_addr constant [3 x i8] c">=\00", align 1
@.str.68 = private unnamed_addr constant [21 x i8] c"%s(Unknown I-type)%s\00", align 1
@.str.69 = private unnamed_addr constant [37 x i8] c"%s$%s%s %s%s%s %s%d%s %s->%s %s$%s%s\00", align 1
@.str.70 = private unnamed_addr constant [24 x i8] c"%s$%s%s %s%s%s%s %s%d%s\00", align 1
@.str.71 = private unnamed_addr constant [2 x i8] c"=\00", align 1
@.str.72 = private unnamed_addr constant [16 x i8] c" %s->%s %s$%s%s\00", align 1
@.str.73 = private unnamed_addr constant [14 x i8] c"%s$%s%s%c%c%s\00", align 1
@.str.74 = private unnamed_addr constant [23 x i8] c"%s$%s%s %s%c=%s %s%d%s\00", align 1
@.str.75 = private unnamed_addr constant [37 x i8] c"%s$%s%s %s%c%s %s%d%s %s->%s %s$%s%s\00", align 1
@.str.76 = private unnamed_addr constant [8 x i8] c" %s%d%s\00", align 1
@.str.77 = private unnamed_addr constant [11 x i8] c"\1B[38;5;90m\00", align 1
@memory = common local_unnamed_addr global i8* null, align 8
@memsize = common local_unnamed_addr global i64 0, align 8
@pc = common local_unnamed_addr global i64 0, align 8
@offset_symtab = common local_unnamed_addr global i64 0, align 8
@offset_handlers = common local_unnamed_addr global i64 0, align 8
@offset_data = common local_unnamed_addr global i64 0, align 8
@offset_code = common local_unnamed_addr global i64 0, align 8
@offset_end = common local_unnamed_addr global i64 0, align 8
@registers = common local_unnamed_addr global [128 x i64] zeroinitializer, align 16
@membytes = common local_unnamed_addr global i64 0, align 8
@cycles = common local_unnamed_addr global i32 0, align 4
@alive = common local_unnamed_addr global i8 0, align 1

; Function Attrs: nounwind ssp uwtable
define i8* @wvm_disassemble(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i16 @wvm_get_opcode(i64 %0) #7
  %3 = tail call i8* @calloc(i64 255, i64 1) #8
  %4 = icmp eq i64 %0, 0
  br i1 %4, label %5, label %7

; <label>:5:                                      ; preds = %1
  %6 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %3, i32 0, i64 255, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %14

; <label>:7:                                      ; preds = %1
  %8 = tail call i32 @wvm_get_type(i16 zeroext %2) #7
  switch i32 %8, label %12 [
    i32 2, label %9
    i32 1, label %10
    i32 0, label %11
  ]

; <label>:9:                                      ; preds = %7
  tail call void @wvm_disassemble_r(i8* %3, i64 %0)
  br label %14

; <label>:10:                                     ; preds = %7
  tail call void @wvm_disassemble_i(i8* %3, i64 %0)
  br label %14

; <label>:11:                                     ; preds = %7
  tail call void @wvm_disassemble_j(i8* %3, i64 %0)
  br label %14

; <label>:12:                                     ; preds = %7
  %13 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %3, i32 0, i64 255, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.3, i64 0, i64 0)) #7
  br label %14

; <label>:14:                                     ; preds = %9, %11, %12, %10, %5
  ret i8* %3
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #1

declare zeroext i16 @wvm_get_opcode(i64) local_unnamed_addr #2

; Function Attrs: nounwind allocsize(0,1)
declare noalias i8* @calloc(i64, i64) local_unnamed_addr #3

declare i32 @__sprintf_chk(i8*, i32, i64, i8*, ...) local_unnamed_addr #2

; Function Attrs: nounwind readnone
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1) #4

declare i32 @wvm_get_type(i16 zeroext) local_unnamed_addr #2

; Function Attrs: nounwind ssp uwtable
define void @wvm_disassemble_r(i8*, i64) local_unnamed_addr #0 {
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  %6 = tail call zeroext i16 @wvm_get_opcode(i64 %1) #7
  %7 = tail call i32 @wvm_r_func(i64 %1) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %5) #7
  call void @wvm_r_regs(i64 %1, i8* nonnull %3, i8* nonnull %4, i8* nonnull %5) #7
  %8 = load i8, i8* %3, align 1, !tbaa !2
  %9 = call i8* @wvm_decode_reg(i8 zeroext %8) #7
  %10 = load i8, i8* %4, align 1, !tbaa !2
  %11 = call i8* @wvm_decode_reg(i8 zeroext %10) #7
  %12 = load i8, i8* %5, align 1, !tbaa !2
  %13 = call i8* @wvm_decode_reg(i8 zeroext %12) #7
  switch i16 %6, label %164 [
    i16 1, label %17
    i16 2, label %33
    i16 14, label %71
    i16 17, label %93
    i16 18, label %108
    i16 31, label %14
  ]

; <label>:14:                                     ; preds = %2
  %15 = shl i32 %7, 24
  %16 = ashr exact i32 %15, 24
  br label %132

; <label>:17:                                     ; preds = %2
  %18 = shl i32 %7, 24
  %19 = ashr exact i32 %18, 24
  switch i32 %19, label %28 [
    i32 9, label %27
    i32 1, label %20
    i32 2, label %21
    i32 6, label %24
    i32 7, label %25
    i32 8, label %26
  ]

; <label>:20:                                     ; preds = %17
  br label %28

; <label>:21:                                     ; preds = %17
  %22 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %23 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %22, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.6, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:24:                                     ; preds = %17
  br label %28

; <label>:25:                                     ; preds = %17
  br label %28

; <label>:26:                                     ; preds = %17
  br label %28

; <label>:27:                                     ; preds = %17
  br label %28

; <label>:28:                                     ; preds = %17, %27, %26, %25, %24, %20
  %29 = phi i8* [ getelementptr inbounds ([2 x i8], [2 x i8]* @.str.12, i64 0, i64 0), %27 ], [ getelementptr inbounds ([3 x i8], [3 x i8]* @.str.11, i64 0, i64 0), %26 ], [ getelementptr inbounds ([4 x i8], [4 x i8]* @.str.10, i64 0, i64 0), %25 ], [ getelementptr inbounds ([3 x i8], [3 x i8]* @.str.9, i64 0, i64 0), %24 ], [ getelementptr inbounds ([2 x i8], [2 x i8]* @.str.5, i64 0, i64 0), %20 ], [ getelementptr inbounds ([2 x i8], [2 x i8]* @.str.4, i64 0, i64 0), %17 ]
  %30 = load i8, i8* %3, align 1, !tbaa !2
  %31 = load i8, i8* %4, align 1, !tbaa !2
  %32 = load i8, i8* %5, align 1, !tbaa !2
  call void @wvm_disassemble_r_alt_op(i8* %0, i8 zeroext %30, i8 zeroext %31, i8 zeroext %32, i8* nonnull %29)
  br label %167

; <label>:33:                                     ; preds = %2
  %34 = shl i32 %7, 24
  %35 = ashr exact i32 %34, 24
  switch i32 %35, label %66 [
    i32 11, label %57
    i32 1, label %36
    i32 2, label %37
    i32 5, label %38
    i32 6, label %39
    i32 8, label %40
    i32 9, label %41
    i32 10, label %42
    i32 13, label %43
    i32 14, label %44
    i32 4, label %45
    i32 12, label %45
    i32 3, label %54
  ]

; <label>:36:                                     ; preds = %33
  br label %66

; <label>:37:                                     ; preds = %33
  br label %66

; <label>:38:                                     ; preds = %33
  br label %66

; <label>:39:                                     ; preds = %33
  br label %66

; <label>:40:                                     ; preds = %33
  br label %66

; <label>:41:                                     ; preds = %33
  br label %66

; <label>:42:                                     ; preds = %33
  br label %66

; <label>:43:                                     ; preds = %33
  br label %66

; <label>:44:                                     ; preds = %33
  br label %66

; <label>:45:                                     ; preds = %33, %33
  %46 = load i8, i8* %3, align 1, !tbaa !2
  %47 = icmp eq i8 %46, 0
  br i1 %47, label %48, label %51

; <label>:48:                                     ; preds = %45
  %49 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %50 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %49, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.23, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:51:                                     ; preds = %45
  %52 = icmp eq i32 %35, 4
  %53 = select i1 %52, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.24, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.25, i64 0, i64 0)
  br label %66

; <label>:54:                                     ; preds = %33
  %55 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %56 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %55, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str.26, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:57:                                     ; preds = %33
  %58 = load i8, i8* %3, align 1, !tbaa !2
  %59 = load i8, i8* %5, align 1, !tbaa !2
  %60 = icmp eq i8 %58, %59
  %61 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  br i1 %60, label %62, label %64

; <label>:62:                                     ; preds = %57
  %63 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %61, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.27, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:64:                                     ; preds = %57
  %65 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %61, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str.28, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:66:                                     ; preds = %33, %51, %44, %43, %42, %41, %40, %39, %38, %37, %36
  %67 = phi i8* [ %53, %51 ], [ getelementptr inbounds ([3 x i8], [3 x i8]* @.str.22, i64 0, i64 0), %44 ], [ getelementptr inbounds ([4 x i8], [4 x i8]* @.str.21, i64 0, i64 0), %43 ], [ getelementptr inbounds ([4 x i8], [4 x i8]* @.str.20, i64 0, i64 0), %42 ], [ getelementptr inbounds ([4 x i8], [4 x i8]* @.str.19, i64 0, i64 0), %41 ], [ getelementptr inbounds ([3 x i8], [3 x i8]* @.str.18, i64 0, i64 0), %40 ], [ getelementptr inbounds ([2 x i8], [2 x i8]* @.str.17, i64 0, i64 0), %39 ], [ getelementptr inbounds ([3 x i8], [3 x i8]* @.str.16, i64 0, i64 0), %38 ], [ getelementptr inbounds ([3 x i8], [3 x i8]* @.str.15, i64 0, i64 0), %37 ], [ getelementptr inbounds ([3 x i8], [3 x i8]* @.str.14, i64 0, i64 0), %36 ], [ getelementptr inbounds ([2 x i8], [2 x i8]* @.str.13, i64 0, i64 0), %33 ]
  %68 = load i8, i8* %3, align 1, !tbaa !2
  %69 = load i8, i8* %4, align 1, !tbaa !2
  %70 = load i8, i8* %5, align 1, !tbaa !2
  call void @wvm_disassemble_r_alt_op(i8* %0, i8 zeroext %68, i8 zeroext %69, i8 zeroext %70, i8* %67)
  br label %167

; <label>:71:                                     ; preds = %2
  %72 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %73 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %72, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.29, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0)) #7
  %74 = shl i32 %7, 24
  %75 = ashr exact i32 %74, 24
  switch i32 %75, label %85 [
    i32 0, label %76
    i32 3, label %76
    i32 1, label %80
    i32 4, label %80
    i32 2, label %83
    i32 5, label %83
  ]

; <label>:76:                                     ; preds = %71, %71
  %77 = call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.30, i64 0, i64 0), i64 %72) #7
  switch i32 %75, label %85 [
    i32 1, label %78
    i32 2, label %83
  ]

; <label>:78:                                     ; preds = %76
  %79 = call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.31, i64 0, i64 0), i64 %72) #7
  br label %85

; <label>:80:                                     ; preds = %71, %71
  %81 = call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.31, i64 0, i64 0), i64 %72) #7
  %82 = icmp eq i32 %75, 2
  br i1 %82, label %83, label %85

; <label>:83:                                     ; preds = %80, %76, %71, %71
  %84 = call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.32, i64 0, i64 0), i64 %72) #7
  br label %85

; <label>:85:                                     ; preds = %78, %80, %71, %76, %83
  %86 = call i64 @strlen(i8* %0)
  %87 = getelementptr inbounds i8, i8* %0, i64 %86
  %88 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %87, i32 0, i64 -1, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.33, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  %89 = add nsw i32 %75, -3
  %90 = icmp ult i32 %89, 3
  br i1 %90, label %91, label %167

; <label>:91:                                     ; preds = %85
  %92 = call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.34, i64 0, i64 0), i64 %72) #7
  br label %167

; <label>:93:                                     ; preds = %2
  %94 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %95 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %94, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.35, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0)) #7
  %96 = shl i32 %7, 24
  %97 = ashr exact i32 %96, 24
  %98 = and i32 %97, -2
  %99 = icmp eq i32 %98, 2
  br i1 %99, label %100, label %102

; <label>:100:                                    ; preds = %93
  %101 = call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.36, i64 0, i64 0), i64 %94) #7
  br label %102

; <label>:102:                                    ; preds = %93, %100
  %103 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %94, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.37, i64 0, i64 0), i8* %0, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  switch i32 %97, label %167 [
    i32 1, label %104
    i32 3, label %104
  ]

; <label>:104:                                    ; preds = %102, %102
  %105 = call i64 @strlen(i8* %0)
  %106 = getelementptr inbounds i8, i8* %0, i64 %105
  %107 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %106, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.38, i64 0, i64 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.39, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:108:                                    ; preds = %2
  %109 = shl i32 %7, 24
  %110 = ashr exact i32 %109, 24
  switch i32 %110, label %132 [
    i32 0, label %111
    i32 3, label %111
    i32 1, label %116
    i32 4, label %116
    i32 2, label %121
    i32 5, label %121
    i32 6, label %126
    i32 7, label %129
  ]

; <label>:111:                                    ; preds = %108, %108
  %112 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %113 = icmp eq i32 %110, 3
  %114 = select i1 %113, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %115 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %112, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str.40, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* %114) #7
  br label %167

; <label>:116:                                    ; preds = %108, %108
  %117 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %118 = icmp eq i32 %110, 4
  %119 = select i1 %118, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %120 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %117, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.43, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* %119) #7
  br label %167

; <label>:121:                                    ; preds = %108, %108
  %122 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %123 = icmp eq i32 %110, 5
  %124 = select i1 %123, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %125 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %122, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.44, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* %124) #7
  br label %167

; <label>:126:                                    ; preds = %108
  %127 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %128 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %127, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.45, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:129:                                    ; preds = %108
  %130 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %131 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %130, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.46, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:132:                                    ; preds = %14, %108
  %133 = phi i32 [ %16, %14 ], [ %110, %108 ]
  %134 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %135 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %134, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.47, i64 0, i64 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i64 0, i64 0)) #7
  switch i32 %133, label %160 [
    i32 1, label %136
    i32 2, label %140
    i32 3, label %144
    i32 4, label %148
    i32 5, label %152
    i32 6, label %156
  ]

; <label>:136:                                    ; preds = %132
  %137 = call i64 @strlen(i8* %0)
  %138 = getelementptr inbounds i8, i8* %0, i64 %137
  %139 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %138, i32 0, i64 -1, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.49, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9) #7
  br label %160

; <label>:140:                                    ; preds = %132
  %141 = call i64 @strlen(i8* %0)
  %142 = getelementptr inbounds i8, i8* %0, i64 %141
  %143 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %142, i32 0, i64 -1, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.50, i64 0, i64 0)) #7
  br label %160

; <label>:144:                                    ; preds = %132
  %145 = call i64 @strlen(i8* %0)
  %146 = getelementptr inbounds i8, i8* %0, i64 %145
  %147 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %146, i32 0, i64 -1, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.51, i64 0, i64 0)) #7
  br label %160

; <label>:148:                                    ; preds = %132
  %149 = call i64 @strlen(i8* %0)
  %150 = getelementptr inbounds i8, i8* %0, i64 %149
  %151 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %150, i32 0, i64 -1, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.52, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9) #7
  br label %160

; <label>:152:                                    ; preds = %132
  %153 = call i64 @strlen(i8* %0)
  %154 = getelementptr inbounds i8, i8* %0, i64 %153
  %155 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %154, i32 0, i64 -1, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.53, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9) #7
  br label %160

; <label>:156:                                    ; preds = %132
  %157 = call i64 @strlen(i8* %0)
  %158 = getelementptr inbounds i8, i8* %0, i64 %157
  %159 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %158, i32 0, i64 -1, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.54, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %9) #7
  br label %160

; <label>:160:                                    ; preds = %132, %156, %152, %148, %144, %140, %136
  %161 = call i64 @strlen(i8* %0)
  %162 = getelementptr inbounds i8, i8* %0, i64 %161
  %163 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %162, i32 0, i64 -1, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.55, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:164:                                    ; preds = %2
  %165 = call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %166 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %165, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.56, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %167

; <label>:167:                                    ; preds = %85, %104, %102, %91, %62, %64, %164, %160, %129, %126, %121, %116, %111, %66, %54, %48, %28, %21
  call void @llvm.lifetime.end(i64 1, i8* nonnull %5) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @wvm_disassemble_i(i8*, i64) local_unnamed_addr #0 {
  %3 = tail call zeroext i16 @wvm_get_opcode(i64 %1) #7
  %4 = tail call zeroext i8 @wvm_i_rs(i64 %1) #7
  %5 = tail call zeroext i8 @wvm_i_rd(i64 %1) #7
  %6 = tail call i32 @wvm_i_imm(i64 %1) #7
  switch i16 %3, label %168 [
    i16 3, label %7
    i16 22, label %7
    i16 4, label %8
    i16 23, label %8
    i16 5, label %9
    i16 24, label %9
    i16 30, label %13
    i16 6, label %24
    i16 7, label %35
    i16 8, label %46
    i16 9, label %57
    i16 10, label %68
    i16 11, label %79
    i16 34, label %90
    i16 35, label %101
    i16 36, label %112
    i16 13, label %123
    i16 19, label %127
    i16 37, label %127
    i16 39, label %131
    i16 40, label %131
    i16 20, label %135
    i16 38, label %135
    i16 21, label %139
    i16 25, label %143
    i16 28, label %143
    i16 26, label %148
    i16 29, label %148
    i16 41, label %153
    i16 42, label %158
    i16 27, label %163
  ]

; <label>:7:                                      ; preds = %2, %2
  tail call void @wvm_disassemble_i_math(i8* %0, i8 zeroext %4, i8 zeroext %5, i32 %6, i8 signext 43)
  br label %171

; <label>:8:                                      ; preds = %2, %2
  tail call void @wvm_disassemble_i_math(i8* %0, i8 zeroext %4, i8 zeroext %5, i32 %6, i8 signext 45)
  br label %171

; <label>:9:                                      ; preds = %2, %2
  %10 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %11 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %12 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %10, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.59, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %171

; <label>:13:                                     ; preds = %2
  %14 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %15 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %16 = icmp eq i8 %4, %5
  %17 = select i1 %16, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %18 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %14, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %15, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.12, i64 0, i64 0), i8* %17, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %16, label %181, label %19

; <label>:19:                                     ; preds = %13
  %20 = tail call i64 @strlen(i8* %0) #7
  %21 = getelementptr inbounds i8, i8* %0, i64 %20
  %22 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %23 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %21, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %22, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:24:                                     ; preds = %2
  %25 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %26 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %27 = icmp eq i8 %4, %5
  %28 = select i1 %27, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %29 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %25, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %26, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.13, i64 0, i64 0), i8* %28, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %27, label %181, label %30

; <label>:30:                                     ; preds = %24
  %31 = tail call i64 @strlen(i8* %0) #7
  %32 = getelementptr inbounds i8, i8* %0, i64 %31
  %33 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %34 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %32, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %33, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:35:                                     ; preds = %2
  %36 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %37 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %38 = icmp eq i8 %4, %5
  %39 = select i1 %38, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %40 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %36, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %37, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.14, i64 0, i64 0), i8* %39, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %38, label %181, label %41

; <label>:41:                                     ; preds = %35
  %42 = tail call i64 @strlen(i8* %0) #7
  %43 = getelementptr inbounds i8, i8* %0, i64 %42
  %44 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %45 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %43, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %44, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:46:                                     ; preds = %2
  %47 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %48 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %49 = icmp eq i8 %4, %5
  %50 = select i1 %49, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %51 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %47, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %48, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.15, i64 0, i64 0), i8* %50, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %49, label %181, label %52

; <label>:52:                                     ; preds = %46
  %53 = tail call i64 @strlen(i8* %0) #7
  %54 = getelementptr inbounds i8, i8* %0, i64 %53
  %55 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %56 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %54, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %55, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:57:                                     ; preds = %2
  %58 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %59 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %60 = icmp eq i8 %4, %5
  %61 = select i1 %60, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %62 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %58, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %59, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.24, i64 0, i64 0), i8* %61, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %60, label %181, label %63

; <label>:63:                                     ; preds = %57
  %64 = tail call i64 @strlen(i8* %0) #7
  %65 = getelementptr inbounds i8, i8* %0, i64 %64
  %66 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %67 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %65, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %66, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:68:                                     ; preds = %2
  %69 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %70 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %71 = icmp eq i8 %4, %5
  %72 = select i1 %71, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %73 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %69, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %70, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.16, i64 0, i64 0), i8* %72, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %71, label %181, label %74

; <label>:74:                                     ; preds = %68
  %75 = tail call i64 @strlen(i8* %0) #7
  %76 = getelementptr inbounds i8, i8* %0, i64 %75
  %77 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %78 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %76, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %77, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:79:                                     ; preds = %2
  %80 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %81 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %82 = icmp eq i8 %4, %5
  %83 = select i1 %82, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %84 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %80, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %81, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.17, i64 0, i64 0), i8* %83, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %82, label %181, label %85

; <label>:85:                                     ; preds = %79
  %86 = tail call i64 @strlen(i8* %0) #7
  %87 = getelementptr inbounds i8, i8* %0, i64 %86
  %88 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %89 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %87, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %88, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:90:                                     ; preds = %2
  %91 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %92 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %93 = icmp eq i8 %4, %5
  %94 = select i1 %93, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %95 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %91, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %92, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.9, i64 0, i64 0), i8* %94, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %93, label %181, label %96

; <label>:96:                                     ; preds = %90
  %97 = tail call i64 @strlen(i8* %0) #7
  %98 = getelementptr inbounds i8, i8* %0, i64 %97
  %99 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %100 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %98, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %99, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:101:                                    ; preds = %2
  %102 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %103 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %104 = icmp eq i8 %4, %5
  %105 = select i1 %104, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %106 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %102, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %103, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.10, i64 0, i64 0), i8* %105, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %104, label %181, label %107

; <label>:107:                                    ; preds = %101
  %108 = tail call i64 @strlen(i8* %0) #7
  %109 = getelementptr inbounds i8, i8* %0, i64 %108
  %110 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %111 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %109, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %110, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:112:                                    ; preds = %2
  %113 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %114 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %115 = icmp eq i8 %4, %5
  %116 = select i1 %115, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %117 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %113, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %114, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.11, i64 0, i64 0), i8* %116, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %115, label %181, label %118

; <label>:118:                                    ; preds = %112
  %119 = tail call i64 @strlen(i8* %0) #7
  %120 = getelementptr inbounds i8, i8* %0, i64 %119
  %121 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %122 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %120, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %121, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:123:                                    ; preds = %2
  %124 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %125 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %126 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %124, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.61, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %125, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:127:                                    ; preds = %2, %2
  %128 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %129 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %130 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %128, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.62, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %129, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %171

; <label>:131:                                    ; preds = %2, %2
  %132 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %133 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %134 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %132, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.63, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %133, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %171

; <label>:135:                                    ; preds = %2, %2
  %136 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %137 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %138 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %136, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.64, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %137, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %171

; <label>:139:                                    ; preds = %2
  %140 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %141 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %142 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %140, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.65, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %141, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:143:                                    ; preds = %2, %2
  %144 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %145 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %146 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %147 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %144, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.69, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %145, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.30, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %146, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %171

; <label>:148:                                    ; preds = %2, %2
  %149 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %150 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %151 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %152 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %149, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.69, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %150, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.31, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %151, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %171

; <label>:153:                                    ; preds = %2
  %154 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %155 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %156 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %157 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %154, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.69, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %155, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.66, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %156, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:158:                                    ; preds = %2
  %159 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %160 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %161 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %162 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %159, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.69, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %160, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.67, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %161, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:163:                                    ; preds = %2
  %164 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false) #7
  %165 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %166 = tail call i8* @wvm_decode_reg(i8 zeroext %5) #7
  %167 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %164, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.69, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %165, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.32, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %166, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:168:                                    ; preds = %2
  %169 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %170 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %169, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.68, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %181

; <label>:171:                                    ; preds = %148, %143, %135, %131, %127, %9, %8, %7
  switch i16 %3, label %175 [
    i16 22, label %172
    i16 23, label %172
    i16 24, label %172
    i16 28, label %172
    i16 29, label %172
  ]

; <label>:172:                                    ; preds = %171, %171, %171, %171, %171
  %173 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %174 = tail call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.34, i64 0, i64 0), i64 %173) #7
  br label %181

; <label>:175:                                    ; preds = %171
  %176 = add i16 %3, -37
  %177 = icmp ult i16 %176, 2
  br i1 %177, label %178, label %181

; <label>:178:                                    ; preds = %175
  %179 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %180 = tail call i8* @__strcat_chk(i8* %0, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i64 0, i64 0), i64 %179) #7
  br label %181

; <label>:181:                                    ; preds = %118, %112, %107, %101, %96, %90, %85, %79, %74, %68, %63, %57, %52, %46, %41, %35, %30, %24, %19, %13, %163, %158, %153, %139, %123, %175, %172, %178, %168
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @wvm_disassemble_j(i8*, i64) local_unnamed_addr #0 {
  %3 = tail call zeroext i16 @wvm_get_opcode(i64 %1) #7
  %4 = tail call zeroext i8 @wvm_j_rs(i64 %1) #7
  %5 = tail call i32 @wvm_j_addr(i64 %1) #7
  %6 = tail call i8* @wvm_decode_reg(i8 zeroext %4) #7
  %7 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %8 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %7, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.35, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0)) #7
  %9 = and i16 %3, -2
  %10 = icmp eq i16 %9, 32
  br i1 %10, label %11, label %13

; <label>:11:                                     ; preds = %2
  %12 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %7, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.35, i64 0, i64 0), i8* %0) #7
  br label %13

; <label>:13:                                     ; preds = %2, %11
  %14 = tail call i64 @strlen(i8* %0)
  %15 = getelementptr inbounds i8, i8* %0, i64 %14
  %16 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %15, i32 0, i64 -1, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.76, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %5, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  switch i16 %3, label %21 [
    i16 16, label %17
    i16 33, label %17
  ]

; <label>:17:                                     ; preds = %13, %13
  %18 = tail call i64 @strlen(i8* %0)
  %19 = getelementptr inbounds i8, i8* %0, i64 %18
  %20 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %19, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.38, i64 0, i64 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.77, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %21

; <label>:21:                                     ; preds = %13, %17
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #1

declare i32 @wvm_r_func(i64) local_unnamed_addr #2

declare void @wvm_r_regs(i64, i8*, i8*, i8*) local_unnamed_addr #2

declare i8* @wvm_decode_reg(i8 zeroext) local_unnamed_addr #2

; Function Attrs: nounwind ssp uwtable
define void @wvm_disassemble_r_alt_op(i8*, i8 zeroext, i8 zeroext, i8 zeroext, i8*) local_unnamed_addr #0 {
  %6 = tail call i8* @wvm_decode_reg(i8 zeroext %1) #7
  %7 = tail call i8* @wvm_decode_reg(i8 zeroext %2) #7
  %8 = tail call i8* @wvm_decode_reg(i8 zeroext %3) #7
  %9 = icmp eq i8 %1, %3
  %10 = icmp eq i8 %2, %3
  %11 = or i1 %9, %10
  br i1 %11, label %12, label %17

; <label>:12:                                     ; preds = %5
  %13 = select i1 %9, i8* %6, i8* %7
  %14 = select i1 %9, i8* %7, i8* %6
  %15 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %16 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %15, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.57, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %13, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* %4, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %14, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %20

; <label>:17:                                     ; preds = %5
  %18 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %19 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %18, i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str.58, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* %4, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %8, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %20

; <label>:20:                                     ; preds = %17, %12
  ret void
}

; Function Attrs: nounwind
declare i8* @__strcat_chk(i8*, i8*, i64) local_unnamed_addr #5

; Function Attrs: nounwind readonly
declare i64 @strlen(i8* nocapture) local_unnamed_addr #6

declare zeroext i8 @wvm_i_rs(i64) local_unnamed_addr #2

declare zeroext i8 @wvm_i_rd(i64) local_unnamed_addr #2

declare i32 @wvm_i_imm(i64) local_unnamed_addr #2

; Function Attrs: nounwind ssp uwtable
define void @wvm_disassemble_i_math(i8*, i8 zeroext, i8 zeroext, i32, i8 signext) local_unnamed_addr #0 {
  %6 = icmp eq i8 %1, %2
  br i1 %6, label %7, label %16

; <label>:7:                                      ; preds = %5
  %8 = icmp eq i32 %3, 1
  %9 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %10 = tail call i8* @wvm_decode_reg(i8 zeroext %1) #7
  %11 = sext i8 %4 to i32
  br i1 %8, label %12, label %14

; <label>:12:                                     ; preds = %7
  %13 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %9, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.73, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %10, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i32 %11, i32 %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %22

; <label>:14:                                     ; preds = %7
  %15 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %9, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.74, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %10, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i32 %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %3, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %22

; <label>:16:                                     ; preds = %5
  %17 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %18 = tail call i8* @wvm_decode_reg(i8 zeroext %1) #7
  %19 = sext i8 %4 to i32
  %20 = tail call i8* @wvm_decode_reg(i8 zeroext %2) #7
  %21 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %17, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.75, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %18, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i32 %19, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %3, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %20, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %22

; <label>:22:                                     ; preds = %12, %14, %16
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @wvm_disassemble_i_alt_op(i8*, i8 zeroext, i8 zeroext, i32, i8*) local_unnamed_addr #0 {
  %6 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %7 = tail call i8* @wvm_decode_reg(i8 zeroext %1) #7
  %8 = icmp eq i8 %1, %2
  %9 = select i1 %8, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i64 0, i64 0)
  %10 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %6, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* %4, i8* %9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %3, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br i1 %8, label %16, label %11

; <label>:11:                                     ; preds = %5
  %12 = tail call i64 @strlen(i8* %0)
  %13 = getelementptr inbounds i8, i8* %0, i64 %12
  %14 = tail call i8* @wvm_decode_reg(i8 zeroext %2) #7
  %15 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %13, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %14, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  br label %16

; <label>:16:                                     ; preds = %5, %11
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @wvm_disassemble_i_comp(i8*, i8 zeroext, i8 zeroext, i32, i8*) local_unnamed_addr #0 {
  %6 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %0, i1 false)
  %7 = tail call i8* @wvm_decode_reg(i8 zeroext %1) #7
  %8 = tail call i8* @wvm_decode_reg(i8 zeroext %2) #7
  %9 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %0, i32 0, i64 %6, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.69, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i64 0, i64 0), i8* %4, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.60, i64 0, i64 0), i32 %3, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i64 0, i64 0), i8* %8, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0)) #7
  ret void
}

declare zeroext i8 @wvm_j_rs(i64) local_unnamed_addr #2

declare i32 @wvm_j_addr(i64) local_unnamed_addr #2

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind readnone }
attributes #5 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nounwind readonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nounwind }
attributes #8 = { allocsize(0,1) }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
