; ModuleID = 'wvm.c'
source_filename = "wvm.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }

@cur_ring = global i32 0, align 4
@pc = common global i64 0, align 8
@memsize = common global i64 0, align 8
@alive = common global i8 0, align 1
@membytes = common global i64 0, align 8
@cycles = common global i32 0, align 4
@memory = common global i8* null, align 8
@.str = private unnamed_addr constant [2 x i8] c"r\00", align 1
@__stderrp = external global %struct.__sFILE*, align 8
@.str.1 = private unnamed_addr constant [19 x i8] c"Couldn't open %s.\0A\00", align 1
@offset_symtab = common global i64 0, align 8
@offset_code = common global i64 0, align 8
@offset_data = common global i64 0, align 8
@offset_end = common global i64 0, align 8
@registers = common global [128 x i64] zeroinitializer, align 16
@.str.2 = private unnamed_addr constant [93 x i8] c"%s Couldn't find function for instruction %s0x%016llx%s (opcode %s%d%s) at address %s%lld%s\0A\00", align 1
@.str.3 = private unnamed_addr constant [27 x i8] c"\1B[2m[\1B[0;31m\E2\9C\98\1B[0;2m]\1B[0m\00", align 1
@.str.4 = private unnamed_addr constant [5 x i8] c"\1B[1m\00", align 1
@.str.5 = private unnamed_addr constant [5 x i8] c"\1B[0m\00", align 1
@.str.6 = private unnamed_addr constant [23 x i8] c"Unknown condition: %d\0A\00", align 1
@.str.7 = private unnamed_addr constant [73 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@.str.8 = private unnamed_addr constant [6 x i8] c"\1B[55G\00", align 1
@.str.9 = private unnamed_addr constant [81 x i8] c"        \E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\AC%s\E2\94\90\0A\00", align 1
@.str.10 = private unnamed_addr constant [43 x i8] c"        \E2\94\82    \1B[1mHexadecimal\1B[0m     \E2\94\82\00", align 1
@.str.11 = private unnamed_addr constant [36 x i8] c"     \1B[1mDecoded\1B[0m            \E2\94\82\00", align 1
@.str.12 = private unnamed_addr constant [98 x i8] c"\0A\E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC%s\E2\94\A4\0A\00", align 1
@.str.13 = private unnamed_addr constant [99 x i8] c"\E2\94\9C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC%s\E2\94\A4%s\0A\00", align 1
@.str.14 = private unnamed_addr constant [13 x i8] c"\E2\94\82\1B[38;5;8m\00", align 1
@.str.15 = private unnamed_addr constant [5 x i8] c"\1B[7m\00", align 1
@.str.16 = private unnamed_addr constant [61 x i8] c" %5lld \1B[0m\E2\94\82 \1B[38;5;7m0x\1B[0m\1B[1m%016llx\1B[0m \E2\94\82\1B[38;5;250m\00", align 1
@.str.17 = private unnamed_addr constant [10 x i8] c" %s%lld%s\00", align 1
@.str.18 = private unnamed_addr constant [6 x i8] c"\1B[35m\00", align 1
@.str.19 = private unnamed_addr constant [2 x i8] c" \00", align 1
@.str.20 = private unnamed_addr constant [6 x i8] c"%s.%s\00", align 1
@.str.21 = private unnamed_addr constant [5 x i8] c"\1B[2m\00", align 1
@.str.22 = private unnamed_addr constant [3 x i8] c"%c\00", align 1
@.str.23 = private unnamed_addr constant [16 x i8] c"\1B[48G%ssymtab%s\00", align 1
@.str.24 = private unnamed_addr constant [14 x i8] c"\1B[50G%scode%s\00", align 1
@.str.25 = private unnamed_addr constant [14 x i8] c"\1B[50G%sdata%s\00", align 1
@.str.26 = private unnamed_addr constant [13 x i8] c"\1B[51G%send%s\00", align 1
@.str.27 = private unnamed_addr constant [8 x i8] c"%s%s\E2\94\82\00", align 1
@.str.28 = private unnamed_addr constant [10 x i8] c" Metadata\00", align 1
@.str.29 = private unnamed_addr constant [14 x i8] c" Symbol Table\00", align 1
@.str.30 = private unnamed_addr constant [6 x i8] c" Data\00", align 1
@.str.31 = private unnamed_addr constant [4 x i8] c" %s\00", align 1
@.str.32 = private unnamed_addr constant [6 x i8] c"%s\E2\94\82\00", align 1
@.str.33 = private unnamed_addr constant [6 x i8] c" Code\00", align 1
@.str.34 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str.35 = private unnamed_addr constant [97 x i8] c"\E2\94\94\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\B4\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\B4%s\E2\94\98\0A\00", align 1
@inttab = common global i64 0, align 8

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i1 @wvm_init(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  store i64 0, i64* @pc, align 8
  store i64 0, i64* @memsize, align 8
  store i8 1, i8* @alive, align 1
  %3 = load i64, i64* %2, align 8
  %4 = mul nsw i64 %3, 8
  store i64 %4, i64* @membytes, align 8
  store i32 0, i32* @cycles, align 4
  %5 = load i64, i64* @membytes, align 8
  %6 = call i8* @calloc(i64 %5, i64 1) #4
  store i8* %6, i8** @memory, align 8
  %7 = icmp ne i8* %6, null
  ret i1 %7
}

; Function Attrs: allocsize(0,1)
declare i8* @calloc(i64, i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_free() #0 {
  %1 = load i8*, i8** @memory, align 8
  %2 = icmp ne i8* %1, null
  br i1 %2, label %3, label %5

; <label>:3:                                      ; preds = %0
  %4 = load i8*, i8** @memory, align 8
  call void @free(i8* %4)
  br label %5

; <label>:5:                                      ; preds = %3, %0
  ret void
}

declare void @free(i8*) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_load(i8*) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca %struct.__sFILE*, align 8
  %4 = alloca [20 x i8], align 16
  %5 = alloca i64, align 8
  store i8* %0, i8** %2, align 8
  %6 = load i8*, i8** %2, align 8
  %7 = call %struct.__sFILE* @"\01_fopen"(i8* %6, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i32 0, i32 0))
  store %struct.__sFILE* %7, %struct.__sFILE** %3, align 8
  %8 = load %struct.__sFILE*, %struct.__sFILE** %3, align 8
  %9 = icmp eq %struct.__sFILE* %8, null
  br i1 %9, label %10, label %14

; <label>:10:                                     ; preds = %1
  %11 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %12 = load i8*, i8** %2, align 8
  %13 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %11, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.1, i32 0, i32 0), i8* %12)
  call void @exit(i32 1) #5
  unreachable

; <label>:14:                                     ; preds = %1
  store i64 0, i64* @memsize, align 8
  br label %15

; <label>:15:                                     ; preds = %20, %14
  %16 = getelementptr inbounds [20 x i8], [20 x i8]* %4, i32 0, i32 0
  %17 = load %struct.__sFILE*, %struct.__sFILE** %3, align 8
  %18 = call i8* @fgets(i8* %16, i32 18, %struct.__sFILE* %17)
  %19 = icmp ne i8* %18, null
  br i1 %19, label %20, label %27

; <label>:20:                                     ; preds = %15
  %21 = getelementptr inbounds [20 x i8], [20 x i8]* %4, i32 0, i32 0
  %22 = call i64 @strtoul(i8* %21, i8** null, i32 16)
  store i64 %22, i64* %5, align 8
  %23 = load i64, i64* @memsize, align 8
  %24 = add i64 %23, 1
  store i64 %24, i64* @memsize, align 8
  %25 = mul i64 %23, 8
  %26 = load i64, i64* %5, align 8
  call void @wvm_set_word(i64 %25, i64 %26)
  br label %15

; <label>:27:                                     ; preds = %15
  %28 = load %struct.__sFILE*, %struct.__sFILE** %3, align 8
  %29 = call i32 @fclose(%struct.__sFILE* %28)
  %30 = call i64 @wvm_get_word(i64 0)
  store i64 %30, i64* @offset_symtab, align 8
  %31 = call i64 @wvm_get_word(i64 8)
  store i64 %31, i64* @offset_code, align 8
  %32 = call i64 @wvm_get_word(i64 16)
  store i64 %32, i64* @offset_data, align 8
  %33 = call i64 @wvm_get_word(i64 24)
  store i64 %33, i64* @offset_end, align 8
  %34 = load i64, i64* @memsize, align 8
  %35 = trunc i64 %34 to i32
  ret i32 %35
}

declare %struct.__sFILE* @"\01_fopen"(i8*, i8*) #2

declare i32 @fprintf(%struct.__sFILE*, i8*, ...) #2

; Function Attrs: noreturn
declare void @exit(i32) #3

declare i8* @fgets(i8*, i32, %struct.__sFILE*) #2

declare i64 @strtoul(i8*, i8**, i32) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_set_word(i64, i64) #0 {
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  store i64 %0, i64* %3, align 8
  store i64 %1, i64* %4, align 8
  %5 = load i64, i64* %4, align 8
  %6 = ashr i64 %5, 56
  %7 = and i64 %6, 255
  %8 = trunc i64 %7 to i8
  %9 = load i8*, i8** @memory, align 8
  %10 = load i64, i64* %3, align 8
  %11 = getelementptr inbounds i8, i8* %9, i64 %10
  store i8 %8, i8* %11, align 1
  %12 = load i64, i64* %4, align 8
  %13 = ashr i64 %12, 48
  %14 = and i64 %13, 255
  %15 = trunc i64 %14 to i8
  %16 = load i8*, i8** @memory, align 8
  %17 = load i64, i64* %3, align 8
  %18 = add nsw i64 %17, 1
  %19 = getelementptr inbounds i8, i8* %16, i64 %18
  store i8 %15, i8* %19, align 1
  %20 = load i64, i64* %4, align 8
  %21 = ashr i64 %20, 40
  %22 = and i64 %21, 255
  %23 = trunc i64 %22 to i8
  %24 = load i8*, i8** @memory, align 8
  %25 = load i64, i64* %3, align 8
  %26 = add nsw i64 %25, 2
  %27 = getelementptr inbounds i8, i8* %24, i64 %26
  store i8 %23, i8* %27, align 1
  %28 = load i64, i64* %4, align 8
  %29 = ashr i64 %28, 32
  %30 = and i64 %29, 255
  %31 = trunc i64 %30 to i8
  %32 = load i8*, i8** @memory, align 8
  %33 = load i64, i64* %3, align 8
  %34 = add nsw i64 %33, 3
  %35 = getelementptr inbounds i8, i8* %32, i64 %34
  store i8 %31, i8* %35, align 1
  %36 = load i64, i64* %4, align 8
  %37 = ashr i64 %36, 24
  %38 = and i64 %37, 255
  %39 = trunc i64 %38 to i8
  %40 = load i8*, i8** @memory, align 8
  %41 = load i64, i64* %3, align 8
  %42 = add nsw i64 %41, 4
  %43 = getelementptr inbounds i8, i8* %40, i64 %42
  store i8 %39, i8* %43, align 1
  %44 = load i64, i64* %4, align 8
  %45 = ashr i64 %44, 16
  %46 = and i64 %45, 255
  %47 = trunc i64 %46 to i8
  %48 = load i8*, i8** @memory, align 8
  %49 = load i64, i64* %3, align 8
  %50 = add nsw i64 %49, 5
  %51 = getelementptr inbounds i8, i8* %48, i64 %50
  store i8 %47, i8* %51, align 1
  %52 = load i64, i64* %4, align 8
  %53 = ashr i64 %52, 8
  %54 = and i64 %53, 255
  %55 = trunc i64 %54 to i8
  %56 = load i8*, i8** @memory, align 8
  %57 = load i64, i64* %3, align 8
  %58 = add nsw i64 %57, 6
  %59 = getelementptr inbounds i8, i8* %56, i64 %58
  store i8 %55, i8* %59, align 1
  %60 = load i64, i64* %4, align 8
  %61 = and i64 %60, 255
  %62 = trunc i64 %61 to i8
  %63 = load i8*, i8** @memory, align 8
  %64 = load i64, i64* %3, align 8
  %65 = add nsw i64 %64, 7
  %66 = getelementptr inbounds i8, i8* %63, i64 %65
  store i8 %62, i8* %66, align 1
  ret void
}

declare i32 @fclose(%struct.__sFILE*) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i64 @wvm_get_word(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  store i64 0, i64* %3, align 8
  store i8 0, i8* %4, align 1
  br label %5

; <label>:5:                                      ; preds = %26, %1
  %6 = load i8, i8* %4, align 1
  %7 = zext i8 %6 to i32
  %8 = icmp slt i32 %7, 8
  br i1 %8, label %9, label %29

; <label>:9:                                      ; preds = %5
  %10 = load i8*, i8** @memory, align 8
  %11 = load i64, i64* %2, align 8
  %12 = load i8, i8* %4, align 1
  %13 = zext i8 %12 to i64
  %14 = add nsw i64 %11, %13
  %15 = getelementptr inbounds i8, i8* %10, i64 %14
  %16 = load i8, i8* %15, align 1
  %17 = zext i8 %16 to i64
  %18 = load i8, i8* %4, align 1
  %19 = zext i8 %18 to i32
  %20 = sub nsw i32 7, %19
  %21 = mul nsw i32 %20, 8
  %22 = zext i32 %21 to i64
  %23 = shl i64 %17, %22
  %24 = load i64, i64* %3, align 8
  %25 = or i64 %24, %23
  store i64 %25, i64* %3, align 8
  br label %26

; <label>:26:                                     ; preds = %9
  %27 = load i8, i8* %4, align 1
  %28 = add i8 %27, 1
  store i8 %28, i8* %4, align 1
  br label %5

; <label>:29:                                     ; preds = %5
  %30 = load i64, i64* %3, align 8
  ret i64 %30
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_init_vm() #0 {
  %1 = load i64, i64* @offset_code, align 8
  store i64 %1, i64* @pc, align 8
  %2 = load i64, i64* @membytes, align 8
  %3 = sub nsw i64 %2, 8
  store i64 %3, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i8 @wvm_get_byte(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i8*, i8** @memory, align 8
  %4 = load i64, i64* %2, align 8
  %5 = getelementptr inbounds i8, i8* %3, i64 %4
  %6 = load i8, i8* %5, align 1
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_set_byte(i64, i8 zeroext) #0 {
  %3 = alloca i64, align 8
  %4 = alloca i8, align 1
  store i64 %0, i64* %3, align 8
  store i8 %1, i8* %4, align 1
  %5 = load i8, i8* %4, align 1
  %6 = load i8*, i8** @memory, align 8
  %7 = load i64, i64* %3, align 8
  %8 = getelementptr inbounds i8, i8* %6, i64 %7
  store i8 %5, i8* %8, align 1
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i8* @wvm_get_string(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i32, align 4
  %4 = alloca i8*, align 8
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  store i32 0, i32* %3, align 4
  br label %6

; <label>:6:                                      ; preds = %17, %1
  %7 = load i8*, i8** @memory, align 8
  %8 = load i64, i64* %2, align 8
  %9 = load i32, i32* %3, align 4
  %10 = sext i32 %9 to i64
  %11 = add nsw i64 %8, %10
  %12 = getelementptr inbounds i8, i8* %7, i64 %11
  %13 = load i8, i8* %12, align 1
  %14 = zext i8 %13 to i32
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %16, label %20

; <label>:16:                                     ; preds = %6
  br label %17

; <label>:17:                                     ; preds = %16
  %18 = load i32, i32* %3, align 4
  %19 = add nsw i32 %18, 1
  store i32 %19, i32* %3, align 4
  br label %6

; <label>:20:                                     ; preds = %6
  %21 = load i32, i32* %3, align 4
  %22 = add nsw i32 %21, 1
  %23 = sext i32 %22 to i64
  %24 = call i8* @calloc(i64 %23, i64 1) #4
  store i8* %24, i8** %4, align 8
  store i32 0, i32* %5, align 4
  br label %25

; <label>:25:                                     ; preds = %41, %20
  %26 = load i32, i32* %5, align 4
  %27 = load i32, i32* %3, align 4
  %28 = icmp slt i32 %26, %27
  br i1 %28, label %29, label %44

; <label>:29:                                     ; preds = %25
  %30 = load i8*, i8** @memory, align 8
  %31 = load i64, i64* %2, align 8
  %32 = load i32, i32* %5, align 4
  %33 = sext i32 %32 to i64
  %34 = add nsw i64 %31, %33
  %35 = getelementptr inbounds i8, i8* %30, i64 %34
  %36 = load i8, i8* %35, align 1
  %37 = load i8*, i8** %4, align 8
  %38 = load i32, i32* %5, align 4
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds i8, i8* %37, i64 %39
  store i8 %36, i8* %40, align 1
  br label %41

; <label>:41:                                     ; preds = %29
  %42 = load i32, i32* %5, align 4
  %43 = add nsw i32 %42, 1
  store i32 %43, i32* %5, align 4
  br label %25

; <label>:44:                                     ; preds = %25
  %45 = load i8*, i8** %4, align 8
  %46 = load i32, i32* %3, align 4
  %47 = sext i32 %46 to i64
  %48 = getelementptr inbounds i8, i8* %45, i64 %47
  store i8 0, i8* %48, align 1
  %49 = load i8*, i8** %4, align 8
  ret i8* %49
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_jump(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  store i64 %3, i64* @pc, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_link() #0 {
  %1 = load i64, i64* @pc, align 8
  %2 = add nsw i64 %1, 8
  store i64 %2, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 4), align 16
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_increment_pc() #0 {
  %1 = load i64, i64* @pc, align 8
  %2 = add nsw i64 %1, 8
  store i64 %2, i64* @pc, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_change_ring(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  %4 = load i32, i32* %3, align 4
  %5 = icmp slt i32 %4, 0
  br i1 %5, label %9, label %6

; <label>:6:                                      ; preds = %1
  %7 = load i32, i32* %3, align 4
  %8 = icmp slt i32 3, %7
  br i1 %8, label %9, label %10

; <label>:9:                                      ; preds = %6, %1
  br label %18

; <label>:10:                                     ; preds = %6
  %11 = load i32, i32* %3, align 4
  %12 = load i32, i32* @cur_ring, align 4
  %13 = icmp slt i32 %11, %12
  br i1 %13, label %14, label %15

; <label>:14:                                     ; preds = %10
  br label %17

; <label>:15:                                     ; preds = %10
  %16 = load i32, i32* %3, align 4
  store i32 %16, i32* @cur_ring, align 4
  store i32 1, i32* %2, align 4
  br label %19

; <label>:17:                                     ; preds = %14
  br label %18

; <label>:18:                                     ; preds = %17, %9
  call void (...) @int_protec()
  store i32 0, i32* %2, align 4
  br label %19

; <label>:19:                                     ; preds = %18, %15
  %20 = load i32, i32* %2, align 4
  ret i32 %20
}

declare void @int_protec(...) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i1 @wvm_tick() #0 {
  %1 = alloca i64, align 8
  %2 = alloca void (i64)*, align 8
  %3 = load i64, i64* @pc, align 8
  %4 = call i64 @wvm_get_word(i64 %3)
  store i64 %4, i64* %1, align 8
  %5 = load i64, i64* %1, align 8
  %6 = call void (i64)* @wvm_get_fn(i64 %5)
  store void (i64)* %6, void (i64)** %2, align 8
  %7 = load void (i64)*, void (i64)** %2, align 8
  %8 = icmp eq void (i64)* %7, null
  br i1 %8, label %9, label %17

; <label>:9:                                      ; preds = %0
  %10 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %11 = load i64, i64* %1, align 8
  %12 = load i64, i64* %1, align 8
  %13 = call zeroext i16 @wvm_get_opcode(i64 %12)
  %14 = zext i16 %13 to i32
  %15 = load i64, i64* @pc, align 8
  %16 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %10, i8* getelementptr inbounds ([93 x i8], [93 x i8]* @.str.2, i32 0, i32 0), i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.3, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.4, i32 0, i32 0), i64 %11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.4, i32 0, i32 0), i32 %14, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.4, i32 0, i32 0), i64 %15, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  call void @exit(i32 1) #5
  unreachable

; <label>:17:                                     ; preds = %0
  %18 = load void (i64)*, void (i64)** %2, align 8
  %19 = load i64, i64* %1, align 8
  call void %18(i64 %19)
  %20 = load i32, i32* @cycles, align 4
  %21 = add nsw i32 %20, 1
  store i32 %21, i32* @cycles, align 4
  %22 = load i8, i8* @alive, align 1
  %23 = trunc i8 %22 to i1
  ret i1 %23
}

declare void (i64)* @wvm_get_fn(i64) #2

declare zeroext i16 @wvm_get_opcode(i64) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_alu_flags_clear() #0 {
  %1 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %2 = and i64 %1, -16
  store i64 %2, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_alu_flags_update(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  call void @wvm_alu_flags_clear()
  %3 = load i64, i64* %2, align 8
  %4 = icmp eq i64 %3, 0
  br i1 %4, label %5, label %9

; <label>:5:                                      ; preds = %1
  %6 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %7 = and i64 %6, -2
  %8 = or i64 %7, 1
  store i64 %8, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  br label %17

; <label>:9:                                      ; preds = %1
  %10 = load i64, i64* %2, align 8
  %11 = icmp slt i64 %10, 0
  br i1 %11, label %12, label %16

; <label>:12:                                     ; preds = %9
  %13 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %14 = and i64 %13, -3
  %15 = or i64 %14, 2
  store i64 %15, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  br label %16

; <label>:16:                                     ; preds = %12, %9
  br label %17

; <label>:17:                                     ; preds = %16, %5
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_check_condition(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  %4 = load i32, i32* %3, align 4
  switch i32 %4, label %32 [
    i32 8, label %5
    i32 9, label %17
    i32 10, label %21
    i32 11, label %25
    i32 0, label %31
  ]

; <label>:5:                                      ; preds = %1
  %6 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %7 = and i64 %6, 2
  %8 = icmp ne i64 %7, 0
  br i1 %8, label %14, label %9

; <label>:9:                                      ; preds = %5
  %10 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %11 = and i64 %10, 1
  %12 = icmp ne i64 %11, 0
  %13 = xor i1 %12, true
  br label %14

; <label>:14:                                     ; preds = %9, %5
  %15 = phi i1 [ false, %5 ], [ %13, %9 ]
  %16 = zext i1 %15 to i32
  store i32 %16, i32* %2, align 4
  br label %36

; <label>:17:                                     ; preds = %1
  %18 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %19 = and i64 %18, 2
  %20 = trunc i64 %19 to i32
  store i32 %20, i32* %2, align 4
  br label %36

; <label>:21:                                     ; preds = %1
  %22 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %23 = and i64 %22, 1
  %24 = trunc i64 %23 to i32
  store i32 %24, i32* %2, align 4
  br label %36

; <label>:25:                                     ; preds = %1
  %26 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %27 = and i64 %26, 1
  %28 = icmp ne i64 %27, 0
  %29 = xor i1 %28, true
  %30 = zext i1 %29 to i32
  store i32 %30, i32* %2, align 4
  br label %36

; <label>:31:                                     ; preds = %1
  store i32 1, i32* %2, align 4
  br label %36

; <label>:32:                                     ; preds = %1
  %33 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %34 = load i32, i32* %3, align 4
  %35 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %33, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.6, i32 0, i32 0), i32 %34)
  call void @exit(i32 2) #5
  unreachable

; <label>:36:                                     ; preds = %31, %25, %21, %17, %14
  %37 = load i32, i32* %2, align 4
  ret i32 %37
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_print_memory() #0 {
  %1 = alloca i8*, align 8
  %2 = alloca i8*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i8, align 1
  %7 = alloca i8, align 1
  %8 = alloca i8*, align 8
  store i8* getelementptr inbounds ([73 x i8], [73 x i8]* @.str.7, i32 0, i32 0), i8** %1, align 8
  store i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.8, i32 0, i32 0), i8** %2, align 8
  %9 = load i8*, i8** %1, align 8
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([81 x i8], [81 x i8]* @.str.9, i32 0, i32 0), i8* %9)
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([43 x i8], [43 x i8]* @.str.10, i32 0, i32 0))
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @.str.11, i32 0, i32 0))
  %13 = load i8*, i8** %1, align 8
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([98 x i8], [98 x i8]* @.str.12, i32 0, i32 0), i8* %13)
  store i32 0, i32* %3, align 4
  br label %15

; <label>:15:                                     ; preds = %174, %0
  %16 = load i32, i32* %3, align 4
  %17 = sext i32 %16 to i64
  %18 = load i64, i64* @memsize, align 8
  %19 = icmp ult i64 %17, %18
  br i1 %19, label %20, label %177

; <label>:20:                                     ; preds = %15
  %21 = load i32, i32* %3, align 4
  %22 = shl i32 %21, 3
  %23 = sext i32 %22 to i64
  store i64 %23, i64* %4, align 8
  %24 = load i64, i64* %4, align 8
  %25 = call i64 @wvm_get_word(i64 %24)
  store i64 %25, i64* %5, align 8
  %26 = load i64, i64* %4, align 8
  %27 = load i64, i64* @offset_symtab, align 8
  %28 = icmp eq i64 %26, %27
  br i1 %28, label %37, label %29

; <label>:29:                                     ; preds = %20
  %30 = load i64, i64* %4, align 8
  %31 = load i64, i64* @offset_code, align 8
  %32 = icmp eq i64 %30, %31
  br i1 %32, label %37, label %33

; <label>:33:                                     ; preds = %29
  %34 = load i64, i64* %4, align 8
  %35 = load i64, i64* @offset_data, align 8
  %36 = icmp eq i64 %34, %35
  br i1 %36, label %37, label %40

; <label>:37:                                     ; preds = %33, %29, %20
  %38 = load i8*, i8** %1, align 8
  %39 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([99 x i8], [99 x i8]* @.str.13, i32 0, i32 0), i8* %38, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %40

; <label>:40:                                     ; preds = %37, %33
  %41 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.14, i32 0, i32 0))
  %42 = load i64, i64* %4, align 8
  %43 = load i64, i64* @pc, align 8
  %44 = icmp eq i64 %42, %43
  br i1 %44, label %45, label %47

; <label>:45:                                     ; preds = %40
  %46 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.15, i32 0, i32 0))
  br label %47

; <label>:47:                                     ; preds = %45, %40
  %48 = load i64, i64* %4, align 8
  %49 = load i64, i64* %5, align 8
  %50 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([61 x i8], [61 x i8]* @.str.16, i32 0, i32 0), i64 %48, i64 %49)
  %51 = load i32, i32* %3, align 4
  %52 = icmp slt i32 %51, 4
  br i1 %52, label %53, label %56

; <label>:53:                                     ; preds = %47
  %54 = load i64, i64* %5, align 8
  %55 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.17, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.18, i32 0, i32 0), i64 %54, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %97

; <label>:56:                                     ; preds = %47
  %57 = load i64, i64* %4, align 8
  %58 = load i64, i64* @offset_code, align 8
  %59 = icmp slt i64 %57, %58
  br i1 %59, label %64, label %60

; <label>:60:                                     ; preds = %56
  %61 = load i64, i64* @offset_data, align 8
  %62 = load i64, i64* %4, align 8
  %63 = icmp sle i64 %61, %62
  br i1 %63, label %64, label %96

; <label>:64:                                     ; preds = %60, %56
  %65 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.19, i32 0, i32 0))
  store i8 56, i8* %6, align 1
  br label %66

; <label>:66:                                     ; preds = %90, %64
  %67 = load i8, i8* %6, align 1
  %68 = sext i8 %67 to i32
  %69 = icmp sge i32 %68, 0
  br i1 %69, label %70, label %95

; <label>:70:                                     ; preds = %66
  %71 = load i64, i64* %5, align 8
  %72 = load i8, i8* %6, align 1
  %73 = sext i8 %72 to i32
  %74 = zext i32 %73 to i64
  %75 = ashr i64 %71, %74
  %76 = trunc i64 %75 to i8
  %77 = sext i8 %76 to i32
  %78 = and i32 %77, 255
  %79 = trunc i32 %78 to i8
  store i8 %79, i8* %7, align 1
  %80 = load i8, i8* %7, align 1
  %81 = sext i8 %80 to i32
  %82 = icmp slt i32 %81, 32
  br i1 %82, label %83, label %85

; <label>:83:                                     ; preds = %70
  %84 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.20, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.21, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %89

; <label>:85:                                     ; preds = %70
  %86 = load i8, i8* %7, align 1
  %87 = sext i8 %86 to i32
  %88 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.22, i32 0, i32 0), i32 %87)
  br label %89

; <label>:89:                                     ; preds = %85, %83
  br label %90

; <label>:90:                                     ; preds = %89
  %91 = load i8, i8* %6, align 1
  %92 = sext i8 %91 to i32
  %93 = sub nsw i32 %92, 8
  %94 = trunc i32 %93 to i8
  store i8 %94, i8* %6, align 1
  br label %66

; <label>:95:                                     ; preds = %66
  br label %96

; <label>:96:                                     ; preds = %95, %60
  br label %97

; <label>:97:                                     ; preds = %96, %53
  %98 = load i32, i32* %3, align 4
  %99 = icmp eq i32 %98, 0
  br i1 %99, label %100, label %102

; <label>:100:                                    ; preds = %97
  %101 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.23, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.21, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %120

; <label>:102:                                    ; preds = %97
  %103 = load i32, i32* %3, align 4
  %104 = icmp eq i32 %103, 1
  br i1 %104, label %105, label %107

; <label>:105:                                    ; preds = %102
  %106 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.24, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.21, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %119

; <label>:107:                                    ; preds = %102
  %108 = load i32, i32* %3, align 4
  %109 = icmp eq i32 %108, 2
  br i1 %109, label %110, label %112

; <label>:110:                                    ; preds = %107
  %111 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.25, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.21, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %118

; <label>:112:                                    ; preds = %107
  %113 = load i32, i32* %3, align 4
  %114 = icmp eq i32 %113, 3
  br i1 %114, label %115, label %117

; <label>:115:                                    ; preds = %112
  %116 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.26, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.21, i32 0, i32 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %117

; <label>:117:                                    ; preds = %115, %112
  br label %118

; <label>:118:                                    ; preds = %117, %110
  br label %119

; <label>:119:                                    ; preds = %118, %105
  br label %120

; <label>:120:                                    ; preds = %119, %100
  %121 = load i64, i64* %4, align 8
  %122 = load i64, i64* @offset_code, align 8
  %123 = icmp slt i64 %121, %122
  br i1 %123, label %128, label %124

; <label>:124:                                    ; preds = %120
  %125 = load i64, i64* @offset_data, align 8
  %126 = load i64, i64* %4, align 8
  %127 = icmp sle i64 %125, %126
  br i1 %127, label %128, label %131

; <label>:128:                                    ; preds = %124, %120
  %129 = load i8*, i8** %2, align 8
  %130 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.27, i32 0, i32 0), i8* %129, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.5, i32 0, i32 0))
  br label %131

; <label>:131:                                    ; preds = %128, %124
  %132 = load i64, i64* %4, align 8
  %133 = icmp eq i64 %132, 0
  br i1 %133, label %134, label %136

; <label>:134:                                    ; preds = %131
  %135 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.28, i32 0, i32 0))
  br label %150

; <label>:136:                                    ; preds = %131
  %137 = load i64, i64* %4, align 8
  %138 = load i64, i64* @offset_symtab, align 8
  %139 = icmp eq i64 %137, %138
  br i1 %139, label %140, label %142

; <label>:140:                                    ; preds = %136
  %141 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.29, i32 0, i32 0))
  br label %149

; <label>:142:                                    ; preds = %136
  %143 = load i64, i64* %4, align 8
  %144 = load i64, i64* @offset_data, align 8
  %145 = icmp eq i64 %143, %144
  br i1 %145, label %146, label %148

; <label>:146:                                    ; preds = %142
  %147 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.30, i32 0, i32 0))
  br label %148

; <label>:148:                                    ; preds = %146, %142
  br label %149

; <label>:149:                                    ; preds = %148, %140
  br label %150

; <label>:150:                                    ; preds = %149, %134
  %151 = load i64, i64* @offset_code, align 8
  %152 = load i64, i64* %4, align 8
  %153 = icmp sle i64 %151, %152
  br i1 %153, label %154, label %166

; <label>:154:                                    ; preds = %150
  %155 = load i64, i64* %4, align 8
  %156 = load i64, i64* @offset_data, align 8
  %157 = icmp slt i64 %155, %156
  br i1 %157, label %158, label %166

; <label>:158:                                    ; preds = %154
  %159 = load i64, i64* %5, align 8
  %160 = call i8* @wvm_disassemble(i64 %159)
  store i8* %160, i8** %8, align 8
  %161 = load i8*, i8** %8, align 8
  %162 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.31, i32 0, i32 0), i8* %161)
  %163 = load i8*, i8** %8, align 8
  call void @free(i8* %163)
  %164 = load i8*, i8** %2, align 8
  %165 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.32, i32 0, i32 0), i8* %164)
  br label %166

; <label>:166:                                    ; preds = %158, %154, %150
  %167 = load i64, i64* %4, align 8
  %168 = load i64, i64* @offset_code, align 8
  %169 = icmp eq i64 %167, %168
  br i1 %169, label %170, label %172

; <label>:170:                                    ; preds = %166
  %171 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.33, i32 0, i32 0))
  br label %172

; <label>:172:                                    ; preds = %170, %166
  %173 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.34, i32 0, i32 0))
  br label %174

; <label>:174:                                    ; preds = %172
  %175 = load i32, i32* %3, align 4
  %176 = add nsw i32 %175, 1
  store i32 %176, i32* %3, align 4
  br label %15

; <label>:177:                                    ; preds = %15
  %178 = load i8*, i8** %1, align 8
  %179 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([97 x i8], [97 x i8]* @.str.35, i32 0, i32 0), i8* %178)
  ret void
}

declare i32 @printf(i8*, ...) #2

declare i8* @wvm_disassemble(i64) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { allocsize(0,1) }
attributes #5 = { noreturn }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
