; ModuleID = 'instruction.c'
source_filename = "instruction.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

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

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i16 @wvm_get_opcode(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 52
  %5 = trunc i64 %4 to i16
  ret i16 %5
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_get_type(i16 zeroext) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i16, align 2
  store i16 %0, i16* %3, align 2
  %4 = load i16, i16* %3, align 2
  %5 = zext i16 %4 to i32
  switch i32 %5, label %8 [
    i32 1, label %6
    i32 2, label %6
    i32 14, label %6
    i32 17, label %6
    i32 18, label %6
    i32 31, label %6
    i32 48, label %6
    i32 50, label %6
    i32 15, label %7
    i32 16, label %7
  ]

; <label>:6:                                      ; preds = %1, %1, %1, %1, %1, %1, %1, %1
  store i32 2, i32* %2, align 4
  br label %9

; <label>:7:                                      ; preds = %1, %1
  store i32 0, i32* %2, align 4
  br label %9

; <label>:8:                                      ; preds = %1
  store i32 1, i32* %2, align 4
  br label %9

; <label>:9:                                      ; preds = %8, %7, %6
  %10 = load i32, i32* %2, align 4
  ret i32 %10
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void (i64)* @wvm_get_fn(i64) #0 {
  %2 = alloca void (i64)*, align 8
  %3 = alloca i64, align 8
  store i64 %0, i64* %3, align 8
  %4 = load i64, i64* %3, align 8
  %5 = call zeroext i16 @wvm_get_opcode(i64 %4)
  %6 = zext i16 %5 to i32
  switch i32 %6, label %119 [
    i32 0, label %7
    i32 1, label %8
    i32 2, label %20
    i32 3, label %38
    i32 4, label %39
    i32 5, label %40
    i32 6, label %41
    i32 7, label %42
    i32 8, label %43
    i32 9, label %44
    i32 10, label %45
    i32 11, label %46
    i32 13, label %47
    i32 14, label %48
    i32 15, label %57
    i32 16, label %58
    i32 17, label %59
    i32 18, label %67
    i32 19, label %79
    i32 20, label %80
    i32 21, label %81
    i32 24, label %82
    i32 25, label %83
    i32 26, label %84
    i32 41, label %85
    i32 42, label %86
    i32 27, label %87
    i32 28, label %88
    i32 29, label %89
    i32 30, label %90
    i32 31, label %91
    i32 34, label %106
    i32 35, label %107
    i32 36, label %108
    i32 37, label %109
    i32 38, label %110
    i32 39, label %111
    i32 40, label %112
    i32 32, label %113
    i32 33, label %114
    i32 48, label %115
    i32 49, label %116
    i32 50, label %117
    i32 51, label %118
  ]

; <label>:7:                                      ; preds = %1
  store void (i64)* @op_nop, void (i64)** %2, align 8
  br label %120

; <label>:8:                                      ; preds = %1
  %9 = load i64, i64* %3, align 8
  %10 = call i32 @wvm_r_func(i64 %9)
  switch i32 %10, label %19 [
    i32 0, label %11
    i32 1, label %12
    i32 2, label %13
    i32 5, label %14
    i32 6, label %15
    i32 7, label %16
    i32 8, label %17
    i32 9, label %18
  ]

; <label>:11:                                     ; preds = %8
  store void (i64)* @op_add, void (i64)** %2, align 8
  br label %120

; <label>:12:                                     ; preds = %8
  store void (i64)* @op_sub, void (i64)** %2, align 8
  br label %120

; <label>:13:                                     ; preds = %8
  store void (i64)* @op_mult, void (i64)** %2, align 8
  br label %120

; <label>:14:                                     ; preds = %8
  store void (i64)* @op_multu, void (i64)** %2, align 8
  br label %120

; <label>:15:                                     ; preds = %8
  store void (i64)* @op_sll, void (i64)** %2, align 8
  br label %120

; <label>:16:                                     ; preds = %8
  store void (i64)* @op_srl, void (i64)** %2, align 8
  br label %120

; <label>:17:                                     ; preds = %8
  store void (i64)* @op_sra, void (i64)** %2, align 8
  br label %120

; <label>:18:                                     ; preds = %8
  store void (i64)* @op_mod, void (i64)** %2, align 8
  br label %120

; <label>:19:                                     ; preds = %8
  br label %20

; <label>:20:                                     ; preds = %1, %19
  %21 = load i64, i64* %3, align 8
  %22 = call i32 @wvm_r_func(i64 %21)
  switch i32 %22, label %37 [
    i32 0, label %23
    i32 1, label %24
    i32 2, label %25
    i32 3, label %26
    i32 4, label %27
    i32 5, label %28
    i32 6, label %29
    i32 8, label %30
    i32 9, label %31
    i32 10, label %32
    i32 11, label %33
    i32 12, label %34
    i32 13, label %35
    i32 14, label %36
  ]

; <label>:23:                                     ; preds = %20
  store void (i64)* @op_and, void (i64)** %2, align 8
  br label %120

; <label>:24:                                     ; preds = %20
  store void (i64)* @op_nand, void (i64)** %2, align 8
  br label %120

; <label>:25:                                     ; preds = %20
  store void (i64)* @op_nor, void (i64)** %2, align 8
  br label %120

; <label>:26:                                     ; preds = %20
  store void (i64)* @op_not, void (i64)** %2, align 8
  br label %120

; <label>:27:                                     ; preds = %20
  store void (i64)* @op_or, void (i64)** %2, align 8
  br label %120

; <label>:28:                                     ; preds = %20
  store void (i64)* @op_xnor, void (i64)** %2, align 8
  br label %120

; <label>:29:                                     ; preds = %20
  store void (i64)* @op_xor, void (i64)** %2, align 8
  br label %120

; <label>:30:                                     ; preds = %20
  store void (i64)* @op_land, void (i64)** %2, align 8
  br label %120

; <label>:31:                                     ; preds = %20
  store void (i64)* @op_lnand, void (i64)** %2, align 8
  br label %120

; <label>:32:                                     ; preds = %20
  store void (i64)* @op_lnor, void (i64)** %2, align 8
  br label %120

; <label>:33:                                     ; preds = %20
  store void (i64)* @op_lnot, void (i64)** %2, align 8
  br label %120

; <label>:34:                                     ; preds = %20
  store void (i64)* @op_lor, void (i64)** %2, align 8
  br label %120

; <label>:35:                                     ; preds = %20
  store void (i64)* @op_lxnor, void (i64)** %2, align 8
  br label %120

; <label>:36:                                     ; preds = %20
  store void (i64)* @op_lxor, void (i64)** %2, align 8
  br label %120

; <label>:37:                                     ; preds = %20
  br label %38

; <label>:38:                                     ; preds = %1, %37
  store void (i64)* @op_addi, void (i64)** %2, align 8
  br label %120

; <label>:39:                                     ; preds = %1
  store void (i64)* @op_subi, void (i64)** %2, align 8
  br label %120

; <label>:40:                                     ; preds = %1
  store void (i64)* @op_multi, void (i64)** %2, align 8
  br label %120

; <label>:41:                                     ; preds = %1
  store void (i64)* @op_andi, void (i64)** %2, align 8
  br label %120

; <label>:42:                                     ; preds = %1
  store void (i64)* @op_nandi, void (i64)** %2, align 8
  br label %120

; <label>:43:                                     ; preds = %1
  store void (i64)* @op_nori, void (i64)** %2, align 8
  br label %120

; <label>:44:                                     ; preds = %1
  store void (i64)* @op_ori, void (i64)** %2, align 8
  br label %120

; <label>:45:                                     ; preds = %1
  store void (i64)* @op_xnori, void (i64)** %2, align 8
  br label %120

; <label>:46:                                     ; preds = %1
  store void (i64)* @op_xori, void (i64)** %2, align 8
  br label %120

; <label>:47:                                     ; preds = %1
  store void (i64)* @op_lui, void (i64)** %2, align 8
  br label %120

; <label>:48:                                     ; preds = %1
  %49 = load i64, i64* %3, align 8
  %50 = call i32 @wvm_r_func(i64 %49)
  switch i32 %50, label %56 [
    i32 0, label %51
    i32 1, label %52
    i32 2, label %53
    i32 3, label %54
    i32 4, label %55
  ]

; <label>:51:                                     ; preds = %48
  store void (i64)* @op_sl, void (i64)** %2, align 8
  br label %120

; <label>:52:                                     ; preds = %48
  store void (i64)* @op_sle, void (i64)** %2, align 8
  br label %120

; <label>:53:                                     ; preds = %48
  store void (i64)* @op_seq, void (i64)** %2, align 8
  br label %120

; <label>:54:                                     ; preds = %48
  store void (i64)* @op_slu, void (i64)** %2, align 8
  br label %120

; <label>:55:                                     ; preds = %48
  store void (i64)* @op_sleu, void (i64)** %2, align 8
  br label %120

; <label>:56:                                     ; preds = %48
  br label %57

; <label>:57:                                     ; preds = %1, %56
  store void (i64)* @op_j, void (i64)** %2, align 8
  br label %120

; <label>:58:                                     ; preds = %1
  store void (i64)* @op_jc, void (i64)** %2, align 8
  br label %120

; <label>:59:                                     ; preds = %1
  %60 = load i64, i64* %3, align 8
  %61 = call i32 @wvm_r_func(i64 %60)
  switch i32 %61, label %66 [
    i32 0, label %62
    i32 1, label %63
    i32 2, label %64
    i32 3, label %65
  ]

; <label>:62:                                     ; preds = %59
  store void (i64)* @op_jr, void (i64)** %2, align 8
  br label %120

; <label>:63:                                     ; preds = %59
  store void (i64)* @op_jrc, void (i64)** %2, align 8
  br label %120

; <label>:64:                                     ; preds = %59
  store void (i64)* @op_jrl, void (i64)** %2, align 8
  br label %120

; <label>:65:                                     ; preds = %59
  store void (i64)* @op_jrlc, void (i64)** %2, align 8
  br label %120

; <label>:66:                                     ; preds = %59
  br label %67

; <label>:67:                                     ; preds = %1, %66
  %68 = load i64, i64* %3, align 8
  %69 = call i32 @wvm_r_func(i64 %68)
  switch i32 %69, label %78 [
    i32 0, label %70
    i32 1, label %71
    i32 2, label %72
    i32 3, label %73
    i32 4, label %74
    i32 5, label %75
    i32 6, label %76
    i32 7, label %77
  ]

; <label>:70:                                     ; preds = %67
  store void (i64)* @op_c, void (i64)** %2, align 8
  br label %120

; <label>:71:                                     ; preds = %67
  store void (i64)* @op_l, void (i64)** %2, align 8
  br label %120

; <label>:72:                                     ; preds = %67
  store void (i64)* @op_s, void (i64)** %2, align 8
  br label %120

; <label>:73:                                     ; preds = %67
  store void (i64)* @op_cb, void (i64)** %2, align 8
  br label %120

; <label>:74:                                     ; preds = %67
  store void (i64)* @op_lb, void (i64)** %2, align 8
  br label %120

; <label>:75:                                     ; preds = %67
  store void (i64)* @op_sb, void (i64)** %2, align 8
  br label %120

; <label>:76:                                     ; preds = %67
  store void (i64)* @op_spush, void (i64)** %2, align 8
  br label %120

; <label>:77:                                     ; preds = %67
  store void (i64)* @op_spop, void (i64)** %2, align 8
  br label %120

; <label>:78:                                     ; preds = %67
  br label %79

; <label>:79:                                     ; preds = %1, %78
  store void (i64)* @op_li, void (i64)** %2, align 8
  br label %120

; <label>:80:                                     ; preds = %1
  store void (i64)* @op_si, void (i64)** %2, align 8
  br label %120

; <label>:81:                                     ; preds = %1
  store void (i64)* @op_set, void (i64)** %2, align 8
  br label %120

; <label>:82:                                     ; preds = %1
  store void (i64)* @op_multui, void (i64)** %2, align 8
  br label %120

; <label>:83:                                     ; preds = %1
  store void (i64)* @op_sli, void (i64)** %2, align 8
  br label %120

; <label>:84:                                     ; preds = %1
  store void (i64)* @op_slei, void (i64)** %2, align 8
  br label %120

; <label>:85:                                     ; preds = %1
  store void (i64)* @op_sgi, void (i64)** %2, align 8
  br label %120

; <label>:86:                                     ; preds = %1
  store void (i64)* @op_sgei, void (i64)** %2, align 8
  br label %120

; <label>:87:                                     ; preds = %1
  store void (i64)* @op_seqi, void (i64)** %2, align 8
  br label %120

; <label>:88:                                     ; preds = %1
  store void (i64)* @op_slui, void (i64)** %2, align 8
  br label %120

; <label>:89:                                     ; preds = %1
  store void (i64)* @op_sleui, void (i64)** %2, align 8
  br label %120

; <label>:90:                                     ; preds = %1
  store void (i64)* @op_modi, void (i64)** %2, align 8
  br label %120

; <label>:91:                                     ; preds = %1
  %92 = load i64, i64* %3, align 8
  %93 = call i32 @wvm_r_func(i64 %92)
  switch i32 %93, label %105 [
    i32 1, label %94
    i32 2, label %95
    i32 3, label %96
    i32 4, label %97
    i32 5, label %98
    i32 6, label %99
    i32 7, label %100
    i32 128, label %101
    i32 129, label %102
    i32 130, label %103
    i32 131, label %104
  ]

; <label>:94:                                     ; preds = %91
  store void (i64)* @op_pr, void (i64)** %2, align 8
  br label %120

; <label>:95:                                     ; preds = %91
  store void (i64)* @op_halt, void (i64)** %2, align 8
  br label %120

; <label>:96:                                     ; preds = %91
  store void (i64)* @op_eval, void (i64)** %2, align 8
  br label %120

; <label>:97:                                     ; preds = %91
  store void (i64)* @op_prc, void (i64)** %2, align 8
  br label %120

; <label>:98:                                     ; preds = %91
  store void (i64)* @op_prd, void (i64)** %2, align 8
  br label %120

; <label>:99:                                     ; preds = %91
  store void (i64)* @op_prx, void (i64)** %2, align 8
  br label %120

; <label>:100:                                    ; preds = %91
  store void (i64)* @op_sleep, void (i64)** %2, align 8
  br label %120

; <label>:101:                                    ; preds = %91
  store void (i64)* @op_xn_init, void (i64)** %2, align 8
  br label %120

; <label>:102:                                    ; preds = %91
  store void (i64)* @op_xn_connect, void (i64)** %2, align 8
  br label %120

; <label>:103:                                    ; preds = %91
  store void (i64)* @op_xn_send, void (i64)** %2, align 8
  br label %120

; <label>:104:                                    ; preds = %91
  store void (i64)* @op_xn_recv, void (i64)** %2, align 8
  br label %120

; <label>:105:                                    ; preds = %91
  br label %106

; <label>:106:                                    ; preds = %1, %105
  store void (i64)* @op_slli, void (i64)** %2, align 8
  br label %120

; <label>:107:                                    ; preds = %1
  store void (i64)* @op_srli, void (i64)** %2, align 8
  br label %120

; <label>:108:                                    ; preds = %1
  store void (i64)* @op_srai, void (i64)** %2, align 8
  br label %120

; <label>:109:                                    ; preds = %1
  store void (i64)* @op_lbi, void (i64)** %2, align 8
  br label %120

; <label>:110:                                    ; preds = %1
  store void (i64)* @op_sbi, void (i64)** %2, align 8
  br label %120

; <label>:111:                                    ; preds = %1
  store void (i64)* @op_lni, void (i64)** %2, align 8
  br label %120

; <label>:112:                                    ; preds = %1
  store void (i64)* @op_lbni, void (i64)** %2, align 8
  br label %120

; <label>:113:                                    ; preds = %1
  store void (i64)* @op_int, void (i64)** %2, align 8
  br label %120

; <label>:114:                                    ; preds = %1
  store void (i64)* @op_rit, void (i64)** %2, align 8
  br label %120

; <label>:115:                                    ; preds = %1
  store void (i64)* @op_time, void (i64)** %2, align 8
  br label %120

; <label>:116:                                    ; preds = %1
  store void (i64)* @op_timei, void (i64)** %2, align 8
  br label %120

; <label>:117:                                    ; preds = %1
  store void (i64)* @op_ring, void (i64)** %2, align 8
  br label %120

; <label>:118:                                    ; preds = %1
  store void (i64)* @op_ringi, void (i64)** %2, align 8
  br label %120

; <label>:119:                                    ; preds = %1
  store void (i64)* null, void (i64)** %2, align 8
  br label %120

; <label>:120:                                    ; preds = %119, %118, %117, %116, %115, %114, %113, %112, %111, %110, %109, %108, %107, %106, %104, %103, %102, %101, %100, %99, %98, %97, %96, %95, %94, %90, %89, %88, %87, %86, %85, %84, %83, %82, %81, %80, %79, %77, %76, %75, %74, %73, %72, %71, %70, %65, %64, %63, %62, %58, %57, %55, %54, %53, %52, %51, %47, %46, %45, %44, %43, %42, %41, %40, %39, %38, %36, %35, %34, %33, %32, %31, %30, %29, %28, %27, %26, %25, %24, %23, %18, %17, %16, %15, %14, %13, %12, %11, %7
  %121 = load void (i64)*, void (i64)** %2, align 8
  ret void (i64)* %121
}

declare void @op_nop(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_r_func(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = and i64 %3, 4095
  %5 = trunc i64 %4 to i32
  ret i32 %5
}

declare void @op_add(i64) #1

declare void @op_sub(i64) #1

declare void @op_mult(i64) #1

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

declare void @op_slli(i64) #1

declare void @op_srli(i64) #1

declare void @op_srai(i64) #1

declare void @op_lbi(i64) #1

declare void @op_sbi(i64) #1

declare void @op_lni(i64) #1

declare void @op_lbni(i64) #1

declare void @op_int(i64) #1

declare void @op_rit(i64) #1

declare void @op_time(i64) #1

declare void @op_timei(i64) #1

declare void @op_ring(i64) #1

declare void @op_ringi(i64) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i8 @wvm_r_rt(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 45
  %5 = and i64 %4, 127
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i8 @wvm_r_rs(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 38
  %5 = and i64 %4, 127
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i8 @wvm_r_rd(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 31
  %5 = and i64 %4, 127
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @wvm_r_regs(i64, i8*, i8*, i8*) #0 {
  %5 = alloca i64, align 8
  %6 = alloca i8*, align 8
  %7 = alloca i8*, align 8
  %8 = alloca i8*, align 8
  store i64 %0, i64* %5, align 8
  store i8* %1, i8** %6, align 8
  store i8* %2, i8** %7, align 8
  store i8* %3, i8** %8, align 8
  %9 = load i64, i64* %5, align 8
  %10 = call zeroext i8 @wvm_r_rs(i64 %9)
  %11 = load i8*, i8** %6, align 8
  store i8 %10, i8* %11, align 1
  %12 = load i64, i64* %5, align 8
  %13 = call zeroext i8 @wvm_r_rt(i64 %12)
  %14 = load i8*, i8** %7, align 8
  store i8 %13, i8* %14, align 1
  %15 = load i64, i64* %5, align 8
  %16 = call zeroext i8 @wvm_r_rd(i64 %15)
  %17 = load i8*, i8** %8, align 8
  store i8 %16, i8* %17, align 1
  ret void
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define signext i8 @wvm_r_flags(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 12
  %5 = and i64 %4, 63
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define signext i8 @wvm_r_cond(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 14
  %5 = and i64 %4, 15
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define signext i8 @wvm_i_flags(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 46
  %5 = and i64 %4, 63
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i8 @wvm_i_rs(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 39
  %5 = and i64 %4, 127
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i8 @wvm_i_rd(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 32
  %5 = and i64 %4, 127
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_i_imm(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = and i64 %3, 4294967295
  %5 = trunc i64 %4 to i32
  ret i32 %5
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define zeroext i8 @wvm_j_rs(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 45
  %5 = and i64 %4, 127
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define signext i8 @wvm_j_flags(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 32
  %5 = and i64 %4, 63
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wvm_j_addr(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = and i64 %3, 4294967295
  %5 = trunc i64 %4 to i32
  ret i32 %5
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define signext i8 @wvm_j_link(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 44
  %5 = and i64 %4, 1
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define signext i8 @wvm_j_cond(i64) #0 {
  %2 = alloca i64, align 8
  store i64 %0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  %4 = ashr i64 %3, 34
  %5 = and i64 %4, 15
  %6 = trunc i64 %5 to i8
  ret i8 %6
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.40.1)"}
