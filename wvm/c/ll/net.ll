; ModuleID = 'ext/net.c'
source_filename = "ext/net.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }
%struct.addrinfo = type { i32, i32, i32, i32, i32, i8*, %struct.sockaddr*, %struct.addrinfo* }
%struct.sockaddr = type { i8, i8, [14 x i8] }
%struct.sockaddr_in = type { i8, i8, i16, %struct.in_addr, [8 x i8] }
%struct.in_addr = type { i32 }

@xn_socket = global i32 -1, align 4
@xn_connected = global i8 0, align 1
@__stderrp = external global %struct.__sFILE*, align 8
@.str = private unnamed_addr constant [45 x i8] c"\1B[31mERROR\1B[0m: Networking not initialized.\0A\00", align 1
@registers = common global [128 x i64] zeroinitializer, align 16
@.str.1 = private unnamed_addr constant [30 x i8] c"Connecting to \1B[1m%s:%d\1B[0m.\0A\00", align 1
@.str.2 = private unnamed_addr constant [12 x i8] c"getaddrinfo\00", align 1
@.str.3 = private unnamed_addr constant [45 x i8] c"\1B[31mERROR\1B[0m: Unable to parse address. %s\0A\00", align 1
@.str.4 = private unnamed_addr constant [44 x i8] c"\1B[31mERROR\1B[0m: Unable to resolve address.\0A\00", align 1
@.str.5 = private unnamed_addr constant [44 x i8] c"\1B[31mERROR\1B[0m: Unable to connect (%d). %s\0A\00", align 1
@.str.6 = private unnamed_addr constant [39 x i8] c"\1B[31mERROR\1B[0m: Socket not connected.\0A\00", align 1
@.str.7 = private unnamed_addr constant [47 x i8] c"\1B[31mERROR\1B[0m: Couldn't receive from socket.\0A\00", align 1
@memory = common global i8* null, align 8
@memsize = common global i64 0, align 8
@pc = common global i64 0, align 8
@offset_symtab = common global i64 0, align 8
@offset_data = common global i64 0, align 8
@offset_code = common global i64 0, align 8
@offset_end = common global i64 0, align 8
@membytes = common global i64 0, align 8
@cycles = common global i32 0, align 4
@alive = common global i8 0, align 1
@cur_ring = common global i32 0, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xn_init(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = call i32 @socket(i32 2, i32 1, i32 6)
  store i32 %3, i32* @xn_socket, align 4
  store i8 0, i8* @xn_connected, align 1
  call void (...) @wvm_increment_pc()
  ret void
}

declare i32 @socket(i32, i32, i32) #1

declare void @wvm_increment_pc(...) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xn_connect(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  %6 = alloca %struct.addrinfo*, align 8
  %7 = alloca %struct.sockaddr_in*, align 8
  %8 = alloca i8*, align 8
  %9 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %10 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %10, i8* %3, i8* %4, i8* %5)
  %11 = load i32, i32* @xn_socket, align 4
  %12 = icmp eq i32 %11, -1
  br i1 %12, label %13, label %16

; <label>:13:                                     ; preds = %1
  %14 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %15 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %14, i8* getelementptr inbounds ([45 x i8], [45 x i8]* @.str, i32 0, i32 0))
  call void @exit(i32 1) #4
  unreachable

; <label>:16:                                     ; preds = %1
  %17 = load i8, i8* %3, align 1
  %18 = zext i8 %17 to i64
  %19 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %18
  %20 = load i64, i64* %19, align 8
  %21 = call i8* @wvm_get_string(i64 %20)
  store i8* %21, i8** %8, align 8
  %22 = load i8*, i8** %8, align 8
  %23 = load i8, i8* %4, align 1
  %24 = zext i8 %23 to i64
  %25 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %24
  %26 = load i64, i64* %25, align 8
  %27 = trunc i64 %26 to i16
  %28 = zext i16 %27 to i32
  %29 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([30 x i8], [30 x i8]* @.str.1, i32 0, i32 0), i8* %22, i32 %28)
  %30 = load i8*, i8** %8, align 8
  %31 = call i32 @getaddrinfo(i8* %30, i8* null, %struct.addrinfo* null, %struct.addrinfo** %6)
  store i32 %31, i32* %9, align 4
  %32 = load i8*, i8** %8, align 8
  call void @free(i8* %32)
  %33 = load i32, i32* %9, align 4
  %34 = icmp eq i32 %33, 11
  br i1 %34, label %35, label %36

; <label>:35:                                     ; preds = %16
  call void @perror(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.2, i32 0, i32 0))
  call void @exit(i32 1) #4
  unreachable

; <label>:36:                                     ; preds = %16
  %37 = load i32, i32* %9, align 4
  %38 = icmp ne i32 %37, 0
  br i1 %38, label %39, label %44

; <label>:39:                                     ; preds = %36
  %40 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %41 = load i32, i32* %9, align 4
  %42 = call i8* @"\01_strerror"(i32 %41)
  %43 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %40, i8* getelementptr inbounds ([45 x i8], [45 x i8]* @.str.3, i32 0, i32 0), i8* %42)
  call void @exit(i32 1) #4
  unreachable

; <label>:44:                                     ; preds = %36
  br label %45

; <label>:45:                                     ; preds = %44
  %46 = load %struct.addrinfo*, %struct.addrinfo** %6, align 8
  %47 = icmp eq %struct.addrinfo* %46, null
  br i1 %47, label %48, label %51

; <label>:48:                                     ; preds = %45
  %49 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %50 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %49, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @.str.4, i32 0, i32 0))
  call void @exit(i32 1) #4
  unreachable

; <label>:51:                                     ; preds = %45
  %52 = load %struct.addrinfo*, %struct.addrinfo** %6, align 8
  %53 = getelementptr inbounds %struct.addrinfo, %struct.addrinfo* %52, i32 0, i32 6
  %54 = load %struct.sockaddr*, %struct.sockaddr** %53, align 8
  %55 = bitcast %struct.sockaddr* %54 to %struct.sockaddr_in*
  store %struct.sockaddr_in* %55, %struct.sockaddr_in** %7, align 8
  %56 = load i8, i8* %4, align 1
  %57 = zext i8 %56 to i64
  %58 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %57
  %59 = load i64, i64* %58, align 8
  %60 = trunc i64 %59 to i16
  %61 = call zeroext i16 @_OSSwapInt16(i16 zeroext %60)
  %62 = zext i16 %61 to i32
  %63 = trunc i32 %62 to i16
  %64 = load %struct.sockaddr_in*, %struct.sockaddr_in** %7, align 8
  %65 = getelementptr inbounds %struct.sockaddr_in, %struct.sockaddr_in* %64, i32 0, i32 2
  store i16 %63, i16* %65, align 2
  %66 = load i32, i32* @xn_socket, align 4
  %67 = load %struct.sockaddr_in*, %struct.sockaddr_in** %7, align 8
  %68 = bitcast %struct.sockaddr_in* %67 to %struct.sockaddr*
  %69 = call i32 @"\01_connect"(i32 %66, %struct.sockaddr* %68, i32 16)
  store i32 %69, i32* %9, align 4
  %70 = load i32, i32* %9, align 4
  %71 = icmp ne i32 %70, 0
  br i1 %71, label %72, label %80

; <label>:72:                                     ; preds = %51
  %73 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %74 = call i32* @__error()
  %75 = load i32, i32* %74, align 4
  %76 = call i32* @__error()
  %77 = load i32, i32* %76, align 4
  %78 = call i8* @"\01_strerror"(i32 %77)
  %79 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %73, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @.str.5, i32 0, i32 0), i32 %75, i8* %78)
  call void @exit(i32 1) #4
  unreachable

; <label>:80:                                     ; preds = %51
  %81 = load %struct.addrinfo*, %struct.addrinfo** %6, align 8
  %82 = bitcast %struct.addrinfo* %81 to i8*
  call void @free(i8* %82)
  %83 = load %struct.sockaddr_in*, %struct.sockaddr_in** %7, align 8
  %84 = bitcast %struct.sockaddr_in* %83 to i8*
  call void @free(i8* %84)
  store i8 1, i8* @xn_connected, align 1
  call void (...) @wvm_increment_pc()
  ret void
}

declare void @wvm_r_regs(i64, i8*, i8*, i8*) #1

declare i32 @fprintf(%struct.__sFILE*, i8*, ...) #1

; Function Attrs: noreturn
declare void @exit(i32) #2

declare i8* @wvm_get_string(i64) #1

declare i32 @printf(i8*, ...) #1

declare i32 @getaddrinfo(i8*, i8*, %struct.addrinfo*, %struct.addrinfo**) #1

declare void @free(i8*) #1

declare void @perror(i8*) #1

declare i8* @"\01_strerror"(i32) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal zeroext i16 @_OSSwapInt16(i16 zeroext) #0 {
  %2 = alloca i16, align 2
  store i16 %0, i16* %2, align 2
  %3 = load i16, i16* %2, align 2
  %4 = zext i16 %3 to i32
  %5 = shl i32 %4, 8
  %6 = load i16, i16* %2, align 2
  %7 = zext i16 %6 to i32
  %8 = ashr i32 %7, 8
  %9 = or i32 %5, %8
  %10 = trunc i32 %9 to i16
  ret i16 %10
}

declare i32 @"\01_connect"(i32, %struct.sockaddr*, i32) #1

declare i32* @__error() #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xn_send(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  %6 = alloca i8*, align 8
  %7 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %8 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %8, i8* %3, i8* %4, i8* %5)
  %9 = load i8, i8* @xn_connected, align 1
  %10 = trunc i8 %9 to i1
  br i1 %10, label %14, label %11

; <label>:11:                                     ; preds = %1
  %12 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %13 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %12, i8* getelementptr inbounds ([39 x i8], [39 x i8]* @.str.6, i32 0, i32 0))
  call void @exit(i32 1) #4
  unreachable

; <label>:14:                                     ; preds = %1
  %15 = load i8, i8* %3, align 1
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  %18 = load i64, i64* %17, align 8
  %19 = call i8* @wvm_get_string(i64 %18)
  store i8* %19, i8** %6, align 8
  %20 = load i8*, i8** %6, align 8
  %21 = call i64 @strlen(i8* %20)
  %22 = trunc i64 %21 to i32
  store i32 %22, i32* %7, align 4
  %23 = load i32, i32* @xn_socket, align 4
  %24 = load i8*, i8** %6, align 8
  %25 = load i32, i32* %7, align 4
  %26 = sext i32 %25 to i64
  %27 = call i64 @"\01_send"(i32 %23, i8* %24, i64 %26, i32 0)
  %28 = load i8*, i8** %6, align 8
  call void @free(i8* %28)
  call void (...) @wvm_increment_pc()
  ret void
}

declare i64 @strlen(i8*) #1

declare i64 @"\01_send"(i32, i8*, i64, i32) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xn_recv(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  %6 = alloca i32, align 4
  %7 = alloca i64, align 8
  %8 = alloca i8*, align 8
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %11 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %11, i8* %3, i8* %4, i8* %5)
  %12 = load i32, i32* @xn_socket, align 4
  %13 = icmp eq i32 %12, -1
  br i1 %13, label %14, label %17

; <label>:14:                                     ; preds = %1
  %15 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %16 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %15, i8* getelementptr inbounds ([45 x i8], [45 x i8]* @.str, i32 0, i32 0))
  call void @exit(i32 1) #4
  unreachable

; <label>:17:                                     ; preds = %1
  %18 = load i8, i8* %3, align 1
  %19 = zext i8 %18 to i64
  %20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
  %21 = load i64, i64* %20, align 8
  %22 = trunc i64 %21 to i32
  store i32 %22, i32* %6, align 4
  %23 = load i8, i8* %5, align 1
  %24 = zext i8 %23 to i64
  %25 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %24
  %26 = load i64, i64* %25, align 8
  store i64 %26, i64* %7, align 8
  %27 = load i32, i32* %6, align 4
  %28 = add nsw i32 %27, 1
  %29 = sext i32 %28 to i64
  %30 = call i8* @calloc(i64 %29, i64 1) #5
  store i8* %30, i8** %8, align 8
  %31 = load i32, i32* @xn_socket, align 4
  %32 = load i8*, i8** %8, align 8
  %33 = load i32, i32* %6, align 4
  %34 = sext i32 %33 to i64
  %35 = call i64 @"\01_recv"(i32 %31, i8* %32, i64 %34, i32 0)
  %36 = trunc i64 %35 to i32
  store i32 %36, i32* %9, align 4
  %37 = load i32, i32* %9, align 4
  %38 = icmp eq i32 %37, -1
  br i1 %38, label %39, label %42

; <label>:39:                                     ; preds = %17
  %40 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %41 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %40, i8* getelementptr inbounds ([47 x i8], [47 x i8]* @.str.7, i32 0, i32 0))
  call void @exit(i32 1) #4
  unreachable

; <label>:42:                                     ; preds = %17
  store i32 0, i32* %10, align 4
  br label %43

; <label>:43:                                     ; preds = %69, %42
  %44 = load i8*, i8** %8, align 8
  %45 = load i32, i32* %10, align 4
  %46 = sext i32 %45 to i64
  %47 = getelementptr inbounds i8, i8* %44, i64 %46
  %48 = load i8, i8* %47, align 1
  %49 = sext i8 %48 to i32
  %50 = icmp ne i32 %49, 0
  br i1 %50, label %51, label %55

; <label>:51:                                     ; preds = %43
  %52 = load i32, i32* %10, align 4
  %53 = load i32, i32* %6, align 4
  %54 = icmp slt i32 %52, %53
  br label %55

; <label>:55:                                     ; preds = %51, %43
  %56 = phi i1 [ false, %43 ], [ %54, %51 ]
  br i1 %56, label %57, label %72

; <label>:57:                                     ; preds = %55
  %58 = load i8*, i8** %8, align 8
  %59 = load i32, i32* %10, align 4
  %60 = sext i32 %59 to i64
  %61 = getelementptr inbounds i8, i8* %58, i64 %60
  %62 = load i8, i8* %61, align 1
  %63 = load i8*, i8** @memory, align 8
  %64 = load i64, i64* %7, align 8
  %65 = load i32, i32* %10, align 4
  %66 = sext i32 %65 to i64
  %67 = add nsw i64 %64, %66
  %68 = getelementptr inbounds i8, i8* %63, i64 %67
  store i8 %62, i8* %68, align 1
  br label %69

; <label>:69:                                     ; preds = %57
  %70 = load i32, i32* %10, align 4
  %71 = add nsw i32 %70, 1
  store i32 %71, i32* %10, align 4
  br label %43

; <label>:72:                                     ; preds = %55
  %73 = load i8*, i8** @memory, align 8
  %74 = load i64, i64* %7, align 8
  %75 = load i32, i32* %10, align 4
  %76 = sext i32 %75 to i64
  %77 = add nsw i64 %74, %76
  %78 = getelementptr inbounds i8, i8* %73, i64 %77
  store i8 0, i8* %78, align 1
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: allocsize(0,1)
declare i8* @calloc(i64, i64) #3

declare i64 @"\01_recv"(i32, i8*, i64, i32) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noreturn }
attributes #5 = { allocsize(0,1) }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
