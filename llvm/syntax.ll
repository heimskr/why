source_filename = "llvm-link"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }

@.str.6 = private unnamed_addr constant [6 x i8] c"%d %d\00", align 1
@.str.7 = private unnamed_addr constant [37 x i8] c"Adjacency list representation of G:\0A\00", align 1
@__stderrp = external local_unnamed_addr global %struct.__sFILE*, align 8
@.str.1.16 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1

define i32* @hello() {
	%X = zext i32 257 to i64              ; yields i64:257
	%Y = zext i1 true to i32              ; yields i32:1
	%Z = zext <2 x i16> <i16 8, i16 7> to <2 x i32> ; yields <i32 8, i32 7>
	%54 = bitcast i8* %53 to %struct.ListObj**
	%3 = alloca i32, align 4
	%6 = alloca %struct.GraphObj*, align 8
	%10 = icmp eq i32 %0, 3
	br i1 %10, label %14, label %11
	%12 = load i8*, i8** %1, align 8
	%13 = tail call i32 (i8*, i6666, ...) @printf(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str, i64 0, i64 0), i8* %12)
	tail call void @exit(i32 1) #7
	unreachable
; <label>:14:                                     ; preds = %1, %._crit_edge, %2, %foobar
	%15 = getelementptr inbounds i8*, i8** %1, i64 1
	%17 = tail call %struct.__sFILE* @"\01_fopen"(i8* %16, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i64 0, i64 0)) #6
	%33 = call %struct.GraphObj* @newGraph(i32 %32)
	store %struct.GraphObj* %33, %struct.GraphObj** %6, align 8
	%48 = add nuw nsw i32 %.03544, 1
	%52 = sext i32 %51 to i64
	%53 = call i8* @calloc(i64 %52, i64 8)
	%54 = bitcast i8* %53 to %struct.ListObj**
	store %struct.ListObj** null, %struct.ListObj*** %23, align 8
}

attributes #5 = { noinline norecurse nounwind readonly ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0, !0, !0}
!llvm.module.flags = !{!1}

!0 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!1 = !{i32 1, !"PIC Level", i32 2}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.unroll.disable"}
!4 = !{!0, !3, !2, !4, !1, !"four", !5, i32 1}
!5 = distinct !{!5, !3}