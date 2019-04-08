source_filename = "liveness.pdf"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@i = global i32 5, align 4
@.str = private unnamed_addr constant [7 x i8] c"hello\0A\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"bye\00", align 1
@.str.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @liveness() #0 {
	%q = alloca i1, align 4 ; dummy variable
	br label %2

; <label>:2:                                      ; preds = %0, %7
	br i1 %q, label %3, label %11

; <label>:3:                                     ; preds = %2
	%w = alloca i32, align 4
	%x = alloca i32, align 4
	%y = alloca i32, align 4
	br i1 %q, label %4, label %8

; <label>:4:                                     ; preds = %3
	call void @strprint(i32 %w)
	br label %5

; <label>:5:                                     ; preds = %4, %6
	call void @strprint(i32 %y)
	br label %6

; <label>:6:                                     ; preds = %5, %9
	br i1 %q, label %5, label %7

; <label>:7:                                     ; preds = %6
	br label %2

; <label>:8:                                     ; preds = %3, %10
	br label %9

; <label>:9:                                     ; preds = %8
	call void @strprint(i32 %x)
	br i1 %q, label %6, label %10

; <label>:10:                                     ; preds = %9
	br label %8

; <label>:11:                                     ; preds = %2
	ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @strprint(i32) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i32, align 4
  store i8* %0, i8** %2, align 8
  store i32 0, i32* %3, align 4
	br label %4

; <label>:4:                                      ; preds = %19, %1
  %5 = load i8*, i8** %2, align 8
  %6 = load i32, i32* %3, align 4
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds i8, i8* %5, i64 %7
  %9 = load i8, i8* %8, align 1
  %10 = sext i8 %9 to i32
  %11 = icmp ne i32 %10, 0
	br i1 %11, label %12, label %22

; <label>:12:                                     ; preds = %4
  %13 = load i8*, i8** %2, align 8
  %14 = load i32, i32* %3, align 4
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds i8, i8* %13, i64 %15
  %17 = load i8, i8* %16, align 1
  %18 = sext i8 %17 to i64
	call void @_prc(i64 %18)
	br label %19

; <label>:19:                                     ; preds = %12
  %20 = load i32, i32* %3, align 4
  %21 = add nsw i32 %20, 1
  store i32 %21, i32* %3, align 4
	br label %4

; <label>:22:                                     ; preds = %4
  ret void
}

declare void @_prc(i64) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
