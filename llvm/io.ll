; ModuleID = 'test/io.c'
source_filename = "test/io.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@i = local_unnamed_addr global i32 5, align 4

; Function Attrs: nounwind ssp uwtable
define void @_main() local_unnamed_addr #0 {
  br label %1

; <label>:1:                                      ; preds = %1, %0
  %2 = phi i32 [ 0, %0 ], [ %3, %1 ]
  tail call void @_prc(i64 104) #2
  tail call void @_prc(i64 101) #2
  tail call void @_prc(i64 108) #2
  tail call void @_prc(i64 108) #2
  tail call void @_prc(i64 111) #2
  tail call void @_prc(i64 10) #2
  %3 = add nuw nsw i32 %2, 1
  %4 = load i32, i32* @i, align 4, !tbaa !3
  %5 = icmp slt i32 %3, %4
  br i1 %5, label %1, label %6

; <label>:6:                                      ; preds = %1
  tail call void @_prc(i64 98) #2
  tail call void @_prc(i64 121) #2
  tail call void @_prc(i64 101) #2
  tail call void @_prc(i64 10) #2
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @strprint(i8* nocapture readonly) local_unnamed_addr #0 {
  %2 = load i8, i8* %0, align 1, !tbaa !7
  %3 = icmp eq i8 %2, 0
  br i1 %3, label %5, label %4

; <label>:4:                                      ; preds = %1
  br label %6

; <label>:5:                                      ; preds = %6, %1
  ret void

; <label>:6:                                      ; preds = %4, %6
  %7 = phi i64 [ %10, %6 ], [ 0, %4 ]
  %8 = phi i8 [ %12, %6 ], [ %2, %4 ]
  %9 = sext i8 %8 to i64
  tail call void @_prc(i64 %9) #2
  %10 = add nuw i64 %7, 1
  %11 = getelementptr inbounds i8, i8* %0, i64 %10
  %12 = load i8, i8* %11, align 1, !tbaa !7
  %13 = icmp eq i8 %12, 0
  br i1 %13, label %5, label %6
}

declare void @_prc(i64) local_unnamed_addr #1

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
!7 = !{!5, !5, i64 0}
