; ModuleID = 'ops.c'
source_filename = "ops.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }

@registers = common global [128 x i64] zeroinitializer, align 16
@inttab = common global i64 0, align 8
@.str = private unnamed_addr constant [20 x i8] c"$%s: 0x%llx / %lld\0A\00", align 1
@alive = common global i8 0, align 1
@__stderrp = external global %struct.__sFILE*, align 8
@.str.1 = private unnamed_addr constant [26 x i8] c"op_eval not implemented.\0A\00", align 1
@.str.2 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@.str.3 = private unnamed_addr constant [5 x i8] c"%llx\00", align 1
@memory = common global i8* null, align 8
@memsize = common global i64 0, align 8
@pc = common global i64 0, align 8
@offset_symtab = common global i64 0, align 8
@offset_data = common global i64 0, align 8
@offset_code = common global i64 0, align 8
@offset_end = common global i64 0, align 8
@membytes = common global i64 0, align 8
@cycles = common global i32 0, align 4
@cur_ring = common global i32 0, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_nop(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

declare void @wvm_increment_pc(...) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_add(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = add nsw i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  %19 = load i8, i8* %5, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  %22 = load i64, i64* %21, align 8
  %23 = load i8, i8* %3, align 1
  %24 = zext i8 %23 to i64
  %25 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %24
  %26 = load i64, i64* %25, align 8
  %27 = icmp slt i64 %22, %26
  br i1 %27, label %28, label %32

; <label>:28:                                     ; preds = %1
  %29 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  %30 = and i64 %29, -9
  %31 = or i64 %30, 8
  store i64 %31, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 101), align 8
  br label %32

; <label>:32:                                     ; preds = %28, %1
  call void (...) @wvm_increment_pc()
  ret void
}

declare void @wvm_r_regs(i64, i8*, i8*, i8*) #1

declare void @wvm_alu_flags_update(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sub(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = sub nsw i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_mult(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_multu(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  call void @op_mult(i64 %3)
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_mod(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = srem i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sll(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = shl i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_srl(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = lshr i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sra(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = ashr i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_and(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = and i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_nand(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = and i64 %10, %14
  %16 = xor i64 %15, -1
  %17 = load i8, i8* %5, align 1
  %18 = zext i8 %17 to i64
  %19 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %18
  store i64 %16, i64* %19, align 8
  call void @wvm_alu_flags_update(i64 %16)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_nor(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = or i64 %10, %14
  %16 = xor i64 %15, -1
  %17 = load i8, i8* %5, align 1
  %18 = zext i8 %17 to i64
  %19 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %18
  store i64 %16, i64* %19, align 8
  call void @wvm_alu_flags_update(i64 %16)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_not(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = xor i64 %10, -1
  %12 = load i8, i8* %5, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  store i64 %11, i64* %14, align 8
  call void @wvm_alu_flags_update(i64 %11)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_or(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = or i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xnor(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = xor i64 %10, %14
  %16 = xor i64 %15, -1
  %17 = load i8, i8* %5, align 1
  %18 = zext i8 %17 to i64
  %19 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %18
  store i64 %16, i64* %19, align 8
  call void @wvm_alu_flags_update(i64 %16)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xor(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = xor i64 %10, %14
  %16 = load i8, i8* %5, align 1
  %17 = zext i8 %16 to i64
  %18 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %17
  store i64 %15, i64* %18, align 8
  call void @wvm_alu_flags_update(i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_land(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = icmp ne i64 %10, 0
  br i1 %11, label %12, label %18

; <label>:12:                                     ; preds = %1
  %13 = load i8, i8* %4, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  %16 = load i64, i64* %15, align 8
  %17 = icmp ne i64 %16, 0
  br label %18

; <label>:18:                                     ; preds = %12, %1
  %19 = phi i1 [ false, %1 ], [ %17, %12 ]
  %20 = zext i1 %19 to i32
  %21 = sext i32 %20 to i64
  %22 = load i8, i8* %5, align 1
  %23 = zext i8 %22 to i64
  %24 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %23
  store i64 %21, i64* %24, align 8
  call void @wvm_alu_flags_update(i64 %21)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lnand(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = icmp ne i64 %10, 0
  br i1 %11, label %12, label %18

; <label>:12:                                     ; preds = %1
  %13 = load i8, i8* %4, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  %16 = load i64, i64* %15, align 8
  %17 = icmp ne i64 %16, 0
  br label %18

; <label>:18:                                     ; preds = %12, %1
  %19 = phi i1 [ false, %1 ], [ %17, %12 ]
  %20 = xor i1 %19, true
  %21 = zext i1 %20 to i32
  %22 = sext i32 %21 to i64
  %23 = load i8, i8* %5, align 1
  %24 = zext i8 %23 to i64
  %25 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %24
  store i64 %22, i64* %25, align 8
  call void @wvm_alu_flags_update(i64 %22)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lnor(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = icmp ne i64 %10, 0
  br i1 %11, label %18, label %12

; <label>:12:                                     ; preds = %1
  %13 = load i8, i8* %4, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  %16 = load i64, i64* %15, align 8
  %17 = icmp ne i64 %16, 0
  br label %18

; <label>:18:                                     ; preds = %12, %1
  %19 = phi i1 [ true, %1 ], [ %17, %12 ]
  %20 = xor i1 %19, true
  %21 = zext i1 %20 to i32
  %22 = sext i32 %21 to i64
  %23 = load i8, i8* %5, align 1
  %24 = zext i8 %23 to i64
  %25 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %24
  store i64 %22, i64* %25, align 8
  call void @wvm_alu_flags_update(i64 %22)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lnot(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = icmp ne i64 %10, 0
  %12 = xor i1 %11, true
  %13 = zext i1 %12 to i32
  %14 = sext i32 %13 to i64
  %15 = load i8, i8* %5, align 1
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8
  call void @wvm_alu_flags_update(i64 %14)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lor(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = icmp ne i64 %10, 0
  br i1 %11, label %18, label %12

; <label>:12:                                     ; preds = %1
  %13 = load i8, i8* %4, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  %16 = load i64, i64* %15, align 8
  %17 = icmp ne i64 %16, 0
  br label %18

; <label>:18:                                     ; preds = %12, %1
  %19 = phi i1 [ true, %1 ], [ %17, %12 ]
  %20 = zext i1 %19 to i32
  %21 = sext i32 %20 to i64
  %22 = load i8, i8* %5, align 1
  %23 = zext i8 %22 to i64
  %24 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %23
  store i64 %21, i64* %24, align 8
  call void @wvm_alu_flags_update(i64 %21)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lxnor(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = xor i64 %10, %14
  %16 = icmp ne i64 %15, 0
  %17 = xor i1 %16, true
  %18 = zext i1 %17 to i32
  %19 = sext i32 %18 to i64
  %20 = load i8, i8* %5, align 1
  %21 = zext i8 %20 to i64
  %22 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %21
  store i64 %19, i64* %22, align 8
  call void @wvm_alu_flags_update(i64 %19)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lxor(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = xor i64 %10, %14
  %16 = icmp ne i64 %15, 0
  %17 = xor i1 %16, true
  %18 = xor i1 %17, true
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %5, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void @wvm_alu_flags_update(i64 %20)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_addi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = add nsw i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

declare zeroext i8 @wvm_i_rs(i64) #1

declare zeroext i8 @wvm_i_rd(i64) #1

declare i32 @wvm_i_imm(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_subi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = sub nsw i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_multi(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_multui(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_slli(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = shl i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_srli(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = lshr i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_srai(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = ashr i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_modi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = srem i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_andi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = and i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_nandi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = and i64 %15, %17
  %19 = xor i64 %18, -1
  %20 = load i8, i8* %4, align 1
  %21 = zext i8 %20 to i64
  %22 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %21
  store i64 %19, i64* %22, align 8
  call void @wvm_alu_flags_update(i64 %19)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_nori(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = or i64 %15, %17
  %19 = xor i64 %18, -1
  %20 = load i8, i8* %4, align 1
  %21 = zext i8 %20 to i64
  %22 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %21
  store i64 %19, i64* %22, align 8
  call void @wvm_alu_flags_update(i64 %19)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_ori(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = or i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xnori(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = xor i64 %15, %17
  %19 = xor i64 %18, -1
  %20 = load i8, i8* %4, align 1
  %21 = zext i8 %20 to i64
  %22 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %21
  store i64 %19, i64* %22, align 8
  call void @wvm_alu_flags_update(i64 %19)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_xori(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = xor i64 %15, %17
  %19 = load i8, i8* %4, align 1
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8
  call void @wvm_alu_flags_update(i64 %18)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lui(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rd(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %4, align 4
  %10 = zext i32 %9 to i64
  %11 = shl i64 %10, 32
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = and i64 %15, 268435455
  %17 = or i64 %11, %16
  %18 = load i8, i8* %3, align 1
  %19 = zext i8 %18 to i64
  %20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
  store i64 %17, i64* %20, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sl(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = icmp slt i64 %10, %14
  %16 = zext i1 %15 to i32
  %17 = sext i32 %16 to i64
  %18 = load i8, i8* %5, align 1
  %19 = zext i8 %18 to i64
  %20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
  store i64 %17, i64* %20, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sle(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = icmp sle i64 %10, %14
  %16 = zext i1 %15 to i32
  %17 = sext i32 %16 to i64
  %18 = load i8, i8* %5, align 1
  %19 = zext i8 %18 to i64
  %20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
  store i64 %17, i64* %20, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_seq(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = icmp eq i64 %10, %14
  %16 = zext i1 %15 to i32
  %17 = sext i32 %16 to i64
  %18 = load i8, i8* %5, align 1
  %19 = zext i8 %18 to i64
  %20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
  store i64 %17, i64* %20, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_slu(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = icmp ult i64 %10, %14
  %16 = zext i1 %15 to i32
  %17 = sext i32 %16 to i64
  %18 = load i8, i8* %5, align 1
  %19 = zext i8 %18 to i64
  %20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
  store i64 %17, i64* %20, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sleu(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %4, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = icmp ule i64 %10, %14
  %16 = zext i1 %15 to i32
  %17 = sext i32 %16 to i64
  %18 = load i8, i8* %5, align 1
  %19 = zext i8 %18 to i64
  %20 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %19
  store i64 %17, i64* %20, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sli(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = icmp slt i64 %15, %17
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %4, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_slei(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = icmp sle i64 %15, %17
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %4, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_seqi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = icmp eq i64 %15, %17
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %4, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_slui(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = icmp ult i64 %15, %17
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %4, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sleui(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = icmp ule i64 %15, %17
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %4, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sgi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = icmp sgt i64 %15, %17
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %4, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sgei(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_i_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call zeroext i8 @wvm_i_rd(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @wvm_i_imm(i64 %10)
  store i32 %11, i32* %5, align 4
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = load i32, i32* %5, align 4
  %17 = zext i32 %16 to i64
  %18 = icmp sge i64 %15, %17
  %19 = zext i1 %18 to i32
  %20 = sext i32 %19 to i64
  %21 = load i8, i8* %4, align 1
  %22 = zext i8 %21 to i64
  %23 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %22
  store i64 %20, i64* %23, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_j(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i32, align 4
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call i32 @wvm_j_addr(i64 %6)
  store i32 %7, i32* %3, align 4
  %8 = load i64, i64* %2, align 8
  %9 = call signext i8 @wvm_j_link(i64 %8)
  store i8 %9, i8* %4, align 1
  %10 = load i64, i64* %2, align 8
  %11 = call signext i8 @wvm_j_cond(i64 %10)
  store i8 %11, i8* %5, align 1
  %12 = load i8, i8* %5, align 1
  %13 = sext i8 %12 to i32
  %14 = call i32 @wvm_check_condition(i32 %13)
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %16, label %23

; <label>:16:                                     ; preds = %1
  %17 = load i8, i8* %4, align 1
  %18 = icmp ne i8 %17, 0
  br i1 %18, label %19, label %20

; <label>:19:                                     ; preds = %16
  call void (...) @wvm_link()
  br label %20

; <label>:20:                                     ; preds = %19, %16
  %21 = load i32, i32* %3, align 4
  %22 = zext i32 %21 to i64
  call void @wvm_jump(i64 %22)
  br label %24

; <label>:23:                                     ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %24

; <label>:24:                                     ; preds = %23, %20
  ret void
}

declare i32 @wvm_j_addr(i64) #1

declare signext i8 @wvm_j_link(i64) #1

declare signext i8 @wvm_j_cond(i64) #1

declare i32 @wvm_check_condition(i32) #1

declare void @wvm_link(...) #1

declare void @wvm_jump(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_jc(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  %7 = call zeroext i8 @wvm_j_rs(i64 %6)
  store i8 %7, i8* %3, align 1
  %8 = load i64, i64* %2, align 8
  %9 = call i32 @wvm_j_addr(i64 %8)
  store i32 %9, i32* %4, align 4
  %10 = load i64, i64* %2, align 8
  %11 = call signext i8 @wvm_j_link(i64 %10)
  store i8 %11, i8* %5, align 1
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = icmp ne i64 %15, 0
  br i1 %16, label %17, label %24

; <label>:17:                                     ; preds = %1
  %18 = load i8, i8* %5, align 1
  %19 = icmp ne i8 %18, 0
  br i1 %19, label %20, label %21

; <label>:20:                                     ; preds = %17
  call void (...) @wvm_link()
  br label %21

; <label>:21:                                     ; preds = %20, %17
  %22 = load i32, i32* %4, align 4
  %23 = zext i32 %22 to i64
  call void @wvm_jump(i64 %23)
  br label %25

; <label>:24:                                     ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %25

; <label>:25:                                     ; preds = %24, %21
  ret void
}

declare zeroext i8 @wvm_j_rs(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_jr(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  %6 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %7 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %7, i8* %3, i8* %4, i8* %5)
  %8 = load i64, i64* %2, align 8
  %9 = call signext i8 @wvm_r_cond(i64 %8)
  store i8 %9, i8* %6, align 1
  %10 = load i8, i8* %6, align 1
  %11 = sext i8 %10 to i32
  %12 = call i32 @wvm_check_condition(i32 %11)
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %14, label %19

; <label>:14:                                     ; preds = %1
  %15 = load i8, i8* %5, align 1
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  %18 = load i64, i64* %17, align 8
  call void @wvm_jump(i64 %18)
  br label %20

; <label>:19:                                     ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %20

; <label>:20:                                     ; preds = %19, %14
  ret void
}

declare signext i8 @wvm_r_cond(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_jrc(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = icmp ne i64 %10, 0
  br i1 %11, label %12, label %17

; <label>:12:                                     ; preds = %1
  %13 = load i8, i8* %5, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  %16 = load i64, i64* %15, align 8
  call void @wvm_jump(i64 %16)
  br label %18

; <label>:17:                                     ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %18

; <label>:18:                                     ; preds = %17, %12
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_jrl(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  %6 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %7 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %7, i8* %3, i8* %4, i8* %5)
  %8 = load i64, i64* %2, align 8
  %9 = call signext i8 @wvm_r_cond(i64 %8)
  store i8 %9, i8* %6, align 1
  %10 = load i8, i8* %6, align 1
  %11 = sext i8 %10 to i32
  %12 = call i32 @wvm_check_condition(i32 %11)
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %14, label %19

; <label>:14:                                     ; preds = %1
  call void (...) @wvm_link()
  %15 = load i8, i8* %5, align 1
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  %18 = load i64, i64* %17, align 8
  call void @wvm_jump(i64 %18)
  br label %20

; <label>:19:                                     ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %20

; <label>:20:                                     ; preds = %19, %14
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_jrlc(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = icmp ne i64 %10, 0
  br i1 %11, label %12, label %17

; <label>:12:                                     ; preds = %1
  call void (...) @wvm_link()
  %13 = load i8, i8* %5, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  %16 = load i64, i64* %15, align 8
  call void @wvm_jump(i64 %16)
  br label %18

; <label>:17:                                     ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %18

; <label>:18:                                     ; preds = %17, %12
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_c(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %5, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %3, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = call i64 @wvm_get_word(i64 %14)
  call void @wvm_set_word(i64 %10, i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

declare void @wvm_set_word(i64, i64) #1

declare i64 @wvm_get_word(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_l(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = call i64 @wvm_get_word(i64 %10)
  %12 = load i8, i8* %5, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  store i64 %11, i64* %14, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_s(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %5, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %3, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  call void @wvm_set_word(i64 %10, i64 %14)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_cb(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %5, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %3, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = call zeroext i8 @wvm_get_byte(i64 %14)
  call void @wvm_set_byte(i64 %10, i8 zeroext %15)
  call void (...) @wvm_increment_pc()
  ret void
}

declare void @wvm_set_byte(i64, i8 zeroext) #1

declare zeroext i8 @wvm_get_byte(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lb(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %3, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = call zeroext i8 @wvm_get_byte(i64 %10)
  %12 = zext i8 %11 to i64
  %13 = load i8, i8* %5, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  store i64 %12, i64* %15, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sb(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i8, i8* %5, align 1
  %8 = zext i8 %7 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  %10 = load i64, i64* %9, align 8
  %11 = load i8, i8* %3, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = trunc i64 %14 to i8
  call void @wvm_set_byte(i64 %10, i8 zeroext %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_spush(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16
  %8 = load i8, i8* %3, align 1
  %9 = zext i8 %8 to i64
  %10 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %9
  %11 = load i64, i64* %10, align 8
  call void @wvm_set_word(i64 %7, i64 %11)
  %12 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16
  %13 = sub nsw i64 %12, 8
  store i64 %13, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_spop(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  %5 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %6 = load i64, i64* %2, align 8
  call void @wvm_r_regs(i64 %6, i8* %3, i8* %4, i8* %5)
  %7 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16
  %8 = add nsw i64 %7, 8
  store i64 %8, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16
  %9 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16
  %10 = call i64 @wvm_get_word(i64 %9)
  %11 = load i8, i8* %5, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  store i64 %10, i64* %13, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_li(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rd(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %4, align 4
  %10 = zext i32 %9 to i64
  %11 = call i64 @wvm_get_word(i64 %10)
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  store i64 %11, i64* %14, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_si(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rs(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %4, align 4
  %10 = zext i32 %9 to i64
  %11 = load i8, i8* %3, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  call void @wvm_set_word(i64 %10, i64 %14)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_set(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rd(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %4, align 4
  %10 = zext i32 %9 to i64
  %11 = load i8, i8* %3, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  store i64 %10, i64* %13, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lbi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rd(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %4, align 4
  %10 = zext i32 %9 to i64
  %11 = call zeroext i8 @wvm_get_byte(i64 %10)
  %12 = zext i8 %11 to i64
  %13 = load i8, i8* %3, align 1
  %14 = zext i8 %13 to i64
  %15 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %14
  store i64 %12, i64* %15, align 8
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sbi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rs(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i32, i32* %4, align 4
  %10 = zext i32 %9 to i64
  %11 = load i8, i8* %3, align 1
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8
  %15 = trunc i64 %14 to i8
  call void @wvm_set_byte(i64 %10, i8 zeroext %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lni(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rd(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i8, i8* %3, align 1
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8
  %13 = load i32, i32* %4, align 4
  %14 = zext i32 %13 to i64
  %15 = call i64 @wvm_get_word(i64 %14)
  call void @wvm_set_word(i64 %12, i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_lbni(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call zeroext i8 @wvm_i_rd(i64 %5)
  store i8 %6, i8* %3, align 1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %4, align 4
  %9 = load i8, i8* %3, align 1
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8
  %13 = load i32, i32* %4, align 4
  %14 = zext i32 %13 to i64
  %15 = call zeroext i8 @wvm_get_byte(i64 %14)
  call void @wvm_set_byte(i64 %12, i8 zeroext %15)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_int(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call i32 @wvm_i_imm(i64 %4)
  store i32 %5, i32* %3, align 4
  %6 = load i32, i32* %3, align 4
  %7 = call i32 @wvm_interrupt(i32 %6)
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %10

; <label>:9:                                      ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %10

; <label>:10:                                     ; preds = %9, %1
  ret void
}

declare i32 @wvm_interrupt(i32) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_rit(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %4 = call i32 @check_ring(i32 0)
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %11

; <label>:6:                                      ; preds = %1
  %7 = load i64, i64* %2, align 8
  %8 = call i32 @wvm_i_imm(i64 %7)
  store i32 %8, i32* %3, align 4
  %9 = load i32, i32* %3, align 4
  %10 = zext i32 %9 to i64
  store i64 %10, i64* @inttab, align 8
  call void (...) @wvm_increment_pc()
  br label %11

; <label>:11:                                     ; preds = %6, %1
  ret void
}

declare i32 @check_ring(i32) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_time(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call zeroext i8 @wvm_r_rs(i64 %4)
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  %7 = zext i8 %6 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  %9 = load i64, i64* %8, align 8
  %10 = call i32 @wvm_set_timer(i64 %9)
  call void (...) @wvm_increment_pc()
  ret void
}

declare zeroext i8 @wvm_r_rs(i64) #1

declare i32 @wvm_set_timer(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_timei(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call i32 @wvm_i_imm(i64 %4)
  store i32 %5, i32* %3, align 4
  %6 = load i32, i32* %3, align 4
  %7 = zext i32 %6 to i64
  %8 = call i32 @wvm_set_timer(i64 %7)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_ring(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call zeroext i8 @wvm_r_rs(i64 %4)
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  %7 = zext i8 %6 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  %9 = load i64, i64* %8, align 8
  %10 = trunc i64 %9 to i32
  %11 = call i32 @wvm_change_ring(i32 %10)
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %13, label %14

; <label>:13:                                     ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %14

; <label>:14:                                     ; preds = %13, %1
  ret void
}

declare i32 @wvm_change_ring(i32) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_ringi(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i32, align 4
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call i32 @wvm_i_imm(i64 %4)
  store i32 %5, i32* %3, align 4
  %6 = load i32, i32* %3, align 4
  %7 = call i32 @wvm_change_ring(i32 %6)
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %10

; <label>:9:                                      ; preds = %1
  call void (...) @wvm_increment_pc()
  br label %10

; <label>:10:                                     ; preds = %9, %1
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_pr(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call zeroext i8 @wvm_r_rs(i64 %4)
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  %7 = call i8* @wvm_decode_reg(i8 zeroext %6)
  %8 = load i8, i8* %3, align 1
  %9 = zext i8 %8 to i64
  %10 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %9
  %11 = load i64, i64* %10, align 8
  %12 = load i8, i8* %3, align 1
  %13 = zext i8 %12 to i64
  %14 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %13
  %15 = load i64, i64* %14, align 8
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @.str, i32 0, i32 0), i8* %7, i64 %11, i64 %15)
  call void (...) @wvm_increment_pc()
  ret void
}

declare i32 @printf(i8*, ...) #1

declare i8* @wvm_decode_reg(i8 zeroext) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_halt(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  store i8 0, i8* @alive, align 1
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_eval(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %4 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %3, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.1, i32 0, i32 0))
  call void @exit(i32 1) #3
  unreachable
                                                  ; No predecessors!
  ret void
}

declare i32 @fprintf(%struct.__sFILE*, i8*, ...) #1

; Function Attrs: noreturn
declare void @exit(i32) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_prc(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call zeroext i8 @wvm_r_rs(i64 %4)
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  %7 = zext i8 %6 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  %9 = load i64, i64* %8, align 8
  %10 = trunc i64 %9 to i8
  %11 = sext i8 %10 to i32
  %12 = call i32 @putchar(i32 %11)
  call void (...) @wvm_increment_pc()
  ret void
}

declare i32 @putchar(i32) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_prd(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call zeroext i8 @wvm_r_rs(i64 %4)
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  %7 = zext i8 %6 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  %9 = load i64, i64* %8, align 8
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i32 0, i32 0), i64 %9)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_prx(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call zeroext i8 @wvm_r_rs(i64 %4)
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  %7 = zext i8 %6 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  %9 = load i64, i64* %8, align 8
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.3, i32 0, i32 0), i64 %9)
  call void (...) @wvm_increment_pc()
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @op_sleep(i64) #0 {
  %2 = alloca i64, align 8
  %3 = alloca i8, align 1
  store i64 %0, i64* %2, align 8
  %4 = load i64, i64* %2, align 8
  %5 = call zeroext i8 @wvm_r_rs(i64 %4)
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  %7 = zext i8 %6 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  %9 = load i64, i64* %8, align 8
  %10 = mul nsw i64 %9, 1000
  %11 = trunc i64 %10 to i32
  %12 = call i32 @"\01_usleep"(i32 %11)
  call void (...) @wvm_increment_pc()
  ret void
}

declare i32 @"\01_usleep"(i32) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
