source_filename = "llvm-link"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }

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
	%15 = getelementptr inbounds i8*, i8** %1, i64 1
	%17 = tail call %struct.__sFILE* @"\01_fopen"(i8* %16, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i64 0, i64 0)) #6
	%33 = call %struct.GraphObj* @newGraph(i32 %32)
	store %struct.GraphObj* %33, %struct.GraphObj** %6, align 8
	%48 = add nuw nsw i32 %.03544, 1
	%52 = sext i32 %51 to i64
	%53 = call i8* @calloc(i64 %52, i64 8)
	%54 = bitcast i8* %53 to %struct.ListObj**
}