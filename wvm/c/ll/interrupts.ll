; ModuleID = 'interrupts.c'
source_filename = "interrupts.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }

@inttab = global i64 0, align 8
@__stderrp = external global %struct.__sFILE*, align 8
@.str = private unnamed_addr constant [23 x i8] c"Unknown interrupt: %u\0A\00", align 1
@pc = common global i64 0, align 8
@registers = common global [128 x i64] zeroinitializer, align 16
@cur_ring = common global i32 0, align 4
@memory = common global i8* null, align 8
@memsize = common global i64 0, align 8
@offset_symtab = common global i64 0, align 8
@offset_data = common global i64 0, align 8
@offset_code = common global i64 0, align 8
@offset_end = common global i64 0, align 8
@membytes = common global i64 0, align 8
@cycles = common global i32 0, align 4
@alive = common global i8 0, align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_interrupt(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  %6 = load i32, i32* %3, align 4
  %7 = icmp ult i32 3, %6
  br i1 %7, label %8, label %12

; <label>:8:                                      ; preds = %1
  %9 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %10 = load i32, i32* %3, align 4
  %11 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %9, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str, i32 0, i32 0), i32 %10)
  call void @exit(i32 1) #3
  unreachable

; <label>:12:                                     ; preds = %1
  %13 = call i32 @check_inttab()
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %15, label %18

; <label>:15:                                     ; preds = %12
  %16 = load i32, i32* %3, align 4
  %17 = icmp eq i32 %16, 0
  br i1 %17, label %18, label %19

; <label>:18:                                     ; preds = %15, %12
  store i32 1, i32* %2, align 4
  br label %33

; <label>:19:                                     ; preds = %15
  store i32 -2, i32* %5, align 4
  %20 = load i32, i32* %3, align 4
  switch i32 %20, label %24 [
    i32 1, label %21
    i32 2, label %22
    i32 3, label %23
  ]

; <label>:21:                                     ; preds = %19
  store i32 -1, i32* %4, align 4
  store i32 0, i32* %5, align 4
  br label %24

; <label>:22:                                     ; preds = %19
  store i32 0, i32* %4, align 4
  store i32 -1, i32* %5, align 4
  br label %24

; <label>:23:                                     ; preds = %19
  store i32 2, i32* %4, align 4
  store i32 0, i32* %5, align 4
  br label %24

; <label>:24:                                     ; preds = %19, %23, %22, %21
  %25 = load i32, i32* %5, align 4
  %26 = call i32 @check_ring(i32 %25)
  %27 = icmp ne i32 %26, 0
  br i1 %27, label %29, label %28

; <label>:28:                                     ; preds = %24
  store i32 1, i32* %2, align 4
  br label %33

; <label>:29:                                     ; preds = %24
  %30 = load i32, i32* %3, align 4
  %31 = load i32, i32* %4, align 4
  %32 = call i32 @wvm_force_interrupt(i32 %30, i32 %31)
  store i32 %32, i32* %2, align 4
  br label %33

; <label>:33:                                     ; preds = %29, %28, %18
  %34 = load i32, i32* %2, align 4
  ret i32 %34
}

declare i32 @fprintf(%struct.__sFILE*, i8*, ...) #1

; Function Attrs: noreturn
declare void @exit(i32) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @check_ring(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  %4 = load i32, i32* %3, align 4
  %5 = icmp ne i32 %4, -1
  br i1 %5, label %6, label %11

; <label>:6:                                      ; preds = %1
  %7 = load i32, i32* %3, align 4
  %8 = load i32, i32* @cur_ring, align 4
  %9 = icmp slt i32 %7, %8
  br i1 %9, label %10, label %11

; <label>:10:                                     ; preds = %6
  call void @int_protec()
  store i32 0, i32* %2, align 4
  br label %12

; <label>:11:                                     ; preds = %6, %1
  store i32 1, i32* %2, align 4
  br label %12

; <label>:12:                                     ; preds = %11, %10
  %13 = load i32, i32* %2, align 4
  ret i32 %13
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_force_interrupt(i32, i32) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  %6 = load i32, i32* %4, align 4
  %7 = icmp ult i32 3, %6
  br i1 %7, label %8, label %12

; <label>:8:                                      ; preds = %2
  %9 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %10 = load i32, i32* %4, align 4
  %11 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %9, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str, i32 0, i32 0), i32 %10)
  call void @exit(i32 1) #3
  unreachable

; <label>:12:                                     ; preds = %2
  %13 = call i32 @check_inttab()
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %15, label %18

; <label>:15:                                     ; preds = %12
  %16 = load i32, i32* %4, align 4
  %17 = icmp eq i32 %16, 0
  br i1 %17, label %18, label %19

; <label>:18:                                     ; preds = %15, %12
  store i32 1, i32* %3, align 4
  br label %35

; <label>:19:                                     ; preds = %15
  %20 = load i64, i64* @pc, align 8
  %21 = add nsw i64 %20, 8
  store i64 %21, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 122), align 16
  %22 = load i32, i32* @cur_ring, align 4
  %23 = sext i32 %22 to i64
  store i64 %23, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 123), align 8
  %24 = load i32, i32* %5, align 4
  %25 = icmp slt i32 -1, %24
  br i1 %25, label %26, label %28

; <label>:26:                                     ; preds = %19
  %27 = load i32, i32* %5, align 4
  store i32 %27, i32* @cur_ring, align 4
  br label %28

; <label>:28:                                     ; preds = %26, %19
  %29 = load i64, i64* @inttab, align 8
  %30 = load i32, i32* %4, align 4
  %31 = mul i32 8, %30
  %32 = zext i32 %31 to i64
  %33 = add i64 %29, %32
  %34 = call i64 @wvm_get_word(i64 %33)
  call void @wvm_jump(i64 %34)
  store i32 0, i32* %3, align 4
  br label %35

; <label>:35:                                     ; preds = %28, %18
  %36 = load i32, i32* %3, align 4
  ret i32 %36
}

declare void @wvm_jump(i64) #1

declare i64 @wvm_get_word(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @int_protec() #0 {
  %1 = call i32 @wvm_force_interrupt(i32 3, i32 -1)
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @check_inttab() #0 {
  %1 = alloca i32, align 4
  %2 = load i64, i64* @inttab, align 8
  %3 = icmp eq i64 %2, 0
  br i1 %3, label %4, label %5

; <label>:4:                                      ; preds = %0
  store i32 0, i32* %1, align 4
  br label %6

; <label>:5:                                      ; preds = %0
  store i32 1, i32* %1, align 4
  br label %6

; <label>:6:                                      ; preds = %5, %4
  %7 = load i32, i32* %1, align 4
  ret i32 %7
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
