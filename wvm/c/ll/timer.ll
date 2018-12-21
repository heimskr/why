; ModuleID = 'timer.c'
source_filename = "timer.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct._opaque_pthread_mutex_t = type { i64, [56 x i8] }
%struct._opaque_pthread_t = type { i64, %struct.__darwin_pthread_handler_rec*, [8176 x i8] }
%struct.__darwin_pthread_handler_rec = type { void (i8*)*, i8*, %struct.__darwin_pthread_handler_rec* }
%struct._opaque_pthread_attr_t = type { i64, [56 x i8] }

@timer_mutex = global %struct._opaque_pthread_mutex_t { i64 850045863, [56 x i8] zeroinitializer }, align 8
@timer_delay = common global i64 0, align 8
@timer_thread = common global %struct._opaque_pthread_t* null, align 8
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
define i32 @wvm_set_timer(i64) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i64, align 8
  %4 = alloca i32, align 4
  store i64 %0, i64* %3, align 8
  %5 = call i32 @pthread_mutex_trylock(%struct._opaque_pthread_mutex_t* @timer_mutex)
  %6 = icmp eq i32 %5, 16
  br i1 %6, label %7, label %8

; <label>:7:                                      ; preds = %1
  store i32 0, i32* %2, align 4
  br label %12

; <label>:8:                                      ; preds = %1
  %9 = load i64, i64* %3, align 8
  store i64 %9, i64* @timer_delay, align 8
  %10 = call i32 @pthread_create(%struct._opaque_pthread_t** @timer_thread, %struct._opaque_pthread_attr_t* null, i8* (i8*)* @wvm_timer_thread, i8* null)
  store i32 %10, i32* %4, align 4
  %11 = load i32, i32* %4, align 4
  store i32 %11, i32* %2, align 4
  br label %12

; <label>:12:                                     ; preds = %8, %7
  %13 = load i32, i32* %2, align 4
  ret i32 %13
}

declare i32 @pthread_mutex_trylock(%struct._opaque_pthread_mutex_t*) #1

declare i32 @pthread_create(%struct._opaque_pthread_t**, %struct._opaque_pthread_attr_t*, i8* (i8*)*, i8*) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal i8* @wvm_timer_thread(i8*) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = load i64, i64* @timer_delay, align 8
  %4 = trunc i64 %3 to i32
  %5 = call i32 @"\01_usleep"(i32 %4)
  %6 = call i32 @wvm_force_interrupt(i32 2, i32 0)
  %7 = call i32 @pthread_mutex_unlock(%struct._opaque_pthread_mutex_t* @timer_mutex)
  ret i8* null
}

declare i32 @"\01_usleep"(i32) #1

declare i32 @wvm_force_interrupt(i32, i32) #1

declare i32 @pthread_mutex_unlock(%struct._opaque_pthread_mutex_t*) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
