; ModuleID = 'wvm.c'
source_filename = "wvm.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }

@pc = common local_unnamed_addr global i64 0, align 8
@memsize = common local_unnamed_addr global i64 0, align 8
@alive = common local_unnamed_addr global i8 0, align 1
@membytes = common local_unnamed_addr global i64 0, align 8
@cycles = common local_unnamed_addr global i32 0, align 4
@memory = common local_unnamed_addr global i8* null, align 8
@.str = private unnamed_addr constant [2 x i8] c"r\00", align 1
@__stderrp = external local_unnamed_addr global %struct.__sFILE*, align 8
@.str.1 = private unnamed_addr constant [19 x i8] c"Couldn't open %s.\0A\00", align 1
@offset_symtab = common local_unnamed_addr global i64 0, align 8
@offset_handlers = common local_unnamed_addr global i64 0, align 8
@offset_code = common local_unnamed_addr global i64 0, align 8
@offset_data = common local_unnamed_addr global i64 0, align 8
@offset_end = common local_unnamed_addr global i64 0, align 8
@registers = common local_unnamed_addr global [128 x i64] zeroinitializer, align 16
@.str.2 = private unnamed_addr constant [73 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@.str.3 = private unnamed_addr constant [6 x i8] c"\1B[55G\00", align 1
@.str.4 = private unnamed_addr constant [81 x i8] c"        \E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\AC%s\E2\94\90\0A\00", align 1
@.str.5 = private unnamed_addr constant [43 x i8] c"        \E2\94\82    \1B[1mHexadecimal\1B[0m     \E2\94\82\00", align 1
@.str.6 = private unnamed_addr constant [36 x i8] c"     \1B[1mDecoded\1B[0m            \E2\94\82\00", align 1
@.str.7 = private unnamed_addr constant [98 x i8] c"\0A\E2\94\8C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC%s\E2\94\A4\0A\00", align 1
@.str.8 = private unnamed_addr constant [99 x i8] c"\E2\94\9C\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\BC%s\E2\94\A4%s\0A\00", align 1
@.str.9 = private unnamed_addr constant [5 x i8] c"\1B[0m\00", align 1
@.str.10 = private unnamed_addr constant [13 x i8] c"\E2\94\82\1B[38;5;8m\00", align 1
@.str.11 = private unnamed_addr constant [5 x i8] c"\1B[7m\00", align 1
@.str.12 = private unnamed_addr constant [61 x i8] c" %5lld \1B[0m\E2\94\82 \1B[38;5;7m0x\1B[0m\1B[1m%016llx\1B[0m \E2\94\82\1B[38;5;250m\00", align 1
@.str.13 = private unnamed_addr constant [10 x i8] c" %s%lld%s\00", align 1
@.str.14 = private unnamed_addr constant [6 x i8] c"\1B[35m\00", align 1
@.str.16 = private unnamed_addr constant [6 x i8] c"%s.%s\00", align 1
@.str.17 = private unnamed_addr constant [5 x i8] c"\1B[2m\00", align 1
@.str.19 = private unnamed_addr constant [16 x i8] c"\1B[48G%ssymtab%s\00", align 1
@.str.20 = private unnamed_addr constant [18 x i8] c"\1B[46G%shandlers%s\00", align 1
@.str.21 = private unnamed_addr constant [14 x i8] c"\1B[50G%scode%s\00", align 1
@.str.22 = private unnamed_addr constant [14 x i8] c"\1B[50G%sdata%s\00", align 1
@.str.23 = private unnamed_addr constant [13 x i8] c"\1B[51G%send%s\00", align 1
@.str.24 = private unnamed_addr constant [8 x i8] c"%s%s\E2\94\82\00", align 1
@.str.25 = private unnamed_addr constant [10 x i8] c" Metadata\00", align 1
@.str.26 = private unnamed_addr constant [14 x i8] c" Symbol Table\00", align 1
@.str.27 = private unnamed_addr constant [10 x i8] c" Handlers\00", align 1
@.str.28 = private unnamed_addr constant [6 x i8] c" Code\00", align 1
@.str.29 = private unnamed_addr constant [6 x i8] c" Data\00", align 1
@.str.30 = private unnamed_addr constant [4 x i8] c" %s\00", align 1
@.str.31 = private unnamed_addr constant [6 x i8] c"%s\E2\94\82\00", align 1
@.str.33 = private unnamed_addr constant [97 x i8] c"\E2\94\94\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\B4\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\B4%s\E2\94\98\0A\00", align 1

; Function Attrs: nounwind ssp uwtable
define zeroext i1 @wvm_init(i64) local_unnamed_addr #0 {
  store i64 0, i64* @pc, align 8, !tbaa !2
  store i64 0, i64* @memsize, align 8, !tbaa !6
  store i8 1, i8* @alive, align 1, !tbaa !8
  %2 = shl nsw i64 %0, 3
  store i64 %2, i64* @membytes, align 8, !tbaa !2
  store i32 0, i32* @cycles, align 4, !tbaa !10
  %3 = tail call i8* @calloc(i64 %2, i64 1) #8
  store i8* %3, i8** @memory, align 8, !tbaa !12
  %4 = icmp ne i8* %3, null
  ret i1 %4
}

; Function Attrs: nounwind allocsize(0,1)
declare noalias i8* @calloc(i64, i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @wvm_free() local_unnamed_addr #0 {
  %1 = load i8*, i8** @memory, align 8, !tbaa !12
  %2 = icmp eq i8* %1, null
  br i1 %2, label %4, label %3

; <label>:3:                                      ; preds = %0
  tail call void @free(i8* nonnull %1)
  br label %4

; <label>:4:                                      ; preds = %0, %3
  ret void
}

; Function Attrs: nounwind
declare void @free(i8* nocapture) local_unnamed_addr #2

; Function Attrs: nounwind ssp uwtable
define i32 @wvm_load(i8*) local_unnamed_addr #0 {
  %2 = alloca [20 x i8], align 16
  %3 = tail call %struct.__sFILE* @"\01_fopen"(i8* %0, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0)) #9
  %4 = icmp eq %struct.__sFILE* %3, null
  br i1 %4, label %5, label %8

; <label>:5:                                      ; preds = %1
  %6 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8, !tbaa !12
  %7 = tail call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %6, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.1, i64 0, i64 0), i8* %0)
  tail call void @exit(i32 1) #10
  unreachable

; <label>:8:                                      ; preds = %1
  store i64 0, i64* @memsize, align 8, !tbaa !6
  %9 = getelementptr inbounds [20 x i8], [20 x i8]* %2, i64 0, i64 0
  call void @llvm.lifetime.start(i64 20, i8* nonnull %9) #9
  %10 = call i8* @fgets(i8* nonnull %9, i32 18, %struct.__sFILE* nonnull %3)
  %11 = icmp eq i8* %10, null
  br i1 %11, label %59, label %12

; <label>:12:                                     ; preds = %8
  br label %13

; <label>:13:                                     ; preds = %12, %13
  %14 = call i64 @strtoul(i8* nocapture nonnull %9, i8** null, i32 16)
  %15 = load i64, i64* @memsize, align 8, !tbaa !6
  %16 = add i64 %15, 1
  store i64 %16, i64* @memsize, align 8, !tbaa !6
  %17 = shl i64 %15, 3
  %18 = lshr i64 %14, 56
  %19 = trunc i64 %18 to i8
  %20 = load i8*, i8** @memory, align 8, !tbaa !12
  %21 = getelementptr inbounds i8, i8* %20, i64 %17
  store i8 %19, i8* %21, align 1, !tbaa !14
  %22 = lshr i64 %14, 48
  %23 = trunc i64 %22 to i8
  %24 = load i8*, i8** @memory, align 8, !tbaa !12
  %25 = or i64 %17, 1
  %26 = getelementptr inbounds i8, i8* %24, i64 %25
  store i8 %23, i8* %26, align 1, !tbaa !14
  %27 = lshr i64 %14, 40
  %28 = trunc i64 %27 to i8
  %29 = load i8*, i8** @memory, align 8, !tbaa !12
  %30 = or i64 %17, 2
  %31 = getelementptr inbounds i8, i8* %29, i64 %30
  store i8 %28, i8* %31, align 1, !tbaa !14
  %32 = lshr i64 %14, 32
  %33 = trunc i64 %32 to i8
  %34 = load i8*, i8** @memory, align 8, !tbaa !12
  %35 = or i64 %17, 3
  %36 = getelementptr inbounds i8, i8* %34, i64 %35
  store i8 %33, i8* %36, align 1, !tbaa !14
  %37 = lshr i64 %14, 24
  %38 = trunc i64 %37 to i8
  %39 = load i8*, i8** @memory, align 8, !tbaa !12
  %40 = or i64 %17, 4
  %41 = getelementptr inbounds i8, i8* %39, i64 %40
  store i8 %38, i8* %41, align 1, !tbaa !14
  %42 = lshr i64 %14, 16
  %43 = trunc i64 %42 to i8
  %44 = load i8*, i8** @memory, align 8, !tbaa !12
  %45 = or i64 %17, 5
  %46 = getelementptr inbounds i8, i8* %44, i64 %45
  store i8 %43, i8* %46, align 1, !tbaa !14
  %47 = lshr i64 %14, 8
  %48 = trunc i64 %47 to i8
  %49 = load i8*, i8** @memory, align 8, !tbaa !12
  %50 = or i64 %17, 6
  %51 = getelementptr inbounds i8, i8* %49, i64 %50
  store i8 %48, i8* %51, align 1, !tbaa !14
  %52 = trunc i64 %14 to i8
  %53 = load i8*, i8** @memory, align 8, !tbaa !12
  %54 = or i64 %17, 7
  %55 = getelementptr inbounds i8, i8* %53, i64 %54
  store i8 %52, i8* %55, align 1, !tbaa !14
  %56 = call i8* @fgets(i8* nonnull %9, i32 18, %struct.__sFILE* nonnull %3)
  %57 = icmp eq i8* %56, null
  br i1 %57, label %58, label %13

; <label>:58:                                     ; preds = %13
  br label %59

; <label>:59:                                     ; preds = %58, %8
  %60 = call i32 @fclose(%struct.__sFILE* nonnull %3)
  %61 = load i8*, i8** @memory, align 8, !tbaa !12
  %62 = load i8, i8* %61, align 1, !tbaa !14
  %63 = zext i8 %62 to i64
  %64 = shl nuw i64 %63, 56
  %65 = getelementptr inbounds i8, i8* %61, i64 1
  %66 = load i8, i8* %65, align 1, !tbaa !14
  %67 = zext i8 %66 to i64
  %68 = shl nuw nsw i64 %67, 48
  %69 = or i64 %68, %64
  %70 = getelementptr inbounds i8, i8* %61, i64 2
  %71 = load i8, i8* %70, align 1, !tbaa !14
  %72 = zext i8 %71 to i64
  %73 = shl nuw nsw i64 %72, 40
  %74 = or i64 %69, %73
  %75 = getelementptr inbounds i8, i8* %61, i64 3
  %76 = load i8, i8* %75, align 1, !tbaa !14
  %77 = zext i8 %76 to i64
  %78 = shl nuw nsw i64 %77, 32
  %79 = or i64 %74, %78
  %80 = getelementptr inbounds i8, i8* %61, i64 4
  %81 = load i8, i8* %80, align 1, !tbaa !14
  %82 = zext i8 %81 to i64
  %83 = shl nuw nsw i64 %82, 24
  %84 = or i64 %79, %83
  %85 = getelementptr inbounds i8, i8* %61, i64 5
  %86 = load i8, i8* %85, align 1, !tbaa !14
  %87 = zext i8 %86 to i64
  %88 = shl nuw nsw i64 %87, 16
  %89 = or i64 %84, %88
  %90 = getelementptr inbounds i8, i8* %61, i64 6
  %91 = load i8, i8* %90, align 1, !tbaa !14
  %92 = zext i8 %91 to i64
  %93 = shl nuw nsw i64 %92, 8
  %94 = or i64 %89, %93
  %95 = getelementptr inbounds i8, i8* %61, i64 7
  %96 = load i8, i8* %95, align 1, !tbaa !14
  %97 = zext i8 %96 to i64
  %98 = or i64 %94, %97
  store i64 %98, i64* @offset_symtab, align 8, !tbaa !2
  %99 = getelementptr inbounds i8, i8* %61, i64 8
  %100 = load i8, i8* %99, align 1, !tbaa !14
  %101 = zext i8 %100 to i64
  %102 = shl nuw i64 %101, 56
  %103 = getelementptr inbounds i8, i8* %61, i64 9
  %104 = load i8, i8* %103, align 1, !tbaa !14
  %105 = zext i8 %104 to i64
  %106 = shl nuw nsw i64 %105, 48
  %107 = or i64 %106, %102
  %108 = getelementptr inbounds i8, i8* %61, i64 10
  %109 = load i8, i8* %108, align 1, !tbaa !14
  %110 = zext i8 %109 to i64
  %111 = shl nuw nsw i64 %110, 40
  %112 = or i64 %107, %111
  %113 = getelementptr inbounds i8, i8* %61, i64 11
  %114 = load i8, i8* %113, align 1, !tbaa !14
  %115 = zext i8 %114 to i64
  %116 = shl nuw nsw i64 %115, 32
  %117 = or i64 %112, %116
  %118 = getelementptr inbounds i8, i8* %61, i64 12
  %119 = load i8, i8* %118, align 1, !tbaa !14
  %120 = zext i8 %119 to i64
  %121 = shl nuw nsw i64 %120, 24
  %122 = or i64 %117, %121
  %123 = getelementptr inbounds i8, i8* %61, i64 13
  %124 = load i8, i8* %123, align 1, !tbaa !14
  %125 = zext i8 %124 to i64
  %126 = shl nuw nsw i64 %125, 16
  %127 = or i64 %122, %126
  %128 = getelementptr inbounds i8, i8* %61, i64 14
  %129 = load i8, i8* %128, align 1, !tbaa !14
  %130 = zext i8 %129 to i64
  %131 = shl nuw nsw i64 %130, 8
  %132 = or i64 %127, %131
  %133 = getelementptr inbounds i8, i8* %61, i64 15
  %134 = load i8, i8* %133, align 1, !tbaa !14
  %135 = zext i8 %134 to i64
  %136 = or i64 %132, %135
  store i64 %136, i64* @offset_handlers, align 8, !tbaa !2
  %137 = getelementptr inbounds i8, i8* %61, i64 16
  %138 = load i8, i8* %137, align 1, !tbaa !14
  %139 = zext i8 %138 to i64
  %140 = shl nuw i64 %139, 56
  %141 = getelementptr inbounds i8, i8* %61, i64 17
  %142 = load i8, i8* %141, align 1, !tbaa !14
  %143 = zext i8 %142 to i64
  %144 = shl nuw nsw i64 %143, 48
  %145 = or i64 %144, %140
  %146 = getelementptr inbounds i8, i8* %61, i64 18
  %147 = load i8, i8* %146, align 1, !tbaa !14
  %148 = zext i8 %147 to i64
  %149 = shl nuw nsw i64 %148, 40
  %150 = or i64 %145, %149
  %151 = getelementptr inbounds i8, i8* %61, i64 19
  %152 = load i8, i8* %151, align 1, !tbaa !14
  %153 = zext i8 %152 to i64
  %154 = shl nuw nsw i64 %153, 32
  %155 = or i64 %150, %154
  %156 = getelementptr inbounds i8, i8* %61, i64 20
  %157 = load i8, i8* %156, align 1, !tbaa !14
  %158 = zext i8 %157 to i64
  %159 = shl nuw nsw i64 %158, 24
  %160 = or i64 %155, %159
  %161 = getelementptr inbounds i8, i8* %61, i64 21
  %162 = load i8, i8* %161, align 1, !tbaa !14
  %163 = zext i8 %162 to i64
  %164 = shl nuw nsw i64 %163, 16
  %165 = or i64 %160, %164
  %166 = getelementptr inbounds i8, i8* %61, i64 22
  %167 = load i8, i8* %166, align 1, !tbaa !14
  %168 = zext i8 %167 to i64
  %169 = shl nuw nsw i64 %168, 8
  %170 = or i64 %165, %169
  %171 = getelementptr inbounds i8, i8* %61, i64 23
  %172 = load i8, i8* %171, align 1, !tbaa !14
  %173 = zext i8 %172 to i64
  %174 = or i64 %170, %173
  store i64 %174, i64* @offset_code, align 8, !tbaa !2
  %175 = load i8*, i8** @memory, align 8, !tbaa !12
  %176 = getelementptr inbounds i8, i8* %175, i64 24
  %177 = load i8, i8* %176, align 1, !tbaa !14
  %178 = zext i8 %177 to i64
  %179 = shl nuw i64 %178, 56
  %180 = getelementptr inbounds i8, i8* %175, i64 25
  %181 = load i8, i8* %180, align 1, !tbaa !14
  %182 = zext i8 %181 to i64
  %183 = shl nuw nsw i64 %182, 48
  %184 = or i64 %183, %179
  %185 = getelementptr inbounds i8, i8* %175, i64 26
  %186 = load i8, i8* %185, align 1, !tbaa !14
  %187 = zext i8 %186 to i64
  %188 = shl nuw nsw i64 %187, 40
  %189 = or i64 %184, %188
  %190 = getelementptr inbounds i8, i8* %175, i64 27
  %191 = load i8, i8* %190, align 1, !tbaa !14
  %192 = zext i8 %191 to i64
  %193 = shl nuw nsw i64 %192, 32
  %194 = or i64 %189, %193
  %195 = getelementptr inbounds i8, i8* %175, i64 28
  %196 = load i8, i8* %195, align 1, !tbaa !14
  %197 = zext i8 %196 to i64
  %198 = shl nuw nsw i64 %197, 24
  %199 = or i64 %194, %198
  %200 = getelementptr inbounds i8, i8* %175, i64 29
  %201 = load i8, i8* %200, align 1, !tbaa !14
  %202 = zext i8 %201 to i64
  %203 = shl nuw nsw i64 %202, 16
  %204 = or i64 %199, %203
  %205 = getelementptr inbounds i8, i8* %175, i64 30
  %206 = load i8, i8* %205, align 1, !tbaa !14
  %207 = zext i8 %206 to i64
  %208 = shl nuw nsw i64 %207, 8
  %209 = or i64 %204, %208
  %210 = getelementptr inbounds i8, i8* %175, i64 31
  %211 = load i8, i8* %210, align 1, !tbaa !14
  %212 = zext i8 %211 to i64
  %213 = or i64 %209, %212
  store i64 %213, i64* @offset_data, align 8, !tbaa !2
  %214 = getelementptr inbounds i8, i8* %175, i64 32
  %215 = load i8, i8* %214, align 1, !tbaa !14
  %216 = zext i8 %215 to i64
  %217 = shl nuw i64 %216, 56
  %218 = getelementptr inbounds i8, i8* %175, i64 33
  %219 = load i8, i8* %218, align 1, !tbaa !14
  %220 = zext i8 %219 to i64
  %221 = shl nuw nsw i64 %220, 48
  %222 = or i64 %221, %217
  %223 = getelementptr inbounds i8, i8* %175, i64 34
  %224 = load i8, i8* %223, align 1, !tbaa !14
  %225 = zext i8 %224 to i64
  %226 = shl nuw nsw i64 %225, 40
  %227 = or i64 %222, %226
  %228 = getelementptr inbounds i8, i8* %175, i64 35
  %229 = load i8, i8* %228, align 1, !tbaa !14
  %230 = zext i8 %229 to i64
  %231 = shl nuw nsw i64 %230, 32
  %232 = or i64 %227, %231
  %233 = getelementptr inbounds i8, i8* %175, i64 36
  %234 = load i8, i8* %233, align 1, !tbaa !14
  %235 = zext i8 %234 to i64
  %236 = shl nuw nsw i64 %235, 24
  %237 = or i64 %232, %236
  %238 = getelementptr inbounds i8, i8* %175, i64 37
  %239 = load i8, i8* %238, align 1, !tbaa !14
  %240 = zext i8 %239 to i64
  %241 = shl nuw nsw i64 %240, 16
  %242 = or i64 %237, %241
  %243 = getelementptr inbounds i8, i8* %175, i64 38
  %244 = load i8, i8* %243, align 1, !tbaa !14
  %245 = zext i8 %244 to i64
  %246 = shl nuw nsw i64 %245, 8
  %247 = or i64 %242, %246
  %248 = getelementptr inbounds i8, i8* %175, i64 39
  %249 = load i8, i8* %248, align 1, !tbaa !14
  %250 = zext i8 %249 to i64
  %251 = or i64 %247, %250
  store i64 %251, i64* @offset_end, align 8, !tbaa !2
  %252 = load i64, i64* @memsize, align 8, !tbaa !6
  %253 = trunc i64 %252 to i32
  call void @llvm.lifetime.end(i64 20, i8* nonnull %9) #9
  ret i32 %253
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #3

declare %struct.__sFILE* @"\01_fopen"(i8*, i8*) local_unnamed_addr #4

; Function Attrs: nounwind
declare i32 @fprintf(%struct.__sFILE* nocapture, i8* nocapture readonly, ...) local_unnamed_addr #2

; Function Attrs: noreturn
declare void @exit(i32) local_unnamed_addr #5

; Function Attrs: nounwind
declare i8* @fgets(i8*, i32, %struct.__sFILE* nocapture) local_unnamed_addr #2

; Function Attrs: nounwind
declare i64 @strtoul(i8* readonly, i8** nocapture, i32) local_unnamed_addr #2

; Function Attrs: norecurse nounwind ssp uwtable
define void @wvm_set_word(i64, i64) local_unnamed_addr #6 {
  %3 = lshr i64 %1, 56
  %4 = trunc i64 %3 to i8
  %5 = load i8*, i8** @memory, align 8, !tbaa !12
  %6 = getelementptr inbounds i8, i8* %5, i64 %0
  store i8 %4, i8* %6, align 1, !tbaa !14
  %7 = lshr i64 %1, 48
  %8 = trunc i64 %7 to i8
  %9 = load i8*, i8** @memory, align 8, !tbaa !12
  %10 = add nsw i64 %0, 1
  %11 = getelementptr inbounds i8, i8* %9, i64 %10
  store i8 %8, i8* %11, align 1, !tbaa !14
  %12 = lshr i64 %1, 40
  %13 = trunc i64 %12 to i8
  %14 = load i8*, i8** @memory, align 8, !tbaa !12
  %15 = add nsw i64 %0, 2
  %16 = getelementptr inbounds i8, i8* %14, i64 %15
  store i8 %13, i8* %16, align 1, !tbaa !14
  %17 = lshr i64 %1, 32
  %18 = trunc i64 %17 to i8
  %19 = load i8*, i8** @memory, align 8, !tbaa !12
  %20 = add nsw i64 %0, 3
  %21 = getelementptr inbounds i8, i8* %19, i64 %20
  store i8 %18, i8* %21, align 1, !tbaa !14
  %22 = lshr i64 %1, 24
  %23 = trunc i64 %22 to i8
  %24 = load i8*, i8** @memory, align 8, !tbaa !12
  %25 = add nsw i64 %0, 4
  %26 = getelementptr inbounds i8, i8* %24, i64 %25
  store i8 %23, i8* %26, align 1, !tbaa !14
  %27 = lshr i64 %1, 16
  %28 = trunc i64 %27 to i8
  %29 = load i8*, i8** @memory, align 8, !tbaa !12
  %30 = add nsw i64 %0, 5
  %31 = getelementptr inbounds i8, i8* %29, i64 %30
  store i8 %28, i8* %31, align 1, !tbaa !14
  %32 = lshr i64 %1, 8
  %33 = trunc i64 %32 to i8
  %34 = load i8*, i8** @memory, align 8, !tbaa !12
  %35 = add nsw i64 %0, 6
  %36 = getelementptr inbounds i8, i8* %34, i64 %35
  store i8 %33, i8* %36, align 1, !tbaa !14
  %37 = trunc i64 %1 to i8
  %38 = load i8*, i8** @memory, align 8, !tbaa !12
  %39 = add nsw i64 %0, 7
  %40 = getelementptr inbounds i8, i8* %38, i64 %39
  store i8 %37, i8* %40, align 1, !tbaa !14
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #3

; Function Attrs: nounwind
declare i32 @fclose(%struct.__sFILE* nocapture) local_unnamed_addr #2

; Function Attrs: norecurse nounwind readonly ssp uwtable
define i64 @wvm_get_word(i64) local_unnamed_addr #7 {
  %2 = load i8*, i8** @memory, align 8, !tbaa !12
  %3 = getelementptr inbounds i8, i8* %2, i64 %0
  %4 = load i8, i8* %3, align 1, !tbaa !14
  %5 = zext i8 %4 to i64
  %6 = shl nuw i64 %5, 56
  %7 = add nsw i64 %0, 1
  %8 = getelementptr inbounds i8, i8* %2, i64 %7
  %9 = load i8, i8* %8, align 1, !tbaa !14
  %10 = zext i8 %9 to i64
  %11 = shl nuw nsw i64 %10, 48
  %12 = or i64 %11, %6
  %13 = add nsw i64 %0, 2
  %14 = getelementptr inbounds i8, i8* %2, i64 %13
  %15 = load i8, i8* %14, align 1, !tbaa !14
  %16 = zext i8 %15 to i64
  %17 = shl nuw nsw i64 %16, 40
  %18 = or i64 %17, %12
  %19 = add nsw i64 %0, 3
  %20 = getelementptr inbounds i8, i8* %2, i64 %19
  %21 = load i8, i8* %20, align 1, !tbaa !14
  %22 = zext i8 %21 to i64
  %23 = shl nuw nsw i64 %22, 32
  %24 = or i64 %23, %18
  %25 = add nsw i64 %0, 4
  %26 = getelementptr inbounds i8, i8* %2, i64 %25
  %27 = load i8, i8* %26, align 1, !tbaa !14
  %28 = zext i8 %27 to i64
  %29 = shl nuw nsw i64 %28, 24
  %30 = or i64 %29, %24
  %31 = add nsw i64 %0, 5
  %32 = getelementptr inbounds i8, i8* %2, i64 %31
  %33 = load i8, i8* %32, align 1, !tbaa !14
  %34 = zext i8 %33 to i64
  %35 = shl nuw nsw i64 %34, 16
  %36 = or i64 %35, %30
  %37 = add nsw i64 %0, 6
  %38 = getelementptr inbounds i8, i8* %2, i64 %37
  %39 = load i8, i8* %38, align 1, !tbaa !14
  %40 = zext i8 %39 to i64
  %41 = shl nuw nsw i64 %40, 8
  %42 = or i64 %41, %36
  %43 = add nsw i64 %0, 7
  %44 = getelementptr inbounds i8, i8* %2, i64 %43
  %45 = load i8, i8* %44, align 1, !tbaa !14
  %46 = zext i8 %45 to i64
  %47 = or i64 %46, %42
  ret i64 %47
}

; Function Attrs: norecurse nounwind ssp uwtable
define void @wvm_init_vm() local_unnamed_addr #6 {
  %1 = load i64, i64* @offset_code, align 8, !tbaa !2
  store i64 %1, i64* @pc, align 8, !tbaa !2
  %2 = load i64, i64* @membytes, align 8, !tbaa !2
  %3 = add nsw i64 %2, -8
  store i64 %3, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 2), align 16, !tbaa !2
  ret void
}

; Function Attrs: norecurse nounwind readonly ssp uwtable
define zeroext i8 @wvm_get_byte(i64) local_unnamed_addr #7 {
  %2 = load i8*, i8** @memory, align 8, !tbaa !12
  %3 = getelementptr inbounds i8, i8* %2, i64 %0
  %4 = load i8, i8* %3, align 1, !tbaa !14
  ret i8 %4
}

; Function Attrs: norecurse nounwind ssp uwtable
define void @wvm_set_byte(i64, i8 zeroext) local_unnamed_addr #6 {
  %3 = load i8*, i8** @memory, align 8, !tbaa !12
  %4 = getelementptr inbounds i8, i8* %3, i64 %0
  store i8 %1, i8* %4, align 1, !tbaa !14
  ret void
}

; Function Attrs: nounwind ssp uwtable
define noalias i8* @wvm_get_string(i64) local_unnamed_addr #0 {
  %2 = load i8*, i8** @memory, align 8, !tbaa !12
  br label %3

; <label>:3:                                      ; preds = %3, %1
  %4 = phi i64 [ %9, %3 ], [ 0, %1 ]
  %5 = add nsw i64 %4, %0
  %6 = getelementptr inbounds i8, i8* %2, i64 %5
  %7 = load i8, i8* %6, align 1, !tbaa !14
  %8 = icmp eq i8 %7, 0
  %9 = add nuw i64 %4, 1
  br i1 %8, label %10, label %3

; <label>:10:                                     ; preds = %3
  %11 = trunc i64 %4 to i32
  %12 = shl i64 %9, 32
  %13 = ashr exact i64 %12, 32
  %14 = tail call i8* @calloc(i64 %13, i64 1) #8
  %15 = icmp sgt i32 %11, 0
  br i1 %15, label %16, label %21

; <label>:16:                                     ; preds = %10
  %17 = getelementptr i8, i8* %2, i64 %0
  %18 = add i64 %4, 4294967295
  %19 = and i64 %18, 4294967295
  %20 = add nuw nsw i64 %19, 1
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %14, i8* %17, i64 %20, i32 1, i1 false)
  br label %21

; <label>:21:                                     ; preds = %16, %10
  %22 = getelementptr inbounds i8, i8* %14, i64 %4
  store i8 0, i8* %22, align 1, !tbaa !14
  ret i8* %14
}

; Function Attrs: norecurse nounwind ssp uwtable
define void @wvm_jump(i64) local_unnamed_addr #6 {
  store i64 %0, i64* @pc, align 8, !tbaa !2
  ret void
}

; Function Attrs: norecurse nounwind ssp uwtable
define void @wvm_link() local_unnamed_addr #6 {
  %1 = load i64, i64* @pc, align 8, !tbaa !2
  %2 = add nsw i64 %1, 8
  store i64 %2, i64* getelementptr inbounds ([128 x i64], [128 x i64]* @registers, i64 0, i64 4), align 16, !tbaa !2
  ret void
}

; Function Attrs: norecurse nounwind ssp uwtable
define void @wvm_increment_pc() local_unnamed_addr #6 {
  %1 = load i64, i64* @pc, align 8, !tbaa !2
  %2 = add nsw i64 %1, 8
  store i64 %2, i64* @pc, align 8, !tbaa !2
  ret void
}

; Function Attrs: nounwind ssp uwtable
define zeroext i1 @wvm_tick() local_unnamed_addr #0 {
  %1 = load i64, i64* @pc, align 8, !tbaa !2
  %2 = load i8*, i8** @memory, align 8, !tbaa !12
  %3 = getelementptr inbounds i8, i8* %2, i64 %1
  %4 = load i8, i8* %3, align 1, !tbaa !14
  %5 = zext i8 %4 to i64
  %6 = shl nuw i64 %5, 56
  %7 = add nsw i64 %1, 1
  %8 = getelementptr inbounds i8, i8* %2, i64 %7
  %9 = load i8, i8* %8, align 1, !tbaa !14
  %10 = zext i8 %9 to i64
  %11 = shl nuw nsw i64 %10, 48
  %12 = or i64 %11, %6
  %13 = add nsw i64 %1, 2
  %14 = getelementptr inbounds i8, i8* %2, i64 %13
  %15 = load i8, i8* %14, align 1, !tbaa !14
  %16 = zext i8 %15 to i64
  %17 = shl nuw nsw i64 %16, 40
  %18 = or i64 %12, %17
  %19 = add nsw i64 %1, 3
  %20 = getelementptr inbounds i8, i8* %2, i64 %19
  %21 = load i8, i8* %20, align 1, !tbaa !14
  %22 = zext i8 %21 to i64
  %23 = shl nuw nsw i64 %22, 32
  %24 = or i64 %18, %23
  %25 = add nsw i64 %1, 4
  %26 = getelementptr inbounds i8, i8* %2, i64 %25
  %27 = load i8, i8* %26, align 1, !tbaa !14
  %28 = zext i8 %27 to i64
  %29 = shl nuw nsw i64 %28, 24
  %30 = or i64 %24, %29
  %31 = add nsw i64 %1, 5
  %32 = getelementptr inbounds i8, i8* %2, i64 %31
  %33 = load i8, i8* %32, align 1, !tbaa !14
  %34 = zext i8 %33 to i64
  %35 = shl nuw nsw i64 %34, 16
  %36 = or i64 %30, %35
  %37 = add nsw i64 %1, 6
  %38 = getelementptr inbounds i8, i8* %2, i64 %37
  %39 = load i8, i8* %38, align 1, !tbaa !14
  %40 = zext i8 %39 to i64
  %41 = shl nuw nsw i64 %40, 8
  %42 = or i64 %36, %41
  %43 = add nsw i64 %1, 7
  %44 = getelementptr inbounds i8, i8* %2, i64 %43
  %45 = load i8, i8* %44, align 1, !tbaa !14
  %46 = zext i8 %45 to i64
  %47 = or i64 %42, %46
  %48 = tail call void (i64)* @wvm_get_fn(i64 %47) #9
  tail call void %48(i64 %47) #9
  %49 = load i32, i32* @cycles, align 4, !tbaa !10
  %50 = add nsw i32 %49, 1
  store i32 %50, i32* @cycles, align 4, !tbaa !10
  %51 = load i8, i8* @alive, align 1, !tbaa !8, !range !15
  %52 = icmp ne i8 %51, 0
  ret i1 %52
}

declare void (i64)* @wvm_get_fn(i64) local_unnamed_addr #4

; Function Attrs: nounwind ssp uwtable
define void @wvm_print_memory() local_unnamed_addr #0 {
  %1 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([81 x i8], [81 x i8]* @.str.4, i64 0, i64 0), i8* getelementptr inbounds ([73 x i8], [73 x i8]* @.str.2, i64 0, i64 0))
  %2 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([43 x i8], [43 x i8]* @.str.5, i64 0, i64 0))
  %3 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @.str.6, i64 0, i64 0))
  %4 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([98 x i8], [98 x i8]* @.str.7, i64 0, i64 0), i8* getelementptr inbounds ([73 x i8], [73 x i8]* @.str.2, i64 0, i64 0))
  %5 = load i64, i64* @memsize, align 8, !tbaa !6
  %6 = icmp eq i64 %5, 0
  br i1 %6, label %9, label %7

; <label>:7:                                      ; preds = %0
  br label %11

; <label>:8:                                      ; preds = %157
  br label %9

; <label>:9:                                      ; preds = %8, %0
  %10 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([97 x i8], [97 x i8]* @.str.33, i64 0, i64 0), i8* getelementptr inbounds ([73 x i8], [73 x i8]* @.str.2, i64 0, i64 0))
  ret void

; <label>:11:                                     ; preds = %7, %157
  %12 = phi i64 [ %159, %157 ], [ 0, %7 ]
  %13 = trunc i64 %12 to i32
  %14 = shl i32 %13, 3
  %15 = sext i32 %14 to i64
  %16 = load i8*, i8** @memory, align 8, !tbaa !12
  %17 = getelementptr inbounds i8, i8* %16, i64 %15
  %18 = load i8, i8* %17, align 1, !tbaa !14
  %19 = zext i8 %18 to i64
  %20 = shl nuw i64 %19, 56
  %21 = or i64 %15, 1
  %22 = getelementptr inbounds i8, i8* %16, i64 %21
  %23 = load i8, i8* %22, align 1, !tbaa !14
  %24 = zext i8 %23 to i64
  %25 = shl nuw nsw i64 %24, 48
  %26 = or i64 %25, %20
  %27 = or i64 %15, 2
  %28 = getelementptr inbounds i8, i8* %16, i64 %27
  %29 = load i8, i8* %28, align 1, !tbaa !14
  %30 = zext i8 %29 to i64
  %31 = shl nuw nsw i64 %30, 40
  %32 = or i64 %26, %31
  %33 = or i64 %15, 3
  %34 = getelementptr inbounds i8, i8* %16, i64 %33
  %35 = load i8, i8* %34, align 1, !tbaa !14
  %36 = zext i8 %35 to i64
  %37 = shl nuw nsw i64 %36, 32
  %38 = or i64 %32, %37
  %39 = or i64 %15, 4
  %40 = getelementptr inbounds i8, i8* %16, i64 %39
  %41 = load i8, i8* %40, align 1, !tbaa !14
  %42 = zext i8 %41 to i64
  %43 = shl nuw nsw i64 %42, 24
  %44 = or i64 %38, %43
  %45 = or i64 %15, 5
  %46 = getelementptr inbounds i8, i8* %16, i64 %45
  %47 = load i8, i8* %46, align 1, !tbaa !14
  %48 = zext i8 %47 to i64
  %49 = shl nuw nsw i64 %48, 16
  %50 = or i64 %44, %49
  %51 = or i64 %15, 6
  %52 = getelementptr inbounds i8, i8* %16, i64 %51
  %53 = load i8, i8* %52, align 1, !tbaa !14
  %54 = zext i8 %53 to i64
  %55 = shl nuw nsw i64 %54, 8
  %56 = or i64 %50, %55
  %57 = or i64 %15, 7
  %58 = getelementptr inbounds i8, i8* %16, i64 %57
  %59 = load i8, i8* %58, align 1, !tbaa !14
  %60 = zext i8 %59 to i64
  %61 = or i64 %56, %60
  %62 = load i64, i64* @offset_handlers, align 8, !tbaa !2
  %63 = icmp eq i64 %15, %62
  %64 = load i64, i64* @offset_data, align 8
  %65 = icmp eq i64 %15, %64
  %66 = or i1 %63, %65
  %67 = load i64, i64* @offset_code, align 8
  %68 = icmp eq i64 %15, %67
  %69 = or i1 %66, %68
  %70 = load i64, i64* @offset_symtab, align 8
  %71 = icmp eq i64 %15, %70
  %72 = or i1 %69, %71
  br i1 %72, label %73, label %75

; <label>:73:                                     ; preds = %11
  %74 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([99 x i8], [99 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([73 x i8], [73 x i8]* @.str.2, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %75

; <label>:75:                                     ; preds = %11, %73
  %76 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.10, i64 0, i64 0))
  %77 = load i64, i64* @pc, align 8, !tbaa !2
  %78 = icmp eq i64 %15, %77
  br i1 %78, label %79, label %81

; <label>:79:                                     ; preds = %75
  %80 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.11, i64 0, i64 0))
  br label %81

; <label>:81:                                     ; preds = %79, %75
  %82 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([61 x i8], [61 x i8]* @.str.12, i64 0, i64 0), i64 %15, i64 %61)
  %83 = icmp slt i64 %12, 5
  br i1 %83, label %84, label %86

; <label>:84:                                     ; preds = %81
  %85 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.13, i64 0, i64 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.14, i64 0, i64 0), i64 %61, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %104

; <label>:86:                                     ; preds = %81
  %87 = load i64, i64* @offset_handlers, align 8, !tbaa !2
  %88 = icmp sge i64 %15, %87
  %89 = load i64, i64* @offset_data, align 8
  %90 = icmp sgt i64 %89, %15
  %91 = and i1 %88, %90
  br i1 %91, label %104, label %92

; <label>:92:                                     ; preds = %86
  %93 = tail call i32 @putchar(i32 32) #9
  %94 = icmp slt i8 %18, 32
  br i1 %94, label %95, label %97

; <label>:95:                                     ; preds = %92
  %96 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %100

; <label>:97:                                     ; preds = %92
  %98 = zext i8 %18 to i32
  %99 = tail call i32 @putchar(i32 %98) #9
  br label %100

; <label>:100:                                    ; preds = %97, %95
  %101 = ashr exact i64 %26, 48
  %102 = trunc i64 %101 to i8
  %103 = icmp slt i8 %102, 32
  br i1 %103, label %166, label %162

; <label>:104:                                    ; preds = %221, %225, %86, %84
  switch i32 %13, label %115 [
    i32 0, label %105
    i32 1, label %107
    i32 2, label %109
    i32 3, label %111
    i32 4, label %113
  ]

; <label>:105:                                    ; preds = %104
  %106 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.19, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %115

; <label>:107:                                    ; preds = %104
  %108 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str.20, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %115

; <label>:109:                                    ; preds = %104
  %110 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.21, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %115

; <label>:111:                                    ; preds = %104
  %112 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.22, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %115

; <label>:113:                                    ; preds = %104
  %114 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.23, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %115

; <label>:115:                                    ; preds = %104, %107, %111, %113, %109, %105
  %116 = load i64, i64* @offset_code, align 8, !tbaa !2
  %117 = icmp sge i64 %15, %116
  %118 = load i64, i64* @offset_data, align 8
  %119 = icmp sgt i64 %118, %15
  %120 = and i1 %117, %119
  br i1 %120, label %123, label %121

; <label>:121:                                    ; preds = %115
  %122 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.24, i64 0, i64 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.3, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %123

; <label>:123:                                    ; preds = %115, %121
  %124 = icmp eq i32 %14, 0
  br i1 %124, label %125, label %127

; <label>:125:                                    ; preds = %123
  %126 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.25, i64 0, i64 0))
  br label %147

; <label>:127:                                    ; preds = %123
  %128 = load i64, i64* @offset_symtab, align 8, !tbaa !2
  %129 = icmp eq i64 %15, %128
  br i1 %129, label %130, label %132

; <label>:130:                                    ; preds = %127
  %131 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.26, i64 0, i64 0))
  br label %147

; <label>:132:                                    ; preds = %127
  %133 = load i64, i64* @offset_handlers, align 8, !tbaa !2
  %134 = icmp eq i64 %15, %133
  br i1 %134, label %135, label %137

; <label>:135:                                    ; preds = %132
  %136 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.27, i64 0, i64 0))
  br label %147

; <label>:137:                                    ; preds = %132
  %138 = load i64, i64* @offset_code, align 8, !tbaa !2
  %139 = icmp eq i64 %15, %138
  br i1 %139, label %140, label %142

; <label>:140:                                    ; preds = %137
  %141 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.28, i64 0, i64 0))
  br label %147

; <label>:142:                                    ; preds = %137
  %143 = load i64, i64* @offset_data, align 8, !tbaa !2
  %144 = icmp eq i64 %15, %143
  br i1 %144, label %145, label %147

; <label>:145:                                    ; preds = %142
  %146 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.29, i64 0, i64 0))
  br label %147

; <label>:147:                                    ; preds = %130, %140, %145, %142, %135, %125
  %148 = load i64, i64* @offset_code, align 8, !tbaa !2
  %149 = icmp sle i64 %148, %15
  %150 = load i64, i64* @offset_data, align 8
  %151 = icmp slt i64 %15, %150
  %152 = and i1 %149, %151
  br i1 %152, label %153, label %157

; <label>:153:                                    ; preds = %147
  %154 = tail call i8* @wvm_disassemble(i64 %61) #9
  %155 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.30, i64 0, i64 0), i8* %154)
  tail call void @free(i8* %154)
  %156 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.31, i64 0, i64 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.3, i64 0, i64 0))
  br label %157

; <label>:157:                                    ; preds = %147, %153
  %158 = tail call i32 @putchar(i32 10) #9
  %159 = add nuw i64 %12, 1
  %160 = load i64, i64* @memsize, align 8, !tbaa !6
  %161 = icmp ult i64 %159, %160
  br i1 %161, label %11, label %8

; <label>:162:                                    ; preds = %100
  %163 = trunc i64 %101 to i32
  %164 = and i32 %163, 255
  %165 = tail call i32 @putchar(i32 %164) #9
  br label %168

; <label>:166:                                    ; preds = %100
  %167 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %168

; <label>:168:                                    ; preds = %166, %162
  %169 = ashr exact i64 %32, 40
  %170 = trunc i64 %169 to i8
  %171 = icmp slt i8 %170, 32
  br i1 %171, label %176, label %172

; <label>:172:                                    ; preds = %168
  %173 = trunc i64 %169 to i32
  %174 = and i32 %173, 255
  %175 = tail call i32 @putchar(i32 %174) #9
  br label %178

; <label>:176:                                    ; preds = %168
  %177 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %178

; <label>:178:                                    ; preds = %176, %172
  %179 = ashr exact i64 %38, 32
  %180 = trunc i64 %179 to i8
  %181 = icmp slt i8 %180, 32
  br i1 %181, label %186, label %182

; <label>:182:                                    ; preds = %178
  %183 = trunc i64 %179 to i32
  %184 = and i32 %183, 255
  %185 = tail call i32 @putchar(i32 %184) #9
  br label %188

; <label>:186:                                    ; preds = %178
  %187 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %188

; <label>:188:                                    ; preds = %186, %182
  %189 = ashr exact i64 %44, 24
  %190 = trunc i64 %189 to i8
  %191 = icmp slt i8 %190, 32
  br i1 %191, label %196, label %192

; <label>:192:                                    ; preds = %188
  %193 = trunc i64 %189 to i32
  %194 = and i32 %193, 255
  %195 = tail call i32 @putchar(i32 %194) #9
  br label %198

; <label>:196:                                    ; preds = %188
  %197 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %198

; <label>:198:                                    ; preds = %196, %192
  %199 = ashr exact i64 %50, 16
  %200 = trunc i64 %199 to i8
  %201 = icmp slt i8 %200, 32
  br i1 %201, label %206, label %202

; <label>:202:                                    ; preds = %198
  %203 = trunc i64 %199 to i32
  %204 = and i32 %203, 255
  %205 = tail call i32 @putchar(i32 %204) #9
  br label %208

; <label>:206:                                    ; preds = %198
  %207 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %208

; <label>:208:                                    ; preds = %206, %202
  %209 = ashr i64 %56, 8
  %210 = trunc i64 %209 to i8
  %211 = icmp slt i8 %210, 32
  br i1 %211, label %216, label %212

; <label>:212:                                    ; preds = %208
  %213 = trunc i64 %209 to i32
  %214 = and i32 %213, 255
  %215 = tail call i32 @putchar(i32 %214) #9
  br label %218

; <label>:216:                                    ; preds = %208
  %217 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %218

; <label>:218:                                    ; preds = %216, %212
  %219 = trunc i64 %61 to i8
  %220 = icmp slt i8 %219, 32
  br i1 %220, label %225, label %221

; <label>:221:                                    ; preds = %218
  %222 = trunc i64 %61 to i32
  %223 = and i32 %222, 255
  %224 = tail call i32 @putchar(i32 %223) #9
  br label %104

; <label>:225:                                    ; preds = %218
  %226 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i64 0, i64 0))
  br label %104
}

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #2

declare i8* @wvm_disassemble(i64) local_unnamed_addr #4

declare i32 @putchar(i32)

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #3

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind allocsize(0,1) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { norecurse nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { norecurse nounwind readonly ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { allocsize(0,1) }
attributes #9 = { nounwind }
attributes #10 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"long long", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"long", !4, i64 0}
!8 = !{!9, !9, i64 0}
!9 = !{!"_Bool", !4, i64 0}
!10 = !{!11, !11, i64 0}
!11 = !{!"int", !4, i64 0}
!12 = !{!13, !13, i64 0}
!13 = !{!"any pointer", !4, i64 0}
!14 = !{!4, !4, i64 0}
!15 = !{i8 0, i8 2}
