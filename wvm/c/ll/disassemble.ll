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
@.str.55 = private unnamed_addr constant [14 x i8] c"sleep%s %s$%s\00", align 1
@.str.56 = private unnamed_addr constant [23 x i8] c"%s(Unknown external)%s\00", align 1
@.str.57 = private unnamed_addr constant [4 x i8] c"%s>\00", align 1
@.str.58 = private unnamed_addr constant [17 x i8] c"%stime %s%s$%s%s\00", align 1
@.str.59 = private unnamed_addr constant [17 x i8] c"%sring %s%s$%s%s\00", align 1
@.str.60 = private unnamed_addr constant [21 x i8] c"%s(Unknown R-type)%s\00", align 1
@.str.61 = private unnamed_addr constant [24 x i8] c"%s$%s%s %s%s=%s %s$%s%s\00", align 1
@.str.62 = private unnamed_addr constant [38 x i8] c"%s$%s%s %s%s%s %s$%s%s %s->%s %s$%s%s\00", align 1
@.str.63 = private unnamed_addr constant [21 x i8] c"%s$%s%s %s*%s %s%d%s\00", align 1
@.str.64 = private unnamed_addr constant [12 x i8] c"\1B[38;5;164m\00", align 1
@.str.65 = private unnamed_addr constant [31 x i8] c"%slui:%s %s%d%s %s->%s %s$%s%s\00", align 1
@.str.66 = private unnamed_addr constant [24 x i8] c"[%s%d%s] %s->%s %s$%s%s\00", align 1
@.str.67 = private unnamed_addr constant [26 x i8] c"[%s%d%s] %s->%s [%s$%s%s]\00", align 1
@.str.68 = private unnamed_addr constant [24 x i8] c"%s$%s%s %s->%s [%s%d%s]\00", align 1
@.str.69 = private unnamed_addr constant [22 x i8] c"%s%d%s %s->%s %s$%s%s\00", align 1
@.str.70 = private unnamed_addr constant [2 x i8] c">\00", align 1
@.str.71 = private unnamed_addr constant [3 x i8] c">=\00", align 1
@.str.72 = private unnamed_addr constant [9 x i8] c"%sint%s \00", align 1
@.str.73 = private unnamed_addr constant [5 x i8] c"null\00", align 1
@.str.74 = private unnamed_addr constant [7 x i8] c"system\00", align 1
@.str.75 = private unnamed_addr constant [6 x i8] c"timer\00", align 1
@.str.76 = private unnamed_addr constant [7 x i8] c"protec\00", align 1
@.str.77 = private unnamed_addr constant [15 x i8] c"%srit%s %s%d%s\00", align 1
@.str.78 = private unnamed_addr constant [16 x i8] c"%stime%s %s%d%s\00", align 1
@.str.79 = private unnamed_addr constant [16 x i8] c"%sring%s %s%d%s\00", align 1
@.str.80 = private unnamed_addr constant [21 x i8] c"%s(Unknown I-type)%s\00", align 1
@.str.81 = private unnamed_addr constant [37 x i8] c"%s$%s%s %s%s%s %s%d%s %s->%s %s$%s%s\00", align 1
@.str.82 = private unnamed_addr constant [24 x i8] c"%s$%s%s %s%s%s%s %s%d%s\00", align 1
@.str.83 = private unnamed_addr constant [2 x i8] c"=\00", align 1
@.str.84 = private unnamed_addr constant [16 x i8] c" %s->%s %s$%s%s\00", align 1
@.str.85 = private unnamed_addr constant [14 x i8] c"%s$%s%s%c%c%s\00", align 1
@.str.86 = private unnamed_addr constant [23 x i8] c"%s$%s%s %s%c=%s %s%d%s\00", align 1
@.str.87 = private unnamed_addr constant [37 x i8] c"%s$%s%s %s%c%s %s%d%s %s->%s %s$%s%s\00", align 1
@.str.88 = private unnamed_addr constant [4 x i8] c"+%s\00", align 1
@.str.89 = private unnamed_addr constant [4 x i8] c"-%s\00", align 1
@.str.90 = private unnamed_addr constant [4 x i8] c"0%s\00", align 1
@.str.91 = private unnamed_addr constant [4 x i8] c"*%s\00", align 1
@.str.92 = private unnamed_addr constant [8 x i8] c" %s%d%s\00", align 1
@.str.93 = private unnamed_addr constant [11 x i8] c"\1B[38;5;90m\00", align 1
@memory = common global i8* null, align 8
@memsize = common global i64 0, align 8
@pc = common global i64 0, align 8
@offset_symtab = common global i64 0, align 8
@offset_data = common global i64 0, align 8
@offset_code = common global i64 0, align 8
@offset_end = common global i64 0, align 8
@registers = common global [128 x i64] zeroinitializer, align 16
@membytes = common global i64 0, align 8
@cycles = common global i32 0, align 4
@alive = common global i8 0, align 1
@cur_ring = common global i32 0, align 4
@inttab = common global i64 0, align 8

; Function Attrs: noinline nounwind optnone ssp uwtable
define i8* @wvm_disassemble(i64) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i64, align 8
  %4 = alloca i16, align 2
  %5 = alloca i8*, align 8
  %6 = alloca i32, align 4
  store i64 %0, i64* %3, align 8
  %7 = load i64, i64* %3, align 8
  %8 = call zeroext i16 @wvm_get_opcode(i64 %7)
  store i16 %8, i16* %4, align 2
  %9 = call i8* @calloc(i64 255, i64 1) #5
  store i8* %9, i8** %5, align 8
  %10 = load i64, i64* %3, align 8
  %11 = icmp eq i64 %10, 0
  br i1 %11, label %12, label %18

; <label>:12:                                     ; preds = %1
  %13 = load i8*, i8** %5, align 8
  %14 = load i8*, i8** %5, align 8
  %15 = call i64 @llvm.objectsize.i64.p0i8(i8* %14, i1 false, i1 true)
  %16 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %13, i32 0, i64 %15, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  %17 = load i8*, i8** %5, align 8
  store i8* %17, i8** %2, align 8
  br label %47

; <label>:18:                                     ; preds = %1
  %19 = load i16, i16* %4, align 2
  %20 = call i32 @wvm_get_type(i16 zeroext %19)
  store i32 %20, i32* %6, align 4
  %21 = load i32, i32* %6, align 4
  %22 = icmp eq i32 %21, 2
  br i1 %22, label %23, label %26

; <label>:23:                                     ; preds = %18
  %24 = load i8*, i8** %5, align 8
  %25 = load i64, i64* %3, align 8
  call void @wvm_disassemble_r(i8* %24, i64 %25)
  br label %45

; <label>:26:                                     ; preds = %18
  %27 = load i32, i32* %6, align 4
  %28 = icmp eq i32 %27, 1
  br i1 %28, label %29, label %32

; <label>:29:                                     ; preds = %26
  %30 = load i8*, i8** %5, align 8
  %31 = load i64, i64* %3, align 8
  call void @wvm_disassemble_i(i8* %30, i64 %31)
  br label %44

; <label>:32:                                     ; preds = %26
  %33 = load i32, i32* %6, align 4
  %34 = icmp eq i32 %33, 0
  br i1 %34, label %35, label %38

; <label>:35:                                     ; preds = %32
  %36 = load i8*, i8** %5, align 8
  %37 = load i64, i64* %3, align 8
  call void @wvm_disassemble_j(i8* %36, i64 %37)
  br label %43

; <label>:38:                                     ; preds = %32
  %39 = load i8*, i8** %5, align 8
  %40 = load i8*, i8** %5, align 8
  %41 = call i64 @llvm.objectsize.i64.p0i8(i8* %40, i1 false, i1 true)
  %42 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %39, i32 0, i64 %41, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.3, i32 0, i32 0))
  br label %43

; <label>:43:                                     ; preds = %38, %35
  br label %44

; <label>:44:                                     ; preds = %43, %29
  br label %45

; <label>:45:                                     ; preds = %44, %23
  %46 = load i8*, i8** %5, align 8
  store i8* %46, i8** %2, align 8
  br label %47

; <label>:47:                                     ; preds = %45, %12
  %48 = load i8*, i8** %2, align 8
  ret i8* %48
}

declare zeroext i16 @wvm_get_opcode(i64) #1

; Function Attrs: allocsize(0,1)
declare i8* @calloc(i64, i64) #2

declare i32 @__sprintf_chk(i8*, i32, i64, i8*, ...) #1

; Function Attrs: nounwind readnone speculatable
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1, i1) #3

declare i32 @wvm_get_type(i16 zeroext) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_disassemble_r(i8*, i64) #0 {
  %3 = alloca i8*, align 8
  %4 = alloca i64, align 8
  %5 = alloca i16, align 2
  %6 = alloca i8, align 1
  %7 = alloca i8*, align 8
  %8 = alloca i8, align 1
  %9 = alloca i8, align 1
  %10 = alloca i8, align 1
  %11 = alloca i8*, align 8
  %12 = alloca i8*, align 8
  %13 = alloca i8*, align 8
  store i8* %0, i8** %3, align 8
  store i64 %1, i64* %4, align 8
  %14 = load i64, i64* %4, align 8
  %15 = call zeroext i16 @wvm_get_opcode(i64 %14)
  store i16 %15, i16* %5, align 2
  %16 = load i64, i64* %4, align 8
  %17 = call i32 @wvm_r_func(i64 %16)
  %18 = trunc i32 %17 to i8
  store i8 %18, i8* %6, align 1
  %19 = load i64, i64* %4, align 8
  call void @wvm_r_regs(i64 %19, i8* %8, i8* %9, i8* %10)
  %20 = load i8, i8* %8, align 1
  %21 = call i8* @wvm_decode_reg(i8 zeroext %20)
  store i8* %21, i8** %11, align 8
  %22 = load i8, i8* %9, align 1
  %23 = call i8* @wvm_decode_reg(i8 zeroext %22)
  store i8* %23, i8** %12, align 8
  %24 = load i8, i8* %10, align 1
  %25 = call i8* @wvm_decode_reg(i8 zeroext %24)
  store i8* %25, i8** %13, align 8
  %26 = load i16, i16* %5, align 2
  %27 = zext i16 %26 to i32
  switch i32 %27, label %350 [
    i32 1, label %28
    i32 2, label %50
    i32 14, label %113
    i32 17, label %182
    i32 18, label %221
    i32 31, label %273
    i32 48, label %338
    i32 50, label %344
  ]

; <label>:28:                                     ; preds = %2
  %29 = load i8, i8* %6, align 1
  %30 = sext i8 %29 to i32
  switch i32 %30, label %44 [
    i32 0, label %31
    i32 1, label %32
    i32 2, label %33
    i32 6, label %40
    i32 7, label %41
    i32 8, label %42
    i32 9, label %43
  ]

; <label>:31:                                     ; preds = %28
  store i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.4, i32 0, i32 0), i8** %7, align 8
  br label %44

; <label>:32:                                     ; preds = %28
  store i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.5, i32 0, i32 0), i8** %7, align 8
  br label %44

; <label>:33:                                     ; preds = %28
  %34 = load i8*, i8** %3, align 8
  %35 = load i8*, i8** %3, align 8
  %36 = call i64 @llvm.objectsize.i64.p0i8(i8* %35, i1 false, i1 true)
  %37 = load i8*, i8** %11, align 8
  %38 = load i8*, i8** %12, align 8
  %39 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %34, i32 0, i64 %36, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.6, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %37, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %38, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:40:                                     ; preds = %28
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.9, i32 0, i32 0), i8** %7, align 8
  br label %44

; <label>:41:                                     ; preds = %28
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.10, i32 0, i32 0), i8** %7, align 8
  br label %44

; <label>:42:                                     ; preds = %28
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.11, i32 0, i32 0), i8** %7, align 8
  br label %44

; <label>:43:                                     ; preds = %28
  store i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.12, i32 0, i32 0), i8** %7, align 8
  br label %44

; <label>:44:                                     ; preds = %28, %43, %42, %41, %40, %32, %31
  %45 = load i8*, i8** %3, align 8
  %46 = load i8, i8* %8, align 1
  %47 = load i8, i8* %9, align 1
  %48 = load i8, i8* %10, align 1
  %49 = load i8*, i8** %7, align 8
  call void @wvm_disassemble_r_alt_op(i8* %45, i8 zeroext %46, i8 zeroext %47, i8 zeroext %48, i8* %49)
  br label %355

; <label>:50:                                     ; preds = %2
  %51 = load i8, i8* %6, align 1
  %52 = sext i8 %51 to i32
  switch i32 %52, label %107 [
    i32 0, label %53
    i32 1, label %54
    i32 2, label %55
    i32 5, label %56
    i32 6, label %57
    i32 8, label %58
    i32 9, label %59
    i32 10, label %60
    i32 13, label %61
    i32 14, label %62
    i32 4, label %63
    i32 12, label %63
    i32 3, label %80
    i32 11, label %87
  ]

; <label>:53:                                     ; preds = %50
  store i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.13, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:54:                                     ; preds = %50
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.14, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:55:                                     ; preds = %50
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.15, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:56:                                     ; preds = %50
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.16, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:57:                                     ; preds = %50
  store i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.17, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:58:                                     ; preds = %50
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.18, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:59:                                     ; preds = %50
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.19, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:60:                                     ; preds = %50
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.20, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:61:                                     ; preds = %50
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.21, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:62:                                     ; preds = %50
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.22, i32 0, i32 0), i8** %7, align 8
  br label %107

; <label>:63:                                     ; preds = %50, %50
  %64 = load i8, i8* %8, align 1
  %65 = zext i8 %64 to i32
  %66 = icmp eq i32 %65, 0
  br i1 %66, label %67, label %74

; <label>:67:                                     ; preds = %63
  %68 = load i8*, i8** %3, align 8
  %69 = load i8*, i8** %3, align 8
  %70 = call i64 @llvm.objectsize.i64.p0i8(i8* %69, i1 false, i1 true)
  %71 = load i8*, i8** %12, align 8
  %72 = load i8*, i8** %13, align 8
  %73 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %68, i32 0, i64 %70, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.23, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %71, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %72, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:74:                                     ; preds = %63
  %75 = load i8, i8* %6, align 1
  %76 = sext i8 %75 to i32
  %77 = icmp eq i32 %76, 4
  %78 = zext i1 %77 to i64
  %79 = select i1 %77, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.24, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.25, i32 0, i32 0)
  store i8* %79, i8** %7, align 8
  br label %107

; <label>:80:                                     ; preds = %50
  %81 = load i8*, i8** %3, align 8
  %82 = load i8*, i8** %3, align 8
  %83 = call i64 @llvm.objectsize.i64.p0i8(i8* %82, i1 false, i1 true)
  %84 = load i8*, i8** %11, align 8
  %85 = load i8*, i8** %13, align 8
  %86 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %81, i32 0, i64 %83, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str.26, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %84, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %85, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:87:                                     ; preds = %50
  %88 = load i8, i8* %8, align 1
  %89 = zext i8 %88 to i32
  %90 = load i8, i8* %10, align 1
  %91 = zext i8 %90 to i32
  %92 = icmp eq i32 %89, %91
  br i1 %92, label %93, label %99

; <label>:93:                                     ; preds = %87
  %94 = load i8*, i8** %3, align 8
  %95 = load i8*, i8** %3, align 8
  %96 = call i64 @llvm.objectsize.i64.p0i8(i8* %95, i1 false, i1 true)
  %97 = load i8*, i8** %11, align 8
  %98 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %94, i32 0, i64 %96, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.27, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %97, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %106

; <label>:99:                                     ; preds = %87
  %100 = load i8*, i8** %3, align 8
  %101 = load i8*, i8** %3, align 8
  %102 = call i64 @llvm.objectsize.i64.p0i8(i8* %101, i1 false, i1 true)
  %103 = load i8*, i8** %11, align 8
  %104 = load i8*, i8** %13, align 8
  %105 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %100, i32 0, i64 %102, i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str.28, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %103, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %104, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %106

; <label>:106:                                    ; preds = %99, %93
  br label %355

; <label>:107:                                    ; preds = %50, %74, %62, %61, %60, %59, %58, %57, %56, %55, %54, %53
  %108 = load i8*, i8** %3, align 8
  %109 = load i8, i8* %8, align 1
  %110 = load i8, i8* %9, align 1
  %111 = load i8, i8* %10, align 1
  %112 = load i8*, i8** %7, align 8
  call void @wvm_disassemble_r_alt_op(i8* %108, i8 zeroext %109, i8 zeroext %110, i8 zeroext %111, i8* %112)
  br label %355

; <label>:113:                                    ; preds = %2
  %114 = load i8*, i8** %3, align 8
  %115 = load i8*, i8** %3, align 8
  %116 = call i64 @llvm.objectsize.i64.p0i8(i8* %115, i1 false, i1 true)
  %117 = load i8*, i8** %11, align 8
  %118 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %114, i32 0, i64 %116, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.29, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %117, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0))
  %119 = load i8, i8* %6, align 1
  %120 = sext i8 %119 to i32
  %121 = icmp eq i32 %120, 0
  br i1 %121, label %126, label %122

; <label>:122:                                    ; preds = %113
  %123 = load i8, i8* %6, align 1
  %124 = sext i8 %123 to i32
  %125 = icmp eq i32 %124, 3
  br i1 %125, label %126, label %131

; <label>:126:                                    ; preds = %122, %113
  %127 = load i8*, i8** %3, align 8
  %128 = load i8*, i8** %3, align 8
  %129 = call i64 @llvm.objectsize.i64.p0i8(i8* %128, i1 false, i1 true)
  %130 = call i8* @__strcat_chk(i8* %127, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.30, i32 0, i32 0), i64 %129) #6
  br label %131

; <label>:131:                                    ; preds = %126, %122
  %132 = load i8, i8* %6, align 1
  %133 = sext i8 %132 to i32
  %134 = icmp eq i32 %133, 1
  br i1 %134, label %139, label %135

; <label>:135:                                    ; preds = %131
  %136 = load i8, i8* %6, align 1
  %137 = sext i8 %136 to i32
  %138 = icmp eq i32 %137, 4
  br i1 %138, label %139, label %144

; <label>:139:                                    ; preds = %135, %131
  %140 = load i8*, i8** %3, align 8
  %141 = load i8*, i8** %3, align 8
  %142 = call i64 @llvm.objectsize.i64.p0i8(i8* %141, i1 false, i1 true)
  %143 = call i8* @__strcat_chk(i8* %140, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.31, i32 0, i32 0), i64 %142) #6
  br label %144

; <label>:144:                                    ; preds = %139, %135
  %145 = load i8, i8* %6, align 1
  %146 = sext i8 %145 to i32
  %147 = icmp eq i32 %146, 2
  br i1 %147, label %152, label %148

; <label>:148:                                    ; preds = %144
  %149 = load i8, i8* %6, align 1
  %150 = sext i8 %149 to i32
  %151 = icmp eq i32 %150, 5
  br i1 %151, label %152, label %157

; <label>:152:                                    ; preds = %148, %144
  %153 = load i8*, i8** %3, align 8
  %154 = load i8*, i8** %3, align 8
  %155 = call i64 @llvm.objectsize.i64.p0i8(i8* %154, i1 false, i1 true)
  %156 = call i8* @__strcat_chk(i8* %153, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.32, i32 0, i32 0), i64 %155) #6
  br label %157

; <label>:157:                                    ; preds = %152, %148
  %158 = load i8*, i8** %3, align 8
  %159 = load i8*, i8** %3, align 8
  %160 = call i64 @strlen(i8* %159)
  %161 = getelementptr inbounds i8, i8* %158, i64 %160
  %162 = load i8*, i8** %12, align 8
  %163 = load i8*, i8** %13, align 8
  %164 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %161, i32 0, i64 -1, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.33, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %162, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %163, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  %165 = load i8, i8* %6, align 1
  %166 = sext i8 %165 to i32
  %167 = icmp eq i32 %166, 3
  br i1 %167, label %176, label %168

; <label>:168:                                    ; preds = %157
  %169 = load i8, i8* %6, align 1
  %170 = sext i8 %169 to i32
  %171 = icmp eq i32 %170, 4
  br i1 %171, label %176, label %172

; <label>:172:                                    ; preds = %168
  %173 = load i8, i8* %6, align 1
  %174 = sext i8 %173 to i32
  %175 = icmp eq i32 %174, 5
  br i1 %175, label %176, label %181

; <label>:176:                                    ; preds = %172, %168, %157
  %177 = load i8*, i8** %3, align 8
  %178 = load i8*, i8** %3, align 8
  %179 = call i64 @llvm.objectsize.i64.p0i8(i8* %178, i1 false, i1 true)
  %180 = call i8* @__strcat_chk(i8* %177, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.34, i32 0, i32 0), i64 %179) #6
  br label %181

; <label>:181:                                    ; preds = %176, %172
  br label %355

; <label>:182:                                    ; preds = %2
  %183 = load i8*, i8** %3, align 8
  %184 = load i8*, i8** %3, align 8
  %185 = call i64 @llvm.objectsize.i64.p0i8(i8* %184, i1 false, i1 true)
  %186 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %183, i32 0, i64 %185, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.35, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0))
  %187 = load i8, i8* %6, align 1
  %188 = sext i8 %187 to i32
  %189 = icmp eq i32 %188, 2
  br i1 %189, label %194, label %190

; <label>:190:                                    ; preds = %182
  %191 = load i8, i8* %6, align 1
  %192 = sext i8 %191 to i32
  %193 = icmp eq i32 %192, 3
  br i1 %193, label %194, label %199

; <label>:194:                                    ; preds = %190, %182
  %195 = load i8*, i8** %3, align 8
  %196 = load i8*, i8** %3, align 8
  %197 = call i64 @llvm.objectsize.i64.p0i8(i8* %196, i1 false, i1 true)
  %198 = call i8* @__strcat_chk(i8* %195, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.36, i32 0, i32 0), i64 %197) #6
  br label %199

; <label>:199:                                    ; preds = %194, %190
  %200 = load i8*, i8** %3, align 8
  %201 = load i8*, i8** %3, align 8
  %202 = call i64 @llvm.objectsize.i64.p0i8(i8* %201, i1 false, i1 true)
  %203 = load i8*, i8** %3, align 8
  %204 = load i8*, i8** %13, align 8
  %205 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %200, i32 0, i64 %202, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.37, i32 0, i32 0), i8* %203, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %204, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  %206 = load i8, i8* %6, align 1
  %207 = sext i8 %206 to i32
  %208 = icmp eq i32 %207, 1
  br i1 %208, label %213, label %209

; <label>:209:                                    ; preds = %199
  %210 = load i8, i8* %6, align 1
  %211 = sext i8 %210 to i32
  %212 = icmp eq i32 %211, 3
  br i1 %212, label %213, label %220

; <label>:213:                                    ; preds = %209, %199
  %214 = load i8*, i8** %3, align 8
  %215 = load i8*, i8** %3, align 8
  %216 = call i64 @strlen(i8* %215)
  %217 = getelementptr inbounds i8, i8* %214, i64 %216
  %218 = load i8*, i8** %11, align 8
  %219 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %217, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.38, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.39, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %218, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %220

; <label>:220:                                    ; preds = %213, %209
  br label %355

; <label>:221:                                    ; preds = %2
  %222 = load i8, i8* %6, align 1
  %223 = sext i8 %222 to i32
  switch i32 %223, label %272 [
    i32 0, label %224
    i32 3, label %224
    i32 1, label %236
    i32 4, label %236
    i32 2, label %248
    i32 5, label %248
    i32 6, label %260
    i32 7, label %266
  ]

; <label>:224:                                    ; preds = %221, %221
  %225 = load i8*, i8** %3, align 8
  %226 = load i8*, i8** %3, align 8
  %227 = call i64 @llvm.objectsize.i64.p0i8(i8* %226, i1 false, i1 true)
  %228 = load i8*, i8** %11, align 8
  %229 = load i8*, i8** %13, align 8
  %230 = load i8, i8* %6, align 1
  %231 = sext i8 %230 to i32
  %232 = icmp eq i32 %231, 3
  %233 = zext i1 %232 to i64
  %234 = select i1 %232, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i32 0, i32 0)
  %235 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %225, i32 0, i64 %227, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str.40, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %228, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %229, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* %234)
  br label %355

; <label>:236:                                    ; preds = %221, %221
  %237 = load i8*, i8** %3, align 8
  %238 = load i8*, i8** %3, align 8
  %239 = call i64 @llvm.objectsize.i64.p0i8(i8* %238, i1 false, i1 true)
  %240 = load i8*, i8** %11, align 8
  %241 = load i8*, i8** %13, align 8
  %242 = load i8, i8* %6, align 1
  %243 = sext i8 %242 to i32
  %244 = icmp eq i32 %243, 4
  %245 = zext i1 %244 to i64
  %246 = select i1 %244, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i32 0, i32 0)
  %247 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %237, i32 0, i64 %239, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.43, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %240, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %241, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* %246)
  br label %355

; <label>:248:                                    ; preds = %221, %221
  %249 = load i8*, i8** %3, align 8
  %250 = load i8*, i8** %3, align 8
  %251 = call i64 @llvm.objectsize.i64.p0i8(i8* %250, i1 false, i1 true)
  %252 = load i8*, i8** %11, align 8
  %253 = load i8*, i8** %13, align 8
  %254 = load i8, i8* %6, align 1
  %255 = sext i8 %254 to i32
  %256 = icmp eq i32 %255, 5
  %257 = zext i1 %256 to i64
  %258 = select i1 %256, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i32 0, i32 0)
  %259 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %249, i32 0, i64 %251, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.44, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %252, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %253, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* %258)
  br label %355

; <label>:260:                                    ; preds = %221
  %261 = load i8*, i8** %3, align 8
  %262 = load i8*, i8** %3, align 8
  %263 = call i64 @llvm.objectsize.i64.p0i8(i8* %262, i1 false, i1 true)
  %264 = load i8*, i8** %11, align 8
  %265 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %261, i32 0, i64 %263, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.45, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %264, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:266:                                    ; preds = %221
  %267 = load i8*, i8** %3, align 8
  %268 = load i8*, i8** %3, align 8
  %269 = call i64 @llvm.objectsize.i64.p0i8(i8* %268, i1 false, i1 true)
  %270 = load i8*, i8** %13, align 8
  %271 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %267, i32 0, i64 %269, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.46, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %270, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:272:                                    ; preds = %221
  br label %273

; <label>:273:                                    ; preds = %2, %272
  %274 = load i8*, i8** %3, align 8
  %275 = load i8*, i8** %3, align 8
  %276 = call i64 @llvm.objectsize.i64.p0i8(i8* %275, i1 false, i1 true)
  %277 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %274, i32 0, i64 %276, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.47, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i32 0, i32 0))
  %278 = load i8, i8* %6, align 1
  %279 = sext i8 %278 to i32
  switch i32 %279, label %327 [
    i32 1, label %280
    i32 2, label %287
    i32 3, label %293
    i32 4, label %299
    i32 5, label %306
    i32 6, label %313
    i32 7, label %320
  ]

; <label>:280:                                    ; preds = %273
  %281 = load i8*, i8** %3, align 8
  %282 = load i8*, i8** %3, align 8
  %283 = call i64 @strlen(i8* %282)
  %284 = getelementptr inbounds i8, i8* %281, i64 %283
  %285 = load i8*, i8** %11, align 8
  %286 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %284, i32 0, i64 -1, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.49, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %285)
  br label %332

; <label>:287:                                    ; preds = %273
  %288 = load i8*, i8** %3, align 8
  %289 = load i8*, i8** %3, align 8
  %290 = call i64 @strlen(i8* %289)
  %291 = getelementptr inbounds i8, i8* %288, i64 %290
  %292 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %291, i32 0, i64 -1, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.50, i32 0, i32 0))
  br label %332

; <label>:293:                                    ; preds = %273
  %294 = load i8*, i8** %3, align 8
  %295 = load i8*, i8** %3, align 8
  %296 = call i64 @strlen(i8* %295)
  %297 = getelementptr inbounds i8, i8* %294, i64 %296
  %298 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %297, i32 0, i64 -1, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.51, i32 0, i32 0))
  br label %332

; <label>:299:                                    ; preds = %273
  %300 = load i8*, i8** %3, align 8
  %301 = load i8*, i8** %3, align 8
  %302 = call i64 @strlen(i8* %301)
  %303 = getelementptr inbounds i8, i8* %300, i64 %302
  %304 = load i8*, i8** %11, align 8
  %305 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %303, i32 0, i64 -1, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.52, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %304)
  br label %332

; <label>:306:                                    ; preds = %273
  %307 = load i8*, i8** %3, align 8
  %308 = load i8*, i8** %3, align 8
  %309 = call i64 @strlen(i8* %308)
  %310 = getelementptr inbounds i8, i8* %307, i64 %309
  %311 = load i8*, i8** %11, align 8
  %312 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %310, i32 0, i64 -1, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.53, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %311)
  br label %332

; <label>:313:                                    ; preds = %273
  %314 = load i8*, i8** %3, align 8
  %315 = load i8*, i8** %3, align 8
  %316 = call i64 @strlen(i8* %315)
  %317 = getelementptr inbounds i8, i8* %314, i64 %316
  %318 = load i8*, i8** %11, align 8
  %319 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %317, i32 0, i64 -1, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.54, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %318)
  br label %332

; <label>:320:                                    ; preds = %273
  %321 = load i8*, i8** %3, align 8
  %322 = load i8*, i8** %3, align 8
  %323 = call i64 @strlen(i8* %322)
  %324 = getelementptr inbounds i8, i8* %321, i64 %323
  %325 = load i8*, i8** %11, align 8
  %326 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %324, i32 0, i64 -1, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.55, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %325)
  br label %332

; <label>:327:                                    ; preds = %273
  %328 = load i8*, i8** %3, align 8
  %329 = load i8*, i8** %3, align 8
  %330 = call i64 @llvm.objectsize.i64.p0i8(i8* %329, i1 false, i1 true)
  %331 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %328, i32 0, i64 %330, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.56, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:332:                                    ; preds = %320, %313, %306, %299, %293, %287, %280
  %333 = load i8*, i8** %3, align 8
  %334 = load i8*, i8** %3, align 8
  %335 = call i64 @strlen(i8* %334)
  %336 = getelementptr inbounds i8, i8* %333, i64 %335
  %337 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %336, i32 0, i64 -1, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.57, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:338:                                    ; preds = %2
  %339 = load i8*, i8** %3, align 8
  %340 = load i8*, i8** %3, align 8
  %341 = call i64 @llvm.objectsize.i64.p0i8(i8* %340, i1 false, i1 true)
  %342 = load i8*, i8** %11, align 8
  %343 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %339, i32 0, i64 %341, i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.58, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %342, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:344:                                    ; preds = %2
  %345 = load i8*, i8** %3, align 8
  %346 = load i8*, i8** %3, align 8
  %347 = call i64 @llvm.objectsize.i64.p0i8(i8* %346, i1 false, i1 true)
  %348 = load i8*, i8** %11, align 8
  %349 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %345, i32 0, i64 %347, i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.59, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %348, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:350:                                    ; preds = %2
  %351 = load i8*, i8** %3, align 8
  %352 = load i8*, i8** %3, align 8
  %353 = call i64 @llvm.objectsize.i64.p0i8(i8* %352, i1 false, i1 true)
  %354 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %351, i32 0, i64 %353, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.60, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %355

; <label>:355:                                    ; preds = %350, %344, %338, %332, %327, %266, %260, %248, %236, %224, %220, %181, %107, %106, %80, %67, %44, %33
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_disassemble_i(i8*, i64) #0 {
  %3 = alloca i8*, align 8
  %4 = alloca i64, align 8
  %5 = alloca i16, align 2
  %6 = alloca i8, align 1
  %7 = alloca i8, align 1
  %8 = alloca i32, align 4
  store i8* %0, i8** %3, align 8
  store i64 %1, i64* %4, align 8
  %9 = load i64, i64* %4, align 8
  %10 = call zeroext i16 @wvm_get_opcode(i64 %9)
  store i16 %10, i16* %5, align 2
  %11 = load i64, i64* %4, align 8
  %12 = call zeroext i8 @wvm_i_rs(i64 %11)
  store i8 %12, i8* %6, align 1
  %13 = load i64, i64* %4, align 8
  %14 = call zeroext i8 @wvm_i_rd(i64 %13)
  store i8 %14, i8* %7, align 1
  %15 = load i64, i64* %4, align 8
  %16 = call i32 @wvm_i_imm(i64 %15)
  store i32 %16, i32* %8, align 4
  %17 = load i16, i16* %5, align 2
  %18 = zext i16 %17 to i32
  switch i32 %18, label %206 [
    i32 3, label %19
    i32 4, label %24
    i32 5, label %29
    i32 24, label %29
    i32 30, label %37
    i32 6, label %42
    i32 7, label %47
    i32 8, label %52
    i32 9, label %57
    i32 10, label %62
    i32 11, label %67
    i32 34, label %72
    i32 35, label %77
    i32 36, label %82
    i32 13, label %87
    i32 19, label %95
    i32 37, label %95
    i32 39, label %103
    i32 40, label %103
    i32 20, label %111
    i32 38, label %111
    i32 21, label %119
    i32 25, label %127
    i32 28, label %127
    i32 26, label %132
    i32 29, label %132
    i32 41, label %137
    i32 42, label %142
    i32 27, label %147
    i32 32, label %152
    i32 33, label %188
    i32 49, label %194
    i32 51, label %200
  ]

; <label>:19:                                     ; preds = %2
  %20 = load i8*, i8** %3, align 8
  %21 = load i8, i8* %6, align 1
  %22 = load i8, i8* %7, align 1
  %23 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_math(i8* %20, i8 zeroext %21, i8 zeroext %22, i32 %23, i8 signext 43)
  br label %211

; <label>:24:                                     ; preds = %2
  %25 = load i8*, i8** %3, align 8
  %26 = load i8, i8* %6, align 1
  %27 = load i8, i8* %7, align 1
  %28 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_math(i8* %25, i8 zeroext %26, i8 zeroext %27, i32 %28, i8 signext 45)
  br label %211

; <label>:29:                                     ; preds = %2, %2
  %30 = load i8*, i8** %3, align 8
  %31 = load i8*, i8** %3, align 8
  %32 = call i64 @llvm.objectsize.i64.p0i8(i8* %31, i1 false, i1 true)
  %33 = load i8, i8* %6, align 1
  %34 = call i8* @wvm_decode_reg(i8 zeroext %33)
  %35 = load i32, i32* %8, align 4
  %36 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %30, i32 0, i64 %32, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.63, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %34, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %35, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:37:                                     ; preds = %2
  %38 = load i8*, i8** %3, align 8
  %39 = load i8, i8* %6, align 1
  %40 = load i8, i8* %7, align 1
  %41 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %38, i8 zeroext %39, i8 zeroext %40, i32 %41, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.12, i32 0, i32 0))
  br label %211

; <label>:42:                                     ; preds = %2
  %43 = load i8*, i8** %3, align 8
  %44 = load i8, i8* %6, align 1
  %45 = load i8, i8* %7, align 1
  %46 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %43, i8 zeroext %44, i8 zeroext %45, i32 %46, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.13, i32 0, i32 0))
  br label %211

; <label>:47:                                     ; preds = %2
  %48 = load i8*, i8** %3, align 8
  %49 = load i8, i8* %6, align 1
  %50 = load i8, i8* %7, align 1
  %51 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %48, i8 zeroext %49, i8 zeroext %50, i32 %51, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.14, i32 0, i32 0))
  br label %211

; <label>:52:                                     ; preds = %2
  %53 = load i8*, i8** %3, align 8
  %54 = load i8, i8* %6, align 1
  %55 = load i8, i8* %7, align 1
  %56 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %53, i8 zeroext %54, i8 zeroext %55, i32 %56, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.15, i32 0, i32 0))
  br label %211

; <label>:57:                                     ; preds = %2
  %58 = load i8*, i8** %3, align 8
  %59 = load i8, i8* %6, align 1
  %60 = load i8, i8* %7, align 1
  %61 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %58, i8 zeroext %59, i8 zeroext %60, i32 %61, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.24, i32 0, i32 0))
  br label %211

; <label>:62:                                     ; preds = %2
  %63 = load i8*, i8** %3, align 8
  %64 = load i8, i8* %6, align 1
  %65 = load i8, i8* %7, align 1
  %66 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %63, i8 zeroext %64, i8 zeroext %65, i32 %66, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.16, i32 0, i32 0))
  br label %211

; <label>:67:                                     ; preds = %2
  %68 = load i8*, i8** %3, align 8
  %69 = load i8, i8* %6, align 1
  %70 = load i8, i8* %7, align 1
  %71 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %68, i8 zeroext %69, i8 zeroext %70, i32 %71, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.17, i32 0, i32 0))
  br label %211

; <label>:72:                                     ; preds = %2
  %73 = load i8*, i8** %3, align 8
  %74 = load i8, i8* %6, align 1
  %75 = load i8, i8* %7, align 1
  %76 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %73, i8 zeroext %74, i8 zeroext %75, i32 %76, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.9, i32 0, i32 0))
  br label %211

; <label>:77:                                     ; preds = %2
  %78 = load i8*, i8** %3, align 8
  %79 = load i8, i8* %6, align 1
  %80 = load i8, i8* %7, align 1
  %81 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %78, i8 zeroext %79, i8 zeroext %80, i32 %81, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.10, i32 0, i32 0))
  br label %211

; <label>:82:                                     ; preds = %2
  %83 = load i8*, i8** %3, align 8
  %84 = load i8, i8* %6, align 1
  %85 = load i8, i8* %7, align 1
  %86 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_alt_op(i8* %83, i8 zeroext %84, i8 zeroext %85, i32 %86, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.11, i32 0, i32 0))
  br label %211

; <label>:87:                                     ; preds = %2
  %88 = load i8*, i8** %3, align 8
  %89 = load i8*, i8** %3, align 8
  %90 = call i64 @llvm.objectsize.i64.p0i8(i8* %89, i1 false, i1 true)
  %91 = load i32, i32* %8, align 4
  %92 = load i8, i8* %7, align 1
  %93 = call i8* @wvm_decode_reg(i8 zeroext %92)
  %94 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %88, i32 0, i64 %90, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.65, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %91, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %93, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:95:                                     ; preds = %2, %2
  %96 = load i8*, i8** %3, align 8
  %97 = load i8*, i8** %3, align 8
  %98 = call i64 @llvm.objectsize.i64.p0i8(i8* %97, i1 false, i1 true)
  %99 = load i32, i32* %8, align 4
  %100 = load i8, i8* %7, align 1
  %101 = call i8* @wvm_decode_reg(i8 zeroext %100)
  %102 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %96, i32 0, i64 %98, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.66, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %99, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %101, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:103:                                    ; preds = %2, %2
  %104 = load i8*, i8** %3, align 8
  %105 = load i8*, i8** %3, align 8
  %106 = call i64 @llvm.objectsize.i64.p0i8(i8* %105, i1 false, i1 true)
  %107 = load i32, i32* %8, align 4
  %108 = load i8, i8* %7, align 1
  %109 = call i8* @wvm_decode_reg(i8 zeroext %108)
  %110 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %104, i32 0, i64 %106, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.67, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %107, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %109, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:111:                                    ; preds = %2, %2
  %112 = load i8*, i8** %3, align 8
  %113 = load i8*, i8** %3, align 8
  %114 = call i64 @llvm.objectsize.i64.p0i8(i8* %113, i1 false, i1 true)
  %115 = load i8, i8* %6, align 1
  %116 = call i8* @wvm_decode_reg(i8 zeroext %115)
  %117 = load i32, i32* %8, align 4
  %118 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %112, i32 0, i64 %114, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.68, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %116, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %117, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:119:                                    ; preds = %2
  %120 = load i8*, i8** %3, align 8
  %121 = load i8*, i8** %3, align 8
  %122 = call i64 @llvm.objectsize.i64.p0i8(i8* %121, i1 false, i1 true)
  %123 = load i32, i32* %8, align 4
  %124 = load i8, i8* %7, align 1
  %125 = call i8* @wvm_decode_reg(i8 zeroext %124)
  %126 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %120, i32 0, i64 %122, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.69, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %123, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %125, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:127:                                    ; preds = %2, %2
  %128 = load i8*, i8** %3, align 8
  %129 = load i8, i8* %6, align 1
  %130 = load i8, i8* %7, align 1
  %131 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_comp(i8* %128, i8 zeroext %129, i8 zeroext %130, i32 %131, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.30, i32 0, i32 0))
  br label %211

; <label>:132:                                    ; preds = %2, %2
  %133 = load i8*, i8** %3, align 8
  %134 = load i8, i8* %6, align 1
  %135 = load i8, i8* %7, align 1
  %136 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_comp(i8* %133, i8 zeroext %134, i8 zeroext %135, i32 %136, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.31, i32 0, i32 0))
  br label %211

; <label>:137:                                    ; preds = %2
  %138 = load i8*, i8** %3, align 8
  %139 = load i8, i8* %6, align 1
  %140 = load i8, i8* %7, align 1
  %141 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_comp(i8* %138, i8 zeroext %139, i8 zeroext %140, i32 %141, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.70, i32 0, i32 0))
  br label %211

; <label>:142:                                    ; preds = %2
  %143 = load i8*, i8** %3, align 8
  %144 = load i8, i8* %6, align 1
  %145 = load i8, i8* %7, align 1
  %146 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_comp(i8* %143, i8 zeroext %144, i8 zeroext %145, i32 %146, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.71, i32 0, i32 0))
  br label %211

; <label>:147:                                    ; preds = %2
  %148 = load i8*, i8** %3, align 8
  %149 = load i8, i8* %6, align 1
  %150 = load i8, i8* %7, align 1
  %151 = load i32, i32* %8, align 4
  call void @wvm_disassemble_i_comp(i8* %148, i8 zeroext %149, i8 zeroext %150, i32 %151, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.32, i32 0, i32 0))
  br label %211

; <label>:152:                                    ; preds = %2
  %153 = load i8*, i8** %3, align 8
  %154 = load i8*, i8** %3, align 8
  %155 = call i64 @llvm.objectsize.i64.p0i8(i8* %154, i1 false, i1 true)
  %156 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %153, i32 0, i64 %155, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.72, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  %157 = load i32, i32* %8, align 4
  switch i32 %157, label %182 [
    i32 0, label %158
    i32 1, label %164
    i32 2, label %170
    i32 3, label %176
  ]

; <label>:158:                                    ; preds = %152
  %159 = load i8*, i8** %3, align 8
  %160 = load i8*, i8** %3, align 8
  %161 = call i64 @strlen(i8* %160)
  %162 = getelementptr inbounds i8, i8* %159, i64 %161
  %163 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %162, i32 0, i64 -1, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.73, i32 0, i32 0))
  br label %242

; <label>:164:                                    ; preds = %152
  %165 = load i8*, i8** %3, align 8
  %166 = load i8*, i8** %3, align 8
  %167 = call i64 @strlen(i8* %166)
  %168 = getelementptr inbounds i8, i8* %165, i64 %167
  %169 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %168, i32 0, i64 -1, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.74, i32 0, i32 0))
  br label %242

; <label>:170:                                    ; preds = %152
  %171 = load i8*, i8** %3, align 8
  %172 = load i8*, i8** %3, align 8
  %173 = call i64 @strlen(i8* %172)
  %174 = getelementptr inbounds i8, i8* %171, i64 %173
  %175 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %174, i32 0, i64 -1, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.75, i32 0, i32 0))
  br label %242

; <label>:176:                                    ; preds = %152
  %177 = load i8*, i8** %3, align 8
  %178 = load i8*, i8** %3, align 8
  %179 = call i64 @strlen(i8* %178)
  %180 = getelementptr inbounds i8, i8* %177, i64 %179
  %181 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %180, i32 0, i64 -1, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.76, i32 0, i32 0))
  br label %242

; <label>:182:                                    ; preds = %152
  %183 = load i8*, i8** %3, align 8
  %184 = load i8*, i8** %3, align 8
  %185 = call i64 @strlen(i8* %184)
  %186 = getelementptr inbounds i8, i8* %183, i64 %185
  %187 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %186, i32 0, i64 -1, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.3, i32 0, i32 0))
  br label %242

; <label>:188:                                    ; preds = %2
  %189 = load i8*, i8** %3, align 8
  %190 = load i8*, i8** %3, align 8
  %191 = call i64 @llvm.objectsize.i64.p0i8(i8* %190, i1 false, i1 true)
  %192 = load i32, i32* %8, align 4
  %193 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %189, i32 0, i64 %191, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.77, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %192, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:194:                                    ; preds = %2
  %195 = load i8*, i8** %3, align 8
  %196 = load i8*, i8** %3, align 8
  %197 = call i64 @llvm.objectsize.i64.p0i8(i8* %196, i1 false, i1 true)
  %198 = load i32, i32* %8, align 4
  %199 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %195, i32 0, i64 %197, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.78, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %198, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:200:                                    ; preds = %2
  %201 = load i8*, i8** %3, align 8
  %202 = load i8*, i8** %3, align 8
  %203 = call i64 @llvm.objectsize.i64.p0i8(i8* %202, i1 false, i1 true)
  %204 = load i32, i32* %8, align 4
  %205 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %201, i32 0, i64 %203, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.79, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.48, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %204, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:206:                                    ; preds = %2
  %207 = load i8*, i8** %3, align 8
  %208 = load i8*, i8** %3, align 8
  %209 = call i64 @llvm.objectsize.i64.p0i8(i8* %208, i1 false, i1 true)
  %210 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %207, i32 0, i64 %209, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.80, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %211

; <label>:211:                                    ; preds = %206, %200, %194, %188, %147, %142, %137, %132, %127, %119, %111, %103, %95, %87, %82, %77, %72, %67, %62, %57, %52, %47, %42, %37, %29, %24, %19
  %212 = load i16, i16* %5, align 2
  %213 = zext i16 %212 to i32
  %214 = icmp eq i32 %213, 24
  br i1 %214, label %223, label %215

; <label>:215:                                    ; preds = %211
  %216 = load i16, i16* %5, align 2
  %217 = zext i16 %216 to i32
  %218 = icmp eq i32 %217, 28
  br i1 %218, label %223, label %219

; <label>:219:                                    ; preds = %215
  %220 = load i16, i16* %5, align 2
  %221 = zext i16 %220 to i32
  %222 = icmp eq i32 %221, 29
  br i1 %222, label %223, label %228

; <label>:223:                                    ; preds = %219, %215, %211
  %224 = load i8*, i8** %3, align 8
  %225 = load i8*, i8** %3, align 8
  %226 = call i64 @llvm.objectsize.i64.p0i8(i8* %225, i1 false, i1 true)
  %227 = call i8* @__strcat_chk(i8* %224, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.34, i32 0, i32 0), i64 %226) #6
  br label %242

; <label>:228:                                    ; preds = %219
  %229 = load i16, i16* %5, align 2
  %230 = zext i16 %229 to i32
  %231 = icmp eq i32 %230, 37
  br i1 %231, label %236, label %232

; <label>:232:                                    ; preds = %228
  %233 = load i16, i16* %5, align 2
  %234 = zext i16 %233 to i32
  %235 = icmp eq i32 %234, 38
  br i1 %235, label %236, label %241

; <label>:236:                                    ; preds = %232, %228
  %237 = load i8*, i8** %3, align 8
  %238 = load i8*, i8** %3, align 8
  %239 = call i64 @llvm.objectsize.i64.p0i8(i8* %238, i1 false, i1 true)
  %240 = call i8* @__strcat_chk(i8* %237, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.41, i32 0, i32 0), i64 %239) #6
  br label %241

; <label>:241:                                    ; preds = %236, %232
  br label %242

; <label>:242:                                    ; preds = %158, %164, %170, %176, %182, %241, %223
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_disassemble_j(i8*, i64) #0 {
  %3 = alloca i8*, align 8
  %4 = alloca i64, align 8
  %5 = alloca i16, align 2
  %6 = alloca i8, align 1
  %7 = alloca i32, align 4
  %8 = alloca i8, align 1
  %9 = alloca i8, align 1
  %10 = alloca i8*, align 8
  store i8* %0, i8** %3, align 8
  store i64 %1, i64* %4, align 8
  %11 = load i64, i64* %4, align 8
  %12 = call zeroext i16 @wvm_get_opcode(i64 %11)
  store i16 %12, i16* %5, align 2
  %13 = load i64, i64* %4, align 8
  %14 = call zeroext i8 @wvm_j_rs(i64 %13)
  store i8 %14, i8* %6, align 1
  %15 = load i64, i64* %4, align 8
  %16 = call i32 @wvm_j_addr(i64 %15)
  store i32 %16, i32* %7, align 4
  %17 = load i64, i64* %4, align 8
  %18 = call signext i8 @wvm_j_link(i64 %17)
  store i8 %18, i8* %8, align 1
  %19 = load i64, i64* %4, align 8
  %20 = call signext i8 @wvm_j_cond(i64 %19)
  store i8 %20, i8* %9, align 1
  %21 = load i8, i8* %6, align 1
  %22 = call i8* @wvm_decode_reg(i8 zeroext %21)
  store i8* %22, i8** %10, align 8
  %23 = load i8*, i8** %3, align 8
  %24 = load i8*, i8** %3, align 8
  %25 = call i64 @llvm.objectsize.i64.p0i8(i8* %24, i1 false, i1 true)
  %26 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %23, i32 0, i64 %25, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.35, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0))
  %27 = load i8, i8* %8, align 1
  %28 = icmp ne i8 %27, 0
  br i1 %28, label %29, label %35

; <label>:29:                                     ; preds = %2
  %30 = load i8*, i8** %3, align 8
  %31 = load i8*, i8** %3, align 8
  %32 = call i64 @llvm.objectsize.i64.p0i8(i8* %31, i1 false, i1 true)
  %33 = load i8*, i8** %3, align 8
  %34 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %30, i32 0, i64 %32, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.35, i32 0, i32 0), i8* %33)
  br label %35

; <label>:35:                                     ; preds = %29, %2
  %36 = load i8, i8* %9, align 1
  %37 = sext i8 %36 to i32
  switch i32 %37, label %62 [
    i32 8, label %38
    i32 9, label %44
    i32 10, label %50
    i32 11, label %56
  ]

; <label>:38:                                     ; preds = %35
  %39 = load i8*, i8** %3, align 8
  %40 = load i8*, i8** %3, align 8
  %41 = call i64 @llvm.objectsize.i64.p0i8(i8* %40, i1 false, i1 true)
  %42 = load i8*, i8** %3, align 8
  %43 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %39, i32 0, i64 %41, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.88, i32 0, i32 0), i8* %42)
  br label %62

; <label>:44:                                     ; preds = %35
  %45 = load i8*, i8** %3, align 8
  %46 = load i8*, i8** %3, align 8
  %47 = call i64 @llvm.objectsize.i64.p0i8(i8* %46, i1 false, i1 true)
  %48 = load i8*, i8** %3, align 8
  %49 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %45, i32 0, i64 %47, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.89, i32 0, i32 0), i8* %48)
  br label %62

; <label>:50:                                     ; preds = %35
  %51 = load i8*, i8** %3, align 8
  %52 = load i8*, i8** %3, align 8
  %53 = call i64 @llvm.objectsize.i64.p0i8(i8* %52, i1 false, i1 true)
  %54 = load i8*, i8** %3, align 8
  %55 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %51, i32 0, i64 %53, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.90, i32 0, i32 0), i8* %54)
  br label %62

; <label>:56:                                     ; preds = %35
  %57 = load i8*, i8** %3, align 8
  %58 = load i8*, i8** %3, align 8
  %59 = call i64 @llvm.objectsize.i64.p0i8(i8* %58, i1 false, i1 true)
  %60 = load i8*, i8** %3, align 8
  %61 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %57, i32 0, i64 %59, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.91, i32 0, i32 0), i8* %60)
  br label %62

; <label>:62:                                     ; preds = %35, %56, %50, %44, %38
  %63 = load i8*, i8** %3, align 8
  %64 = load i8*, i8** %3, align 8
  %65 = call i64 @strlen(i8* %64)
  %66 = getelementptr inbounds i8, i8* %63, i64 %65
  %67 = load i32, i32* %7, align 4
  %68 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %66, i32 0, i64 -1, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.92, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %67, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  %69 = load i16, i16* %5, align 2
  %70 = zext i16 %69 to i32
  %71 = icmp eq i32 %70, 16
  br i1 %71, label %72, label %79

; <label>:72:                                     ; preds = %62
  %73 = load i8*, i8** %3, align 8
  %74 = load i8*, i8** %3, align 8
  %75 = call i64 @strlen(i8* %74)
  %76 = getelementptr inbounds i8, i8* %73, i64 %75
  %77 = load i8*, i8** %10, align 8
  %78 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %76, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.38, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.93, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %77, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %79

; <label>:79:                                     ; preds = %72, %62
  ret void
}

declare i32 @wvm_r_func(i64) #1

declare void @wvm_r_regs(i64, i8*, i8*, i8*) #1

declare i8* @wvm_decode_reg(i8 zeroext) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_disassemble_r_alt_op(i8*, i8 zeroext, i8 zeroext, i8 zeroext, i8*) #0 {
  %6 = alloca i8*, align 8
  %7 = alloca i8, align 1
  %8 = alloca i8, align 1
  %9 = alloca i8, align 1
  %10 = alloca i8*, align 8
  %11 = alloca i8*, align 8
  %12 = alloca i8*, align 8
  %13 = alloca i8*, align 8
  %14 = alloca i8*, align 8
  %15 = alloca i8*, align 8
  store i8* %0, i8** %6, align 8
  store i8 %1, i8* %7, align 1
  store i8 %2, i8* %8, align 1
  store i8 %3, i8* %9, align 1
  store i8* %4, i8** %10, align 8
  %16 = load i8, i8* %7, align 1
  %17 = call i8* @wvm_decode_reg(i8 zeroext %16)
  store i8* %17, i8** %11, align 8
  %18 = load i8, i8* %8, align 1
  %19 = call i8* @wvm_decode_reg(i8 zeroext %18)
  store i8* %19, i8** %12, align 8
  %20 = load i8, i8* %9, align 1
  %21 = call i8* @wvm_decode_reg(i8 zeroext %20)
  store i8* %21, i8** %13, align 8
  %22 = load i8, i8* %7, align 1
  %23 = zext i8 %22 to i32
  %24 = load i8, i8* %9, align 1
  %25 = zext i8 %24 to i32
  %26 = icmp eq i32 %23, %25
  br i1 %26, label %33, label %27

; <label>:27:                                     ; preds = %5
  %28 = load i8, i8* %8, align 1
  %29 = zext i8 %28 to i32
  %30 = load i8, i8* %9, align 1
  %31 = zext i8 %30 to i32
  %32 = icmp eq i32 %29, %31
  br i1 %32, label %33, label %63

; <label>:33:                                     ; preds = %27, %5
  %34 = load i8, i8* %7, align 1
  %35 = zext i8 %34 to i32
  %36 = load i8, i8* %9, align 1
  %37 = zext i8 %36 to i32
  %38 = icmp eq i32 %35, %37
  br i1 %38, label %39, label %41

; <label>:39:                                     ; preds = %33
  %40 = load i8*, i8** %11, align 8
  br label %43

; <label>:41:                                     ; preds = %33
  %42 = load i8*, i8** %12, align 8
  br label %43

; <label>:43:                                     ; preds = %41, %39
  %44 = phi i8* [ %40, %39 ], [ %42, %41 ]
  store i8* %44, i8** %14, align 8
  %45 = load i8, i8* %7, align 1
  %46 = zext i8 %45 to i32
  %47 = load i8, i8* %9, align 1
  %48 = zext i8 %47 to i32
  %49 = icmp eq i32 %46, %48
  br i1 %49, label %50, label %52

; <label>:50:                                     ; preds = %43
  %51 = load i8*, i8** %12, align 8
  br label %54

; <label>:52:                                     ; preds = %43
  %53 = load i8*, i8** %11, align 8
  br label %54

; <label>:54:                                     ; preds = %52, %50
  %55 = phi i8* [ %51, %50 ], [ %53, %52 ]
  store i8* %55, i8** %15, align 8
  %56 = load i8*, i8** %6, align 8
  %57 = load i8*, i8** %6, align 8
  %58 = call i64 @llvm.objectsize.i64.p0i8(i8* %57, i1 false, i1 true)
  %59 = load i8*, i8** %14, align 8
  %60 = load i8*, i8** %10, align 8
  %61 = load i8*, i8** %15, align 8
  %62 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %56, i32 0, i64 %58, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.61, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %59, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* %60, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %61, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %72

; <label>:63:                                     ; preds = %27
  %64 = load i8*, i8** %6, align 8
  %65 = load i8*, i8** %6, align 8
  %66 = call i64 @llvm.objectsize.i64.p0i8(i8* %65, i1 false, i1 true)
  %67 = load i8*, i8** %11, align 8
  %68 = load i8*, i8** %10, align 8
  %69 = load i8*, i8** %12, align 8
  %70 = load i8*, i8** %13, align 8
  %71 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %64, i32 0, i64 %66, i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str.62, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %67, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* %68, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %69, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %70, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %72

; <label>:72:                                     ; preds = %63, %54
  ret void
}

; Function Attrs: nounwind
declare i8* @__strcat_chk(i8*, i8*, i64) #4

declare i64 @strlen(i8*) #1

declare zeroext i8 @wvm_i_rs(i64) #1

declare zeroext i8 @wvm_i_rd(i64) #1

declare i32 @wvm_i_imm(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_disassemble_i_math(i8*, i8 zeroext, i8 zeroext, i32, i8 signext) #0 {
  %6 = alloca i8*, align 8
  %7 = alloca i8, align 1
  %8 = alloca i8, align 1
  %9 = alloca i32, align 4
  %10 = alloca i8, align 1
  store i8* %0, i8** %6, align 8
  store i8 %1, i8* %7, align 1
  store i8 %2, i8* %8, align 1
  store i32 %3, i32* %9, align 4
  store i8 %4, i8* %10, align 1
  %11 = load i8, i8* %7, align 1
  %12 = zext i8 %11 to i32
  %13 = load i8, i8* %8, align 1
  %14 = zext i8 %13 to i32
  %15 = icmp eq i32 %12, %14
  br i1 %15, label %16, label %41

; <label>:16:                                     ; preds = %5
  %17 = load i32, i32* %9, align 4
  %18 = icmp eq i32 %17, 1
  br i1 %18, label %19, label %30

; <label>:19:                                     ; preds = %16
  %20 = load i8*, i8** %6, align 8
  %21 = load i8*, i8** %6, align 8
  %22 = call i64 @llvm.objectsize.i64.p0i8(i8* %21, i1 false, i1 true)
  %23 = load i8, i8* %7, align 1
  %24 = call i8* @wvm_decode_reg(i8 zeroext %23)
  %25 = load i8, i8* %10, align 1
  %26 = sext i8 %25 to i32
  %27 = load i8, i8* %10, align 1
  %28 = sext i8 %27 to i32
  %29 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %20, i32 0, i64 %22, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.85, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %24, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i32 %26, i32 %28, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %40

; <label>:30:                                     ; preds = %16
  %31 = load i8*, i8** %6, align 8
  %32 = load i8*, i8** %6, align 8
  %33 = call i64 @llvm.objectsize.i64.p0i8(i8* %32, i1 false, i1 true)
  %34 = load i8, i8* %7, align 1
  %35 = call i8* @wvm_decode_reg(i8 zeroext %34)
  %36 = load i8, i8* %10, align 1
  %37 = sext i8 %36 to i32
  %38 = load i32, i32* %9, align 4
  %39 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %31, i32 0, i64 %33, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.86, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %35, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i32 %37, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %38, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %40

; <label>:40:                                     ; preds = %30, %19
  br label %53

; <label>:41:                                     ; preds = %5
  %42 = load i8*, i8** %6, align 8
  %43 = load i8*, i8** %6, align 8
  %44 = call i64 @llvm.objectsize.i64.p0i8(i8* %43, i1 false, i1 true)
  %45 = load i8, i8* %7, align 1
  %46 = call i8* @wvm_decode_reg(i8 zeroext %45)
  %47 = load i8, i8* %10, align 1
  %48 = sext i8 %47 to i32
  %49 = load i32, i32* %9, align 4
  %50 = load i8, i8* %8, align 1
  %51 = call i8* @wvm_decode_reg(i8 zeroext %50)
  %52 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %42, i32 0, i64 %44, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.87, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %46, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i32 %48, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %49, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %51, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %53

; <label>:53:                                     ; preds = %41, %40
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_disassemble_i_alt_op(i8*, i8 zeroext, i8 zeroext, i32, i8*) #0 {
  %6 = alloca i8*, align 8
  %7 = alloca i8, align 1
  %8 = alloca i8, align 1
  %9 = alloca i32, align 4
  %10 = alloca i8*, align 8
  store i8* %0, i8** %6, align 8
  store i8 %1, i8* %7, align 1
  store i8 %2, i8* %8, align 1
  store i32 %3, i32* %9, align 4
  store i8* %4, i8** %10, align 8
  %11 = load i8*, i8** %6, align 8
  %12 = load i8*, i8** %6, align 8
  %13 = call i64 @llvm.objectsize.i64.p0i8(i8* %12, i1 false, i1 true)
  %14 = load i8, i8* %7, align 1
  %15 = call i8* @wvm_decode_reg(i8 zeroext %14)
  %16 = load i8*, i8** %10, align 8
  %17 = load i8, i8* %7, align 1
  %18 = zext i8 %17 to i32
  %19 = load i8, i8* %8, align 1
  %20 = zext i8 %19 to i32
  %21 = icmp eq i32 %18, %20
  %22 = zext i1 %21 to i64
  %23 = select i1 %21, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.83, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.42, i32 0, i32 0)
  %24 = load i32, i32* %9, align 4
  %25 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %11, i32 0, i64 %13, i8* getelementptr inbounds ([24 x i8], [24 x i8]* @.str.82, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %15, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* %16, i8* %23, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %24, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  %26 = load i8, i8* %7, align 1
  %27 = zext i8 %26 to i32
  %28 = load i8, i8* %8, align 1
  %29 = zext i8 %28 to i32
  %30 = icmp ne i32 %27, %29
  br i1 %30, label %31, label %39

; <label>:31:                                     ; preds = %5
  %32 = load i8*, i8** %6, align 8
  %33 = load i8*, i8** %6, align 8
  %34 = call i64 @strlen(i8* %33)
  %35 = getelementptr inbounds i8, i8* %32, i64 %34
  %36 = load i8, i8* %8, align 1
  %37 = call i8* @wvm_decode_reg(i8 zeroext %36)
  %38 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %35, i32 0, i64 -1, i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.84, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %37, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  br label %39

; <label>:39:                                     ; preds = %31, %5
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_disassemble_i_comp(i8*, i8 zeroext, i8 zeroext, i32, i8*) #0 {
  %6 = alloca i8*, align 8
  %7 = alloca i8, align 1
  %8 = alloca i8, align 1
  %9 = alloca i32, align 4
  %10 = alloca i8*, align 8
  store i8* %0, i8** %6, align 8
  store i8 %1, i8* %7, align 1
  store i8 %2, i8* %8, align 1
  store i32 %3, i32* %9, align 4
  store i8* %4, i8** %10, align 8
  %11 = load i8*, i8** %6, align 8
  %12 = load i8*, i8** %6, align 8
  %13 = call i64 @llvm.objectsize.i64.p0i8(i8* %12, i1 false, i1 true)
  %14 = load i8, i8* %7, align 1
  %15 = call i8* @wvm_decode_reg(i8 zeroext %14)
  %16 = load i8*, i8** %10, align 8
  %17 = load i32, i32* %9, align 4
  %18 = load i8, i8* %8, align 1
  %19 = call i8* @wvm_decode_reg(i8 zeroext %18)
  %20 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %11, i32 0, i64 %13, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @.str.81, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %15, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.8, i32 0, i32 0), i8* %16, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.64, i32 0, i32 0), i32 %17, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.1, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.7, i32 0, i32 0), i8* %19, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0))
  ret void
}

declare zeroext i8 @wvm_j_rs(i64) #1

declare i32 @wvm_j_addr(i64) #1

declare signext i8 @wvm_j_link(i64) #1

declare signext i8 @wvm_j_cond(i64) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind readnone speculatable }
attributes #4 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { allocsize(0,1) }
attributes #6 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
