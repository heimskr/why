; ModuleID = 'main.c'
source_filename = "main.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }

@__stderrp = external global %struct.__sFILE*, align 8
@.str = private unnamed_addr constant [27 x i8] c"Couldn't allocate memory.\0A\00", align 1
@.str.1 = private unnamed_addr constant [31 x i8] c"../../wasm/compiled/memory.why\00", align 1
@.str.2 = private unnamed_addr constant [3 x i8] c"-c\00", align 1
@.str.3 = private unnamed_addr constant [27 x i8] c"../../wasm/compiled/%s.why\00", align 1
@.str.4 = private unnamed_addr constant [46 x i8] c"\0A\1B[31mExecution halted after %d cycle%s.\1B[0m\0A\00", align 1
@cycles = common global i32 0, align 4
@.str.5 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@.str.6 = private unnamed_addr constant [2 x i8] c"s\00", align 1
@memory = common global i8* null, align 8
@memsize = common global i64 0, align 8
@pc = common global i64 0, align 8
@offset_symtab = common global i64 0, align 8
@offset_data = common global i64 0, align 8
@offset_code = common global i64 0, align 8
@offset_end = common global i64 0, align 8
@registers = common global [128 x i64] zeroinitializer, align 16
@membytes = common global i64 0, align 8
@alive = common global i8 0, align 1
@cur_ring = common global i32 0, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main(i32, i8**) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i8*, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %7 = call zeroext i1 @wvm_init(i64 16777215)
  br i1 %7, label %11, label %8

; <label>:8:                                      ; preds = %2
  %9 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %10 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %9, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str, i32 0, i32 0))
  call void @exit(i32 1) #5
  unreachable

; <label>:11:                                     ; preds = %2
  %12 = load i32, i32* %4, align 4
  %13 = icmp eq i32 %12, 1
  br i1 %13, label %14, label %16

; <label>:14:                                     ; preds = %11
  %15 = call i32 @wvm_load(i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.1, i32 0, i32 0))
  br label %53

; <label>:16:                                     ; preds = %11
  %17 = load i32, i32* %4, align 4
  %18 = icmp eq i32 %17, 2
  br i1 %18, label %19, label %24

; <label>:19:                                     ; preds = %16
  %20 = load i8**, i8*** %5, align 8
  %21 = getelementptr inbounds i8*, i8** %20, i64 1
  %22 = load i8*, i8** %21, align 8
  %23 = call i32 @wvm_load(i8* %22)
  br label %52

; <label>:24:                                     ; preds = %16
  %25 = load i32, i32* %4, align 4
  %26 = icmp eq i32 %25, 3
  br i1 %26, label %27, label %51

; <label>:27:                                     ; preds = %24
  %28 = load i8**, i8*** %5, align 8
  %29 = getelementptr inbounds i8*, i8** %28, i64 1
  %30 = load i8*, i8** %29, align 8
  %31 = call i32 @strcmp(i8* %30, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i32 0, i32 0))
  %32 = icmp eq i32 %31, 0
  br i1 %32, label %33, label %51

; <label>:33:                                     ; preds = %27
  %34 = load i8**, i8*** %5, align 8
  %35 = getelementptr inbounds i8*, i8** %34, i64 2
  %36 = load i8*, i8** %35, align 8
  %37 = call i64 @strlen(i8* %36)
  %38 = add i64 20, %37
  %39 = add i64 %38, 4
  %40 = add i64 %39, 1
  %41 = call i8* @calloc(i64 %40, i64 1) #6
  store i8* %41, i8** %6, align 8
  %42 = load i8*, i8** %6, align 8
  %43 = load i8*, i8** %6, align 8
  %44 = call i64 @llvm.objectsize.i64.p0i8(i8* %43, i1 false, i1 true)
  %45 = load i8**, i8*** %5, align 8
  %46 = getelementptr inbounds i8*, i8** %45, i64 2
  %47 = load i8*, i8** %46, align 8
  %48 = call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %42, i32 0, i64 %44, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.3, i32 0, i32 0), i8* %47)
  %49 = load i8*, i8** %6, align 8
  %50 = call i32 @wvm_load(i8* %49)
  br label %51

; <label>:51:                                     ; preds = %33, %27, %24
  br label %52

; <label>:52:                                     ; preds = %51, %19
  br label %53

; <label>:53:                                     ; preds = %52, %14
  call void (...) @wvm_init_vm()
  call void (...) @wvm_print_memory()
  br label %54

; <label>:54:                                     ; preds = %56, %53
  %55 = call zeroext i1 (...) @wvm_tick()
  br i1 %55, label %56, label %57

; <label>:56:                                     ; preds = %54
  br label %54

; <label>:57:                                     ; preds = %54
  %58 = load i32, i32* @cycles, align 4
  %59 = load i32, i32* @cycles, align 4
  %60 = icmp eq i32 %59, 1
  %61 = zext i1 %60 to i64
  %62 = select i1 %60, i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.5, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.6, i32 0, i32 0)
  %63 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([46 x i8], [46 x i8]* @.str.4, i32 0, i32 0), i32 %58, i8* %62)
  call void (...) @wvm_free()
  ret i32 0
}

declare zeroext i1 @wvm_init(i64) #1

declare i32 @fprintf(%struct.__sFILE*, i8*, ...) #1

; Function Attrs: noreturn
declare void @exit(i32) #2

declare i32 @wvm_load(i8*) #1

declare i32 @strcmp(i8*, i8*) #1

; Function Attrs: allocsize(0,1)
declare i8* @calloc(i64, i64) #3

declare i64 @strlen(i8*) #1

declare i32 @__sprintf_chk(i8*, i32, i64, i8*, ...) #1

; Function Attrs: nounwind readnone speculatable
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1, i1) #4

declare void @wvm_init_vm(...) #1

declare void @wvm_print_memory(...) #1

declare zeroext i1 @wvm_tick(...) #1

declare i32 @printf(i8*, ...) #1

declare void @wvm_free(...) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind readnone speculatable }
attributes #5 = { noreturn }
attributes #6 = { allocsize(0,1) }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
