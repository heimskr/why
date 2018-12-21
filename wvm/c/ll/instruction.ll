; ModuleID = 'instruction.c'
source_filename = "instruction.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

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
@cycles = common local_unnamed_addr global i32 0, align 4
@alive = common local_unnamed_addr global i8 0, align 1
@switch.table = private unnamed_addr constant [10 x void (i64)*] [void (i64)* @op_add, void (i64)* @op_sub, void (i64)* @op_mult, void (i64)* @op_addu, void (i64)* @op_subu, void (i64)* @op_multu, void (i64)* @op_sll, void (i64)* @op_srl, void (i64)* @op_sra, void (i64)* @op_mod]
@switch.table.1 = private unnamed_addr constant [15 x void (i64)*] [void (i64)* @op_and, void (i64)* @op_nand, void (i64)* @op_nor, void (i64)* @op_not, void (i64)* @op_or, void (i64)* @op_xnor, void (i64)* @op_xor, void (i64)* @op_addi, void (i64)* @op_land, void (i64)* @op_lnand, void (i64)* @op_lnor, void (i64)* @op_lnot, void (i64)* @op_lor, void (i64)* @op_lxnor, void (i64)* @op_lxor]
@switch.table.2 = private unnamed_addr constant [5 x void (i64)*] [void (i64)* @op_sl, void (i64)* @op_sle, void (i64)* @op_seq, void (i64)* @op_slu, void (i64)* @op_sleu]
@switch.table.3 = private unnamed_addr constant [4 x void (i64)*] [void (i64)* @op_jr, void (i64)* @op_jrc, void (i64)* @op_jrl, void (i64)* @op_jrlc]
@switch.table.4 = private unnamed_addr constant [8 x void (i64)*] [void (i64)* @op_c, void (i64)* @op_l, void (i64)* @op_s, void (i64)* @op_cb, void (i64)* @op_lb, void (i64)* @op_sb, void (i64)* @op_spush, void (i64)* @op_spop]

; Function Attrs: norecurse nounwind readnone ssp uwtable
define zeroext i16 @wvm_get_opcode(i64) local_unnamed_addr #0 {
  %2 = ashr i64 %0, 52
  %3 = trunc i64 %2 to i16
  ret i16 %3
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define i32 @wvm_get_type(i16 zeroext) local_unnamed_addr #0 {
  switch i16 %0, label %3 [
    i16 1, label %4
    i16 2, label %4
    i16 14, label %4
    i16 17, label %4
    i16 18, label %4
    i16 31, label %4
    i16 15, label %2
    i16 16, label %2
    i16 32, label %2
    i16 33, label %2
  ]

; <label>:2:                                      ; preds = %1, %1, %1, %1
  br label %4

; <label>:3:                                      ; preds = %1
  br label %4

; <label>:4:                                      ; preds = %1, %1, %1, %1, %1, %1, %3, %2
  %5 = phi i32 [ 1, %3 ], [ 0, %2 ], [ 2, %1 ], [ 2, %1 ], [ 2, %1 ], [ 2, %1 ], [ 2, %1 ], [ 2, %1 ]
  ret i32 %5
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define void (i64)* @wvm_get_fn(i64) local_unnamed_addr #0 {
  %2 = ashr i64 %0, 52
  %3 = trunc i64 %2 to i16
  switch i16 %3, label %75 [
    i16 0, label %96
    i16 1, label %10
    i16 2, label %4
    i16 3, label %17
    i16 4, label %18
    i16 5, label %19
    i16 6, label %20
    i16 7, label %21
    i16 8, label %22
    i16 9, label %23
    i16 10, label %24
    i16 11, label %25
    i16 13, label %26
    i16 14, label %27
    i16 15, label %31
    i16 16, label %32
    i16 17, label %33
    i16 18, label %7
    i16 19, label %40
    i16 20, label %41
    i16 21, label %42
    i16 22, label %43
    i16 23, label %44
    i16 24, label %45
    i16 25, label %46
    i16 26, label %47
    i16 41, label %48
    i16 42, label %49
    i16 27, label %50
    i16 28, label %51
    i16 29, label %52
    i16 30, label %53
    i16 31, label %54
    i16 32, label %66
    i16 33, label %67
    i16 34, label %68
    i16 35, label %69
    i16 36, label %70
    i16 37, label %71
    i16 38, label %72
    i16 39, label %73
    i16 40, label %74
  ]

; <label>:4:                                      ; preds = %1
  %5 = trunc i64 %0 to i32
  %6 = and i32 %5, 4095
  br label %14

; <label>:7:                                      ; preds = %1
  %8 = trunc i64 %0 to i32
  %9 = and i32 %8, 4095
  br label %37

; <label>:10:                                     ; preds = %1
  %11 = trunc i64 %0 to i32
  %12 = and i32 %11, 4095
  %13 = icmp ult i32 %12, 10
  br i1 %13, label %76, label %14

; <label>:14:                                     ; preds = %4, %10
  %15 = phi i32 [ %6, %4 ], [ %12, %10 ]
  %16 = icmp ult i32 %15, 15
  br i1 %16, label %80, label %17

; <label>:17:                                     ; preds = %14, %1
  br label %96

; <label>:18:                                     ; preds = %1
  br label %96

; <label>:19:                                     ; preds = %1
  br label %96

; <label>:20:                                     ; preds = %1
  br label %96

; <label>:21:                                     ; preds = %1
  br label %96

; <label>:22:                                     ; preds = %1
  br label %96

; <label>:23:                                     ; preds = %1
  br label %96

; <label>:24:                                     ; preds = %1
  br label %96

; <label>:25:                                     ; preds = %1
  br label %96

; <label>:26:                                     ; preds = %1
  br label %96

; <label>:27:                                     ; preds = %1
  %28 = trunc i64 %0 to i32
  %29 = and i32 %28, 4095
  %30 = icmp ult i32 %29, 5
  br i1 %30, label %84, label %31

; <label>:31:                                     ; preds = %27, %1
  br label %96

; <label>:32:                                     ; preds = %1
  br label %96

; <label>:33:                                     ; preds = %1
  %34 = trunc i64 %0 to i32
  %35 = and i32 %34, 4095
  %36 = icmp ult i32 %35, 4
  br i1 %36, label %88, label %37

; <label>:37:                                     ; preds = %7, %33
  %38 = phi i32 [ %9, %7 ], [ %35, %33 ]
  %39 = icmp ult i32 %38, 8
  br i1 %39, label %92, label %40

; <label>:40:                                     ; preds = %37, %1
  br label %96

; <label>:41:                                     ; preds = %1
  br label %96

; <label>:42:                                     ; preds = %1
  br label %96

; <label>:43:                                     ; preds = %1
  br label %96

; <label>:44:                                     ; preds = %1
  br label %96

; <label>:45:                                     ; preds = %1
  br label %96

; <label>:46:                                     ; preds = %1
  br label %96

; <label>:47:                                     ; preds = %1
  br label %96

; <label>:48:                                     ; preds = %1
  br label %96

; <label>:49:                                     ; preds = %1
  br label %96

; <label>:50:                                     ; preds = %1
  br label %96

; <label>:51:                                     ; preds = %1
  br label %96

; <label>:52:                                     ; preds = %1
  br label %96

; <label>:53:                                     ; preds = %1
  br label %96

; <label>:54:                                     ; preds = %1
  %55 = trunc i64 %0 to i12
  switch i12 %55, label %66 [
    i12 1, label %96
    i12 2, label %56
    i12 3, label %57
    i12 4, label %58
    i12 5, label %59
    i12 6, label %60
    i12 7, label %61
    i12 128, label %62
    i12 129, label %63
    i12 130, label %64
    i12 131, label %65
  ]

; <label>:56:                                     ; preds = %54
  br label %96

; <label>:57:                                     ; preds = %54
  br label %96

; <label>:58:                                     ; preds = %54
  br label %96

; <label>:59:                                     ; preds = %54
  br label %96

; <label>:60:                                     ; preds = %54
  br label %96

; <label>:61:                                     ; preds = %54
  br label %96

; <label>:62:                                     ; preds = %54
  br label %96

; <label>:63:                                     ; preds = %54
  br label %96

; <label>:64:                                     ; preds = %54
  br label %96

; <label>:65:                                     ; preds = %54
  br label %96

; <label>:66:                                     ; preds = %54, %1
  br label %96

; <label>:67:                                     ; preds = %1
  br label %96

; <label>:68:                                     ; preds = %1
  br label %96

; <label>:69:                                     ; preds = %1
  br label %96

; <label>:70:                                     ; preds = %1
  br label %96

; <label>:71:                                     ; preds = %1
  br label %96

; <label>:72:                                     ; preds = %1
  br label %96

; <label>:73:                                     ; preds = %1
  br label %96

; <label>:74:                                     ; preds = %1
  br label %96

; <label>:75:                                     ; preds = %1
  br label %96

; <label>:76:                                     ; preds = %10
  %77 = zext i32 %12 to i64
  %78 = getelementptr inbounds [10 x void (i64)*], [10 x void (i64)*]* @switch.table, i64 0, i64 %77
  %79 = load void (i64)*, void (i64)** %78, align 8
  ret void (i64)* %79

; <label>:80:                                     ; preds = %14
  %81 = zext i32 %15 to i64
  %82 = getelementptr inbounds [15 x void (i64)*], [15 x void (i64)*]* @switch.table.1, i64 0, i64 %81
  %83 = load void (i64)*, void (i64)** %82, align 8
  ret void (i64)* %83

; <label>:84:                                     ; preds = %27
  %85 = zext i32 %29 to i64
  %86 = getelementptr inbounds [5 x void (i64)*], [5 x void (i64)*]* @switch.table.2, i64 0, i64 %85
  %87 = load void (i64)*, void (i64)** %86, align 8
  ret void (i64)* %87

; <label>:88:                                     ; preds = %33
  %89 = zext i32 %35 to i64
  %90 = getelementptr inbounds [4 x void (i64)*], [4 x void (i64)*]* @switch.table.3, i64 0, i64 %89
  %91 = load void (i64)*, void (i64)** %90, align 8
  ret void (i64)* %91

; <label>:92:                                     ; preds = %37
  %93 = zext i32 %38 to i64
  %94 = getelementptr inbounds [8 x void (i64)*], [8 x void (i64)*]* @switch.table.4, i64 0, i64 %93
  %95 = load void (i64)*, void (i64)** %94, align 8
  ret void (i64)* %95

; <label>:96:                                     ; preds = %54, %1, %75, %74, %73, %72, %71, %70, %69, %68, %67, %66, %65, %64, %63, %62, %61, %60, %59, %58, %57, %56, %53, %52, %51, %50, %49, %48, %47, %46, %45, %44, %43, %42, %41, %40, %32, %31, %26, %25, %24, %23, %22, %21, %20, %19, %18, %17
  %97 = phi void (i64)* [ null, %75 ], [ @op_lbni, %74 ], [ @op_lni, %73 ], [ @op_sbi, %72 ], [ @op_lbi, %71 ], [ @op_srai, %70 ], [ @op_srli, %69 ], [ @op_slli, %68 ], [ @op_jlc, %67 ], [ @op_jl, %66 ], [ @op_xn_recv, %65 ], [ @op_xn_send, %64 ], [ @op_xn_connect, %63 ], [ @op_xn_init, %62 ], [ @op_sleep, %61 ], [ @op_prx, %60 ], [ @op_prd, %59 ], [ @op_prc, %58 ], [ @op_eval, %57 ], [ @op_halt, %56 ], [ @op_modi, %53 ], [ @op_sleui, %52 ], [ @op_slui, %51 ], [ @op_seqi, %50 ], [ @op_sgei, %49 ], [ @op_sgi, %48 ], [ @op_slei, %47 ], [ @op_sli, %46 ], [ @op_multui, %45 ], [ @op_subui, %44 ], [ @op_addui, %43 ], [ @op_set, %42 ], [ @op_si, %41 ], [ @op_li, %40 ], [ @op_jc, %32 ], [ @op_j, %31 ], [ @op_lui, %26 ], [ @op_xori, %25 ], [ @op_xnori, %24 ], [ @op_ori, %23 ], [ @op_nori, %22 ], [ @op_nandi, %21 ], [ @op_andi, %20 ], [ @op_multi, %19 ], [ @op_subi, %18 ], [ @op_addi, %17 ], [ @op_nop, %1 ], [ @op_pr, %54 ]
  ret void (i64)* %97
}

declare void @op_nop(i64) #1

; Function Attrs: norecurse nounwind readnone ssp uwtable
define i32 @wvm_r_func(i64) local_unnamed_addr #0 {
  %2 = trunc i64 %0 to i32
  %3 = and i32 %2, 4095
  ret i32 %3
}

declare void @op_add(i64) #1

declare void @op_sub(i64) #1

declare void @op_mult(i64) #1

declare void @op_addu(i64) #1

declare void @op_subu(i64) #1

declare void @op_multu(i64) #1

declare void @op_sll(i64) #1

declare void @op_srl(i64) #1

declare void @op_sra(i64) #1

declare void @op_mod(i64) #1

declare void @op_and(i64) #1

declare void @op_nand(i64) #1

declare void @op_nor(i64) #1

declare void @op_not(i64) #1

declare void @op_or(i64) #1

declare void @op_xnor(i64) #1

declare void @op_xor(i64) #1

declare void @op_land(i64) #1

declare void @op_lnand(i64) #1

declare void @op_lnor(i64) #1

declare void @op_lnot(i64) #1

declare void @op_lor(i64) #1

declare void @op_lxnor(i64) #1

declare void @op_lxor(i64) #1

declare void @op_addi(i64) #1

declare void @op_subi(i64) #1

declare void @op_multi(i64) #1

declare void @op_andi(i64) #1

declare void @op_nandi(i64) #1

declare void @op_nori(i64) #1

declare void @op_ori(i64) #1

declare void @op_xnori(i64) #1

declare void @op_xori(i64) #1

declare void @op_lui(i64) #1

declare void @op_sl(i64) #1

declare void @op_sle(i64) #1

declare void @op_seq(i64) #1

declare void @op_slu(i64) #1

declare void @op_sleu(i64) #1

declare void @op_j(i64) #1

declare void @op_jc(i64) #1

declare void @op_jr(i64) #1

declare void @op_jrc(i64) #1

declare void @op_jrl(i64) #1

declare void @op_jrlc(i64) #1

declare void @op_c(i64) #1

declare void @op_l(i64) #1

declare void @op_s(i64) #1

declare void @op_cb(i64) #1

declare void @op_lb(i64) #1

declare void @op_sb(i64) #1

declare void @op_spush(i64) #1

declare void @op_spop(i64) #1

declare void @op_li(i64) #1

declare void @op_si(i64) #1

declare void @op_set(i64) #1

declare void @op_addui(i64) #1

declare void @op_subui(i64) #1

declare void @op_multui(i64) #1

declare void @op_sli(i64) #1

declare void @op_slei(i64) #1

declare void @op_sgi(i64) #1

declare void @op_sgei(i64) #1

declare void @op_seqi(i64) #1

declare void @op_slui(i64) #1

declare void @op_sleui(i64) #1

declare void @op_modi(i64) #1

declare void @op_pr(i64) #1

declare void @op_halt(i64) #1

declare void @op_eval(i64) #1

declare void @op_prc(i64) #1

declare void @op_prd(i64) #1

declare void @op_prx(i64) #1

declare void @op_sleep(i64) #1

declare void @op_xn_init(i64) #1

declare void @op_xn_connect(i64) #1

declare void @op_xn_send(i64) #1

declare void @op_xn_recv(i64) #1

declare void @op_jl(i64) #1

declare void @op_jlc(i64) #1

declare void @op_slli(i64) #1

declare void @op_srli(i64) #1

declare void @op_srai(i64) #1

declare void @op_lbi(i64) #1

declare void @op_sbi(i64) #1

declare void @op_lni(i64) #1

declare void @op_lbni(i64) #1

; Function Attrs: norecurse nounwind readnone ssp uwtable
define zeroext i8 @wvm_r_rt(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 45
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 127
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define zeroext i8 @wvm_r_rs(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 38
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 127
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define zeroext i8 @wvm_r_rd(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 31
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 127
  ret i8 %4
}

; Function Attrs: norecurse nounwind ssp uwtable
define void @wvm_r_regs(i64, i8* nocapture, i8* nocapture, i8* nocapture) local_unnamed_addr #2 {
  %5 = lshr i64 %0, 38
  %6 = trunc i64 %5 to i8
  %7 = and i8 %6, 127
  store i8 %7, i8* %1, align 1, !tbaa !2
  %8 = lshr i64 %0, 45
  %9 = trunc i64 %8 to i8
  %10 = and i8 %9, 127
  store i8 %10, i8* %2, align 1, !tbaa !2
  %11 = lshr i64 %0, 31
  %12 = trunc i64 %11 to i8
  %13 = and i8 %12, 127
  store i8 %13, i8* %3, align 1, !tbaa !2
  ret void
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define signext i8 @wvm_r_flags(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 12
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 63
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define signext i8 @wvm_i_flags(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 46
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 63
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define zeroext i8 @wvm_i_rs(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 39
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 127
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define zeroext i8 @wvm_i_rd(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 32
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 127
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define i32 @wvm_i_imm(i64) local_unnamed_addr #0 {
  %2 = trunc i64 %0 to i32
  ret i32 %2
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define zeroext i8 @wvm_j_rs(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 45
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 127
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define signext i8 @wvm_j_flags(i64) local_unnamed_addr #0 {
  %2 = lshr i64 %0, 32
  %3 = trunc i64 %2 to i8
  %4 = and i8 %3, 63
  ret i8 %4
}

; Function Attrs: norecurse nounwind readnone ssp uwtable
define i32 @wvm_j_addr(i64) local_unnamed_addr #0 {
  %2 = trunc i64 %0 to i32
  ret i32 %2
}

attributes #0 = { norecurse nounwind readnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { norecurse nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
