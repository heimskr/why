; ModuleID = 'ext/net.c'
source_filename = "ext/net.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }
%struct.addrinfo = type { i32, i32, i32, i32, i32, i8*, %struct.sockaddr*, %struct.addrinfo* }
%struct.sockaddr = type { i8, i8, [14 x i8] }

@xn_socket = local_unnamed_addr global i32 -1, align 4
@xn_connected = local_unnamed_addr global i8 0, align 1
@__stderrp = external local_unnamed_addr global %struct.__sFILE*, align 8
@.str = private unnamed_addr constant [45 x i8] c"\1B[31mERROR\1B[0m: Networking not initialized.\0A\00", align 1
@registers = common local_unnamed_addr global [128 x i64] zeroinitializer, align 16
@.str.1 = private unnamed_addr constant [30 x i8] c"Connecting to \1B[1m%s:%d\1B[0m.\0A\00", align 1
@.str.2 = private unnamed_addr constant [12 x i8] c"getaddrinfo\00", align 1
@.str.3 = private unnamed_addr constant [45 x i8] c"\1B[31mERROR\1B[0m: Unable to parse address. %s\0A\00", align 1
@.str.4 = private unnamed_addr constant [44 x i8] c"\1B[31mERROR\1B[0m: Unable to resolve address.\0A\00", align 1
@.str.5 = private unnamed_addr constant [44 x i8] c"\1B[31mERROR\1B[0m: Unable to connect (%d). %s\0A\00", align 1
@.str.6 = private unnamed_addr constant [39 x i8] c"\1B[31mERROR\1B[0m: Socket not connected.\0A\00", align 1
@.str.7 = private unnamed_addr constant [47 x i8] c"\1B[31mERROR\1B[0m: Couldn't receive from socket.\0A\00", align 1
@memory = common local_unnamed_addr global i8* null, align 8
@memsize = common local_unnamed_addr global i64 0, align 8
@pc = common local_unnamed_addr global i64 0, align 8
@offset_symtab = common local_unnamed_addr global i64 0, align 8
@offset_handlers = common local_unnamed_addr global i64 0, align 8
@offset_data = common local_unnamed_addr global i64 0, align 8
@offset_code = common local_unnamed_addr global i64 0, align 8
@offset_end = common local_unnamed_addr global i64 0, align 8
@membytes = common local_unnamed_addr global i64 0, align 8
@cycles = common local_unnamed_addr global i32 0, align 4
@alive = common local_unnamed_addr global i8 0, align 1

; Function Attrs: nounwind ssp uwtable
define void @op_xn_init(i64) local_unnamed_addr #0 {
	%2 = tail call i32 @socket(i32 2, i32 1, i32 6) #7
	store i32 %2, i32* @xn_socket, align 4, !tbaa !2
	store i8 0, i8* @xn_connected, align 1, !tbaa !6
	tail call void (...) @wvm_increment_pc() #7
	ret void
}

declare i32 @socket(i32, i32, i32) local_unnamed_addr #1

declare void @wvm_increment_pc(...) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_xn_connect(i64) local_unnamed_addr #0 {
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	%4 = alloca i8, align 1
	%5 = alloca %struct.addrinfo*, align 8
	call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
	call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
	call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
	call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
	%6 = bitcast %struct.addrinfo** %5 to i8*
	call void @llvm.lifetime.start(i64 8, i8* nonnull %6) #7
	%7 = load i32, i32* @xn_socket, align 4, !tbaa !2
	%8 = icmp eq i32 %7, -1
	br i1 %8, label %9, label %12

; <label>:9:                                      ; preds = %1
	%10 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !8
	%11 = call i64 @fwrite(i8* getelementptr inbounds ([45 x i8], [45 x i8]* @.str, i64 0, i64 0), i64 44, i64 1, %struct.__sFILE* %10)
	call void @exit(i32 1) #9
	unreachable

; <label>:12:                                     ; preds = %1
	%13 = load i8, i8* %2, align 1, !tbaa !10
	%14 = zext i8 %13 to i64
	%15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
	%16 = load i64, i64* %15, align 8, !tbaa !11
	%17 = call i8* @wvm_get_string(i64 %16) #7
	%18 = load i8, i8* %3, align 1, !tbaa !10
	%19 = zext i8 %18 to i64
	%20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
	%21 = load i64, i64* %20, align 8, !tbaa !11
	%22 = trunc i64 %21 to i32
	%23 = and i32 %22, 65535
	%24 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([30 x i8], [30 x i8]* @.str.1, i64 0, i64 0), i8* %17, i32 %23)
	%25 = call i32 @getaddrinfo(i8* %17, i8* null, %struct.addrinfo* null, %struct.addrinfo** nonnull %5) #7
	call void @free(i8* %17)
	switch i32 %25, label %27 [
	i32 11, label %26
	i32 0, label %31
]

; <label>:26:                                     ; preds = %12
	call void @perror(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.2, i64 0, i64 0)) #10
	call void @exit(i32 1) #9
	unreachable

; <label>:27:                                     ; preds = %12
	%28 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !8
	%29 = call i8* @"\01_strerror"(i32 %25) #7
	%30 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %28, i8* getelementptr inbounds ([45 x i8], [45 x i8]* @.str.3, i64 0, i64 0), i8* %29)
	call void @exit(i32 1) #9
	unreachable

; <label>:31:                                     ; preds = %12
	%32 = load %struct.addrinfo*, %struct.addrinfo** %5, align 8, !tbaa !8
	%33 = icmp eq %struct.addrinfo* %32, null
	br i1 %33, label %34, label %37

; <label>:34:                                     ; preds = %31
	%35 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !8
	%36 = call i64 @fwrite(i8* getelementptr inbounds ([44 x i8], [44 x i8]* @.str.4, i64 0, i64 0), i64 43, i64 1, %struct.__sFILE* %35)
	call void @exit(i32 1) #9
	unreachable

; <label>:37:                                     ; preds = %31
	%38 = getelementptr inbounds %struct.addrinfo, %struct.addrinfo* %32, i64 0, i32 6
	%39 = load %struct.sockaddr*, %struct.sockaddr** %38, align 8, !tbaa !13
	%40 = load i8, i8* %3, align 1, !tbaa !10
	%41 = zext i8 %40 to i64
	%42 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %41
	%43 = load i64, i64* %42, align 8, !tbaa !11
	%44 = trunc i64 %43 to i16
	%45 = call i16 @llvm.bswap.i16(i16 %44) #7
	%46 = getelementptr inbounds %struct.sockaddr, %struct.sockaddr* %39, i64 0, i32 2
	%47 = bitcast [14 x i8]* %46 to i16*
	store i16 %45, i16* %47, align 2, !tbaa !15
	%48 = load i32, i32* @xn_socket, align 4, !tbaa !2
	%49 = call i32 @"\01_connect"(i32 %48, %struct.sockaddr* %39, i32 16) #7
	%50 = icmp eq i32 %49, 0
	br i1 %50, label %59, label %51

; <label>:51:                                     ; preds = %37
	%52 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !8
	%53 = call i32* @__error() #7
	%54 = load i32, i32* %53, align 4, !tbaa !2
	%55 = call i32* @__error() #7
	%56 = load i32, i32* %55, align 4, !tbaa !2
	%57 = call i8* @"\01_strerror"(i32 %56) #7
	%58 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %52, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @.str.5, i64 0, i64 0), i32 %54, i8* %57)
	call void @exit(i32 1) #9
	unreachable

; <label>:59:                                     ; preds = %37
	%60 = bitcast %struct.addrinfo** %5 to i8**
	%61 = load i8*, i8** %60, align 8, !tbaa !8
	call void @free(i8* %61)
	%62 = getelementptr inbounds %struct.sockaddr, %struct.sockaddr* %39, i64 0, i32 0
	call void @free(i8* %62)
	store i8 1, i8* @xn_connected, align 1, !tbaa !6
	call void (...) @wvm_increment_pc() #7
	call void @llvm.lifetime.end(i64 8, i8* nonnull %6) #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
	ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #2

declare void @wvm_r_regs(i64, i8*, i8*, i8*) local_unnamed_addr #1

; Function Attrs: nounwind
declare i32 @fprintf(%struct.__sFILE* nocapture, i8* nocapture readonly, ...) local_unnamed_addr #3

; Function Attrs: noreturn
declare void @exit(i32) local_unnamed_addr #4

declare i8* @wvm_get_string(i64) local_unnamed_addr #1

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #3

declare i32 @getaddrinfo(i8*, i8*, %struct.addrinfo*, %struct.addrinfo**) local_unnamed_addr #1

; Function Attrs: nounwind
declare void @free(i8* nocapture) local_unnamed_addr #3

; Function Attrs: nounwind
declare void @perror(i8* nocapture readonly) local_unnamed_addr #3

declare i8* @"\01_strerror"(i32) local_unnamed_addr #1

declare i32 @"\01_connect"(i32, %struct.sockaddr*, i32) local_unnamed_addr #1

declare i32* @__error() local_unnamed_addr #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #2

; Function Attrs: nounwind ssp uwtable
define void @op_xn_send(i64) local_unnamed_addr #0 {
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	%4 = alloca i8, align 1
	call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
	call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
	call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
	call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
	%5 = load i8, i8* @xn_connected, align 1, !tbaa !6, !range !19
	%6 = icmp eq i8 %5, 0
	br i1 %6, label %7, label %10

; <label>:7:                                      ; preds = %1
	%8 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !8
	%9 = call i64 @fwrite(i8* getelementptr inbounds ([39 x i8], [39 x i8]* @.str.6, i64 0, i64 0), i64 38, i64 1, %struct.__sFILE* %8)
	call void @exit(i32 1) #9
	unreachable

; <label>:10:                                     ; preds = %1
	%11 = load i8, i8* %2, align 1, !tbaa !10
	%12 = zext i8 %11 to i64
	%13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
	%14 = load i64, i64* %13, align 8, !tbaa !11
	%15 = call i8* @wvm_get_string(i64 %14) #7
	%16 = call i64 @strlen(i8* %15)
	%17 = load i32, i32* @xn_socket, align 4, !tbaa !2
	%18 = shl i64 %16, 32
	%19 = ashr exact i64 %18, 32
	%20 = call i64 @"\01_send"(i32 %17, i8* %15, i64 %19, i32 0) #7
	call void @free(i8* %15)
	call void (...) @wvm_increment_pc() #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
	ret void
}

; Function Attrs: nounwind readonly
declare i64 @strlen(i8* nocapture) local_unnamed_addr #5

declare i64 @"\01_send"(i32, i8*, i64, i32) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_xn_recv(i64) local_unnamed_addr #0 {
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	%4 = alloca i8, align 1
	call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
	call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
	call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
	call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
	%5 = load i32, i32* @xn_socket, align 4, !tbaa !2
	%6 = icmp eq i32 %5, -1
	br i1 %6, label %7, label %10

; <label>:7:                                      ; preds = %1
	%8 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !8
	%9 = call i64 @fwrite(i8* getelementptr inbounds ([45 x i8], [45 x i8]* @.str, i64 0, i64 0), i64 44, i64 1, %struct.__sFILE* %8)
	call void @exit(i32 1) #9
	unreachable

; <label>:10:                                     ; preds = %1
	%11 = load i8, i8* %2, align 1, !tbaa !10
	%12 = zext i8 %11 to i64
	%13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
	%14 = load i64, i64* %13, align 8, !tbaa !11
	%15 = load i8, i8* %4, align 1, !tbaa !10
	%16 = zext i8 %15 to i64
	%17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
	%18 = load i64, i64* %17, align 8, !tbaa !11
	%19 = shl i64 %14, 32
	%20 = add i64 %19, 4294967296
	%21 = ashr exact i64 %20, 32
	%22 = call i8* @calloc(i64 %21, i64 1) #11
	%23 = ashr exact i64 %19, 32
	%24 = call i64 @"\01_recv"(i32 %5, i8* %22, i64 %23, i32 0) #7
	%25 = trunc i64 %24 to i32
	%26 = icmp eq i32 %25, -1
	br i1 %26, label %38, label %27

; <label>:27:                                     ; preds = %10
	%28 = trunc i64 %14 to i32
	%29 = load i8, i8* %22, align 1, !tbaa !10
	%30 = icmp ne i8 %29, 0
	%31 = icmp sgt i32 %28, 0
	%32 = and i1 %31, %30
	%33 = load i8*, i8** @memory, align 8, !tbaa !8
	%34 = getelementptr inbounds i8, i8* %33, i64 %18
	br i1 %32, label %35, label %55

; <label>:35:                                     ; preds = %27
	%36 = shl i64 %14, 32
	%37 = ashr exact i64 %36, 32
	br label %41

; <label>:38:                                     ; preds = %10
	%39 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !8
	%40 = call i64 @fwrite(i8* getelementptr inbounds ([47 x i8], [47 x i8]* @.str.7, i64 0, i64 0), i64 46, i64 1, %struct.__sFILE* %39)
	call void @exit(i32 1) #9
	unreachable

; <label>:41:                                     ; preds = %35, %41
	%42 = phi i64 [ 0, %35 ], [ %45, %41 ]
	%43 = phi i8* [ %34, %35 ], [ %53, %41 ]
	%44 = phi i8 [ %29, %35 ], [ %47, %41 ]
	store i8 %44, i8* %43, align 1, !tbaa !10
	%45 = add nuw nsw i64 %42, 1
	%46 = getelementptr inbounds i8, i8* %22, i64 %45
	%47 = load i8, i8* %46, align 1, !tbaa !10
	%48 = icmp ne i8 %47, 0
	%49 = icmp slt i64 %45, %37
	%50 = and i1 %49, %48
	%51 = load i8*, i8** @memory, align 8, !tbaa !8
	%52 = add nsw i64 %45, %18
	%53 = getelementptr inbounds i8, i8* %51, i64 %52
	br i1 %50, label %41, label %54

; <label>:54:                                     ; preds = %41
	br label %55

; <label>:55:                                     ; preds = %54, %27
	%56 = phi i8* [ %34, %27 ], [ %53, %54 ]
	store i8 0, i8* %56, align 1, !tbaa !10
	call void (...) @wvm_increment_pc() #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
	call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
	ret void
}

; Function Attrs: nounwind allocsize(0,1)
declare noalias i8* @calloc(i64, i64) local_unnamed_addr #6

declare i64 @"\01_recv"(i32, i8*, i64, i32) local_unnamed_addr #1

; Function Attrs: nounwind
declare i64 @fwrite(i8* nocapture, i64, i64, %struct.__sFILE* nocapture) #7

; Function Attrs: nounwind readnone
declare i16 @llvm.bswap.i16(i16) #8

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind readonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nounwind allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nounwind }
attributes #8 = { nounwind readnone }
attributes #9 = { noreturn nounwind }
attributes #10 = { cold }
attributes #11 = { allocsize(0,1) }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"int", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"_Bool", !4, i64 0}
!8 = !{!9, !9, i64 0}
!9 = !{!"any pointer", !4, i64 0}
!10 = !{!4, !4, i64 0}
!11 = !{!12, !12, i64 0}
!12 = !{!"long long", !4, i64 0}
!13 = !{!14, !9, i64 32}
!14 = !{!"addrinfo", !3, i64 0, !3, i64 4, !3, i64 8, !3, i64 12, !3, i64 16, !9, i64 24, !9, i64 32, !9, i64 40}
!15 = !{!16, !17, i64 2}
!16 = !{!"sockaddr_in", !4, i64 0, !4, i64 1, !17, i64 2, !18, i64 4, !4, i64 8}
!17 = !{!"short", !4, i64 0}
!18 = !{!"in_addr", !3, i64 0}
!19 = !{i8 0, i8 2}
