; ModuleID = 'main.c'
source_filename = "main.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }

@__stderrp = external local_unnamed_addr global %struct.__sFILE*, align 8
@.str = private unnamed_addr constant [27 x i8] c"Couldn't allocate memory.\0A\00", align 1
@.str.1 = private unnamed_addr constant [31 x i8] c"../../wasm/compiled/memory.why\00", align 1
@.str.2 = private unnamed_addr constant [3 x i8] c"-c\00", align 1
@.str.3 = private unnamed_addr constant [27 x i8] c"../../wasm/compiled/%s.why\00", align 1
@.str.4 = private unnamed_addr constant [46 x i8] c"\0A\1B[31mExecution halted after %d cycle%s.\1B[0m\0A\00", align 1
@cycles = common local_unnamed_addr global i32 0, align 4
@.str.5 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@.str.6 = private unnamed_addr constant [2 x i8] c"s\00", align 1
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
@alive = common local_unnamed_addr global i8 0, align 1

; Function Attrs: nounwind ssp uwtable
define i32 @main(i32, i8** nocapture readonly) local_unnamed_addr #0 {
  %3 = tail call zeroext i1 @wvm_init(i64 16777215) #7
  br i1 %3, label %7, label %4

; <label>:4:                                      ; preds = %2
  %5 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !2
  %6 = tail call i64 @fwrite(i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str, i64 0, i64 0), i64 26, i64 1, %struct.__sFILE* %5)
  tail call void @exit(i32 1) #8
  unreachable

; <label>:7:                                      ; preds = %2
  switch i32 %0, label %28 [
    i32 1, label %8
    i32 2, label %10
    i32 3, label %14
  ]

; <label>:8:                                      ; preds = %7
  %9 = tail call i32 @wvm_load(i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.1, i64 0, i64 0)) #7
  br label %28

; <label>:10:                                     ; preds = %7
  %11 = getelementptr inbounds i8*, i8** %1, i64 1
  %12 = load i8*, i8** %11, align 8, !tbaa !2
  %13 = tail call i32 @wvm_load(i8* %12) #7
  br label %28

; <label>:14:                                     ; preds = %7
  %15 = getelementptr inbounds i8*, i8** %1, i64 1
  %16 = load i8*, i8** %15, align 8, !tbaa !2
  %17 = tail call i32 @strcmp(i8* %16, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i64 0, i64 0))
  %18 = icmp eq i32 %17, 0
  br i1 %18, label %19, label %28

; <label>:19:                                     ; preds = %14
  %20 = getelementptr inbounds i8*, i8** %1, i64 2
  %21 = load i8*, i8** %20, align 8, !tbaa !2
  %22 = tail call i64 @strlen(i8* %21)
  %23 = add i64 %22, 25
  %24 = tail call i8* @calloc(i64 %23, i64 1) #9
  %25 = tail call i64 @llvm.objectsize.i64.p0i8(i8* %24, i1 false)
  %26 = tail call i32 (i8*, i32, i64, i8*, ...) @__sprintf_chk(i8* %24, i32 0, i64 %25, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.3, i64 0, i64 0), i8* %21) #7
  %27 = tail call i32 @wvm_load(i8* %24) #7
  br label %28

; <label>:28:                                     ; preds = %7, %10, %19, %14, %8
  tail call void (...) @wvm_init_vm() #7
  tail call void (...) @wvm_print_memory() #7
  br label %29

; <label>:29:                                     ; preds = %29, %28
  %30 = tail call zeroext i1 (...) @wvm_tick() #7
  br i1 %30, label %29, label %31

; <label>:31:                                     ; preds = %29
  %32 = load i32, i32* @cycles, align 4, !tbaa !6
  %33 = icmp eq i32 %32, 1
  %34 = select i1 %33, i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.str.5, i64 0, i64 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.6, i64 0, i64 0)
  %35 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([46 x i8], [46 x i8]* @.str.4, i64 0, i64 0), i32 %32, i8* %34)
  tail call void (...) @wvm_free() #7
  ret i32 0
}

declare zeroext i1 @wvm_init(i64) local_unnamed_addr #1

; Function Attrs: noreturn
declare void @exit(i32) local_unnamed_addr #2

declare i32 @wvm_load(i8*) local_unnamed_addr #1

; Function Attrs: nounwind readonly
declare i32 @strcmp(i8* nocapture, i8* nocapture) local_unnamed_addr #3

; Function Attrs: nounwind allocsize(0,1)
declare noalias i8* @calloc(i64, i64) local_unnamed_addr #4

; Function Attrs: nounwind readonly
declare i64 @strlen(i8* nocapture) local_unnamed_addr #3

declare i32 @__sprintf_chk(i8*, i32, i64, i8*, ...) local_unnamed_addr #1

; Function Attrs: nounwind readnone
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1) #5

declare void @wvm_init_vm(...) local_unnamed_addr #1

declare void @wvm_print_memory(...) local_unnamed_addr #1

declare zeroext i1 @wvm_tick(...) local_unnamed_addr #1

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #6

declare void @wvm_free(...) local_unnamed_addr #1

; Function Attrs: nounwind
declare i64 @fwrite(i8* nocapture, i64, i64, %struct.__sFILE* nocapture) #7

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind readonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind readnone }
attributes #6 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nounwind }
attributes #8 = { noreturn nounwind }
attributes #9 = { allocsize(0,1) }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"any pointer", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"int", !4, i64 0}
