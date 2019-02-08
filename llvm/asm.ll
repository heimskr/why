; ModuleID = 'test/asm.c'
source_filename = "test/asm.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@i = global i32 5, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @test1() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = load i32, i32* @i, align 4
  %6 = add nsw i32 %5, 1
  store i32 %6, i32* %3, align 4
  %7 = load i32, i32* %3, align 4
  %8 = sub nsw i32 %7, 2
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %3, align 4
  %10 = load i32, i32* %4, align 4
  %11 = call { i32, i32 } asm "$2 + $3 -> $0; [$4] -> $1; ring $3; ring $4", "=r,=y,{ax},{bx},e,~{cc},~{dirflag},~{fpsr},~{flags}"(i32 %9, i32 %10, i64 664) #1, !srcloc !3
  %12 = extractvalue { i32, i32 } %11, 0
  %13 = extractvalue { i32, i32 } %11, 1
  store i32 %12, i32* %1, align 4
  store i32 %13, i32* %2, align 4
  %14 = load i32, i32* %1, align 4
  ret i32 %14
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @test2() #0 {
  %1 = load i32, i32* @i, align 4
  call void asm sideeffect "ring $$a0", "{ax},~{dirflag},~{fpsr},~{flags}"(i32 %1) #2, !srcloc !4
  ret void
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
!3 = !{i32 106}
!4 = !{i32 273}
