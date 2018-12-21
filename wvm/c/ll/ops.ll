; ModuleID = 'ops.c'
source_filename = "ops.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }

@registers = common local_unnamed_addr global [128 x i64] zeroinitializer, align 16
@.str = private unnamed_addr constant [20 x i8] c"$%s: 0x%llx / %lld\0A\00", align 1
@alive = common local_unnamed_addr global i8 0, align 1
@__stderrp = external local_unnamed_addr global %struct.__sFILE*, align 8
@.str.1 = private unnamed_addr constant [26 x i8] c"op_eval not implemented.\0A\00", align 1
@.str.2 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@.str.3 = private unnamed_addr constant [5 x i8] c"%llx\00", align 1
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

; Function Attrs: nounwind ssp uwtable
define void @op_nop(i64) local_unnamed_addr #0 {
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

declare void @wvm_increment_pc(...) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_add(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = add nsw i64 %12, %8
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #2

declare void @wvm_r_regs(i64, i8*, i8*, i8*) local_unnamed_addr #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #2

; Function Attrs: nounwind ssp uwtable
define void @op_sub(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = sub nsw i64 %8, %12
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_mult(i64) local_unnamed_addr #0 {
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_addu(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = add nsw i64 %12, %8
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_subu(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = sub nsw i64 %8, %12
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_multu(i64) local_unnamed_addr #0 {
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_mod(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = srem i64 %8, %12
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sll(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = shl i64 %8, %12
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_srl(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = lshr i64 %8, %12
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sra(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = ashr i64 %8, %12
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_and(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = and i64 %12, %8
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_nand(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = and i64 %12, %8
  %14 = xor i64 %13, -1
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_nor(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = or i64 %12, %8
  %14 = xor i64 %13, -1
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_not(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_r_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_r_rd(i64 %0) #7
  %4 = zext i8 %2 to i64
  %5 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %4
  %6 = load i64, i64* %5, align 8, !tbaa !5
  %7 = xor i64 %6, -1
  %8 = zext i8 %3 to i64
  %9 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %8
  store i64 %7, i64* %9, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

declare zeroext i8 @wvm_r_rs(i64) local_unnamed_addr #1

declare zeroext i8 @wvm_r_rd(i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_or(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = or i64 %12, %8
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_xnor(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = xor i64 %8, -1
  %14 = xor i64 %12, %13
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_xor(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = xor i64 %12, %8
  %14 = load i8, i8* %4, align 1, !tbaa !2
  %15 = zext i8 %14 to i64
  %16 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %15
  store i64 %13, i64* %16, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_land(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %16, label %10

; <label>:10:                                     ; preds = %1
  %11 = load i8, i8* %3, align 1, !tbaa !2
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8, !tbaa !5
  %15 = icmp ne i64 %14, 0
  br label %16

; <label>:16:                                     ; preds = %1, %10
  %17 = phi i1 [ false, %1 ], [ %15, %10 ]
  %18 = zext i1 %17 to i64
  %19 = load i8, i8* %4, align 1, !tbaa !2
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lnand(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %16, label %10

; <label>:10:                                     ; preds = %1
  %11 = load i8, i8* %3, align 1, !tbaa !2
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8, !tbaa !5
  %15 = icmp eq i64 %14, 0
  br label %16

; <label>:16:                                     ; preds = %1, %10
  %17 = phi i1 [ true, %1 ], [ %15, %10 ]
  %18 = zext i1 %17 to i64
  %19 = load i8, i8* %4, align 1, !tbaa !2
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lnor(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %10, label %16

; <label>:10:                                     ; preds = %1
  %11 = load i8, i8* %3, align 1, !tbaa !2
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8, !tbaa !5
  %15 = icmp eq i64 %14, 0
  br label %16

; <label>:16:                                     ; preds = %1, %10
  %17 = phi i1 [ false, %1 ], [ %15, %10 ]
  %18 = zext i1 %17 to i64
  %19 = load i8, i8* %4, align 1, !tbaa !2
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lnot(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_r_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_r_rd(i64 %0) #7
  %4 = zext i8 %2 to i64
  %5 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %4
  %6 = load i64, i64* %5, align 8, !tbaa !5
  %7 = icmp eq i64 %6, 0
  %8 = zext i1 %7 to i64
  %9 = zext i8 %3 to i64
  %10 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %9
  store i64 %8, i64* %10, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lor(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %10, label %16

; <label>:10:                                     ; preds = %1
  %11 = load i8, i8* %3, align 1, !tbaa !2
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8, !tbaa !5
  %15 = icmp ne i64 %14, 0
  br label %16

; <label>:16:                                     ; preds = %1, %10
  %17 = phi i1 [ true, %1 ], [ %15, %10 ]
  %18 = zext i1 %17 to i64
  %19 = load i8, i8* %4, align 1, !tbaa !2
  %20 = zext i8 %19 to i64
  %21 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %20
  store i64 %18, i64* %21, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lxnor(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = icmp eq i64 %8, %12
  %14 = zext i1 %13 to i64
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lxor(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = icmp ne i64 %8, %12
  %14 = zext i1 %13 to i64
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_addi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = add nsw i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

declare zeroext i8 @wvm_i_rs(i64) local_unnamed_addr #1

declare zeroext i8 @wvm_i_rd(i64) local_unnamed_addr #1

declare i32 @wvm_i_imm(i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_subi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = sub nsw i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_multi(i64) local_unnamed_addr #0 {
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_addui(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = add i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_subui(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = sub i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_multui(i64) local_unnamed_addr #0 {
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_slli(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = shl i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_srli(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = lshr i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_srai(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = ashr i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_modi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = srem i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_andi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = and i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_nandi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = and i64 %7, %8
  %10 = xor i64 %9, -1
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_nori(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = or i64 %7, %8
  %10 = xor i64 %9, -1
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_ori(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = or i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_xnori(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = xor i64 %8, -1
  %10 = xor i64 %7, %9
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_xori(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = xor i64 %7, %8
  %10 = zext i8 %3 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  store i64 %9, i64* %11, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lui(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i32 %3 to i64
  %5 = shl nuw i64 %4, 32
  %6 = zext i8 %2 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  store i64 %5, i64* %7, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sl(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = icmp slt i64 %8, %12
  %14 = zext i1 %13 to i64
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sle(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = icmp sle i64 %8, %12
  %14 = zext i1 %13 to i64
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_seq(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = icmp eq i64 %8, %12
  %14 = zext i1 %13 to i64
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_slu(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = icmp ult i64 %8, %12
  %14 = zext i1 %13 to i64
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sleu(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %3, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = icmp ule i64 %8, %12
  %14 = zext i1 %13 to i64
  %15 = load i8, i8* %4, align 1, !tbaa !2
  %16 = zext i8 %15 to i64
  %17 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %16
  store i64 %14, i64* %17, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sli(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = icmp slt i64 %7, %8
  %10 = zext i1 %9 to i64
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_slei(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = icmp sle i64 %7, %8
  %10 = zext i1 %9 to i64
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_seqi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = icmp eq i64 %7, %8
  %10 = zext i1 %9 to i64
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_slui(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = icmp ult i64 %7, %8
  %10 = zext i1 %9 to i64
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sleui(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = icmp ule i64 %7, %8
  %10 = zext i1 %9 to i64
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sgi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = icmp sgt i64 %7, %8
  %10 = zext i1 %9 to i64
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sgei(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %4 = tail call i32 @wvm_i_imm(i64 %0) #7
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = zext i32 %4 to i64
  %9 = icmp sge i64 %7, %8
  %10 = zext i1 %9 to i64
  %11 = zext i8 %3 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %10, i64* %12, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_j(i64) local_unnamed_addr #0 {
  %2 = tail call i32 @wvm_j_addr(i64 %0) #7
  %3 = zext i32 %2 to i64
  tail call void @wvm_jump(i64 %3) #7
  ret void
}

declare i32 @wvm_j_addr(i64) local_unnamed_addr #1

declare void @wvm_jump(i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_jc(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_j_rs(i64 %0) #7
  %3 = tail call i32 @wvm_j_addr(i64 %0) #7
  %4 = zext i8 %2 to i64
  %5 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %4
  %6 = load i64, i64* %5, align 8, !tbaa !5
  %7 = icmp eq i64 %6, 0
  br i1 %7, label %10, label %8

; <label>:8:                                      ; preds = %1
  %9 = zext i32 %3 to i64
  tail call void @wvm_jump(i64 %9) #7
  br label %11

; <label>:10:                                     ; preds = %1
  tail call void (...) @wvm_increment_pc() #7
  br label %11

; <label>:11:                                     ; preds = %10, %8
  ret void
}

declare zeroext i8 @wvm_j_rs(i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_jl(i64) local_unnamed_addr #0 {
  %2 = tail call i32 @wvm_j_addr(i64 %0) #7
  tail call void (...) @wvm_link() #7
  %3 = zext i32 %2 to i64
  tail call void @wvm_jump(i64 %3) #7
  ret void
}

declare void @wvm_link(...) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_jlc(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_j_rs(i64 %0) #7
  %3 = tail call i32 @wvm_j_addr(i64 %0) #7
  %4 = zext i8 %2 to i64
  %5 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %4
  %6 = load i64, i64* %5, align 8, !tbaa !5
  %7 = icmp eq i64 %6, 0
  br i1 %7, label %10, label %8

; <label>:8:                                      ; preds = %1
  tail call void (...) @wvm_link() #7
  %9 = zext i32 %3 to i64
  tail call void @wvm_jump(i64 %9) #7
  br label %11

; <label>:10:                                     ; preds = %1
  tail call void (...) @wvm_increment_pc() #7
  br label %11

; <label>:11:                                     ; preds = %10, %8
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_jr(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %4, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  call void @wvm_jump(i64 %8) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_jrc(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %15, label %10

; <label>:10:                                     ; preds = %1
  %11 = load i8, i8* %4, align 1, !tbaa !2
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8, !tbaa !5
  call void @wvm_jump(i64 %14) #7
  br label %16

; <label>:15:                                     ; preds = %1
  call void (...) @wvm_increment_pc() #7
  br label %16

; <label>:16:                                     ; preds = %15, %10
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_jrl(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  call void (...) @wvm_link() #7
  %5 = load i8, i8* %4, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  call void @wvm_jump(i64 %8) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_jrlc(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %15, label %10

; <label>:10:                                     ; preds = %1
  call void (...) @wvm_link() #7
  %11 = load i8, i8* %4, align 1, !tbaa !2
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  %14 = load i64, i64* %13, align 8, !tbaa !5
  call void @wvm_jump(i64 %14) #7
  br label %16

; <label>:15:                                     ; preds = %1
  call void (...) @wvm_increment_pc() #7
  br label %16

; <label>:16:                                     ; preds = %15, %10
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_c(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %4, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %2, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = call i64 @wvm_get_word(i64 %12) #7
  call void @wvm_set_word(i64 %8, i64 %13) #7
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

declare void @wvm_set_word(i64, i64) local_unnamed_addr #1

declare i64 @wvm_get_word(i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_l(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = call i64 @wvm_get_word(i64 %8) #7
  %10 = load i8, i8* %4, align 1, !tbaa !2
  %11 = zext i8 %10 to i64
  %12 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %11
  store i64 %9, i64* %12, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_s(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %4, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %2, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  call void @wvm_set_word(i64 %8, i64 %12) #7
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_cb(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %4, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %2, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = call zeroext i8 @wvm_get_byte(i64 %12) #7
  call void @wvm_set_byte(i64 %8, i8 zeroext %13) #7
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

declare void @wvm_set_byte(i64, i8 zeroext) local_unnamed_addr #1

declare zeroext i8 @wvm_get_byte(i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @op_lb(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %2, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = call zeroext i8 @wvm_get_byte(i64 %8) #7
  %10 = zext i8 %9 to i64
  %11 = load i8, i8* %4, align 1, !tbaa !2
  %12 = zext i8 %11 to i64
  %13 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %12
  store i64 %10, i64* %13, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sb(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i8, i8* %4, align 1, !tbaa !2
  %6 = zext i8 %5 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  %8 = load i64, i64* %7, align 8, !tbaa !5
  %9 = load i8, i8* %2, align 1, !tbaa !2
  %10 = zext i8 %9 to i64
  %11 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %10
  %12 = load i64, i64* %11, align 8, !tbaa !5
  %13 = trunc i64 %12 to i8
  call void @wvm_set_byte(i64 %8, i8 zeroext %13) #7
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_spush(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16, !tbaa !5
  %6 = load i8, i8* %2, align 1, !tbaa !2
  %7 = zext i8 %6 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  %9 = load i64, i64* %8, align 8, !tbaa !5
  call void @wvm_set_word(i64 %5, i64 %9) #7
  %10 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16, !tbaa !5
  %11 = add nsw i64 %10, -8
  store i64 %11, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_spop(i64) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  %3 = alloca i8, align 1
  %4 = alloca i8, align 1
  call void @llvm.lifetime.start(i64 1, i8* nonnull %2) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.start(i64 1, i8* nonnull %4) #7
  call void @wvm_r_regs(i64 %0, i8* nonnull %2, i8* nonnull %3, i8* nonnull %4) #7
  %5 = load i64, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16, !tbaa !5
  %6 = add nsw i64 %5, 8
  store i64 %6, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16, !tbaa !5
  %7 = call i64 @wvm_get_word(i64 %6) #7
  %8 = load i8, i8* %4, align 1, !tbaa !2
  %9 = zext i8 %8 to i64
  %10 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %9
  store i64 %7, i64* %10, align 8, !tbaa !5
  call void (...) @wvm_increment_pc() #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %4) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %3) #7
  call void @llvm.lifetime.end(i64 1, i8* nonnull %2) #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_li(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i32 %3 to i64
  %5 = tail call i64 @wvm_get_word(i64 %4) #7
  %6 = zext i8 %2 to i64
  %7 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %6
  store i64 %5, i64* %7, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_si(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i32 %3 to i64
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  tail call void @wvm_set_word(i64 %4, i64 %7) #7
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_set(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i32 %3 to i64
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  store i64 %4, i64* %6, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lbi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i32 %3 to i64
  %5 = tail call zeroext i8 @wvm_get_byte(i64 %4) #7
  %6 = zext i8 %5 to i64
  %7 = zext i8 %2 to i64
  %8 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %7
  store i64 %6, i64* %8, align 8, !tbaa !5
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sbi(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rs(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i32 %3 to i64
  %5 = zext i8 %2 to i64
  %6 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %5
  %7 = load i64, i64* %6, align 8, !tbaa !5
  %8 = trunc i64 %7 to i8
  tail call void @wvm_set_byte(i64 %4, i8 zeroext %8) #7
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lni(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i8 %2 to i64
  %5 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %4
  %6 = load i64, i64* %5, align 8, !tbaa !5
  %7 = zext i32 %3 to i64
  %8 = tail call i64 @wvm_get_word(i64 %7) #7
  tail call void @wvm_set_word(i64 %6, i64 %8) #7
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_lbni(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_i_rd(i64 %0) #7
  %3 = tail call i32 @wvm_i_imm(i64 %0) #7
  %4 = zext i8 %2 to i64
  %5 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %4
  %6 = load i64, i64* %5, align 8, !tbaa !5
  %7 = zext i32 %3 to i64
  %8 = tail call zeroext i8 @wvm_get_byte(i64 %7) #7
  tail call void @wvm_set_byte(i64 %6, i8 zeroext %8) #7
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_pr(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_r_rs(i64 %0) #7
  %3 = tail call i8* @wvm_decode_reg(i8 zeroext %2) #7
  %4 = zext i8 %2 to i64
  %5 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %4
  %6 = load i64, i64* %5, align 8, !tbaa !5
  %7 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @.str, i64 0, i64 0), i8* %3, i64 %6, i64 %6)
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #3

declare i8* @wvm_decode_reg(i8 zeroext) local_unnamed_addr #1

; Function Attrs: norecurse nounwind ssp uwtable
define void @op_halt(i64) local_unnamed_addr #4 {
  store i8 0, i8* @alive, align 1, !tbaa !7
  ret void
}

; Function Attrs: noreturn nounwind ssp uwtable
define void @op_eval(i64) local_unnamed_addr #5 {
  %2 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !9
  %3 = tail call i64 @fwrite(i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.1, i64 0, i64 0), i64 25, i64 1, %struct.__sFILE* %2)
  tail call void @exit(i32 1) #8
  unreachable
}

; Function Attrs: noreturn
declare void @exit(i32) local_unnamed_addr #6

; Function Attrs: nounwind ssp uwtable
define void @op_prc(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_r_rs(i64 %0) #7
  %3 = zext i8 %2 to i64
  %4 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %3
  %5 = load i64, i64* %4, align 8, !tbaa !5
  %6 = trunc i64 %5 to i32
  %7 = shl i32 %6, 24
  %8 = ashr exact i32 %7, 24
  %9 = tail call i32 @putchar(i32 %8)
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind
declare i32 @putchar(i32) local_unnamed_addr #3

; Function Attrs: nounwind ssp uwtable
define void @op_prd(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_r_rs(i64 %0) #7
  %3 = zext i8 %2 to i64
  %4 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %3
  %5 = load i64, i64* %4, align 8, !tbaa !5
  %6 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2, i64 0, i64 0), i64 %5)
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_prx(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_r_rs(i64 %0) #7
  %3 = zext i8 %2 to i64
  %4 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %3
  %5 = load i64, i64* %4, align 8, !tbaa !5
  %6 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.3, i64 0, i64 0), i64 %5)
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

; Function Attrs: nounwind ssp uwtable
define void @op_sleep(i64) local_unnamed_addr #0 {
  %2 = tail call zeroext i8 @wvm_r_rs(i64 %0) #7
  %3 = zext i8 %2 to i64
  %4 = getelementptr inbounds [128 x i64], [128 x i64]* @registers, i64 0, i64 %3
  %5 = load i64, i64* %4, align 8, !tbaa !5
  %6 = mul nsw i64 %5, 1000
  %7 = trunc i64 %6 to i32
  %8 = tail call i32 @"\01_usleep"(i32 %7) #7
  tail call void (...) @wvm_increment_pc() #7
  ret void
}

declare i32 @"\01_usleep"(i32) local_unnamed_addr #1

; Function Attrs: nounwind
declare i64 @fwrite(i8* nocapture, i64, i64, %struct.__sFILE* nocapture) #7

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { norecurse nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noreturn nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nounwind }
attributes #8 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
!5 = !{!6, !6, i64 0}
!6 = !{!"long long", !3, i64 0}
!7 = !{!8, !8, i64 0}
!8 = !{!"_Bool", !3, i64 0}
!9 = !{!10, !10, i64 0}
!10 = !{!"any pointer", !3, i64 0}
