; Function Attrs: noinline nounwind ssp uwtable
define i32 @main(i32, i8** nocapture readonly) local_unnamed_addr #0 {
  %3 = alloca i32, align 4
  br i1 %10, label %14, label %11

; <label>:11:                                     ; preds = %2
  %12 = load i8*, i8** %1, align 8

; <label>:14:                                     ; preds = %2
  %15 = getelementptr inbounds i8*, i8** %1, i64 1

.lr.ph49.preheader:                               ; preds = %30
  br label %.lr.ph49

.lr.ph49:                                         ; preds = %.lr.ph49.preheader, %40
  %36 = load i32, i32* %4, align 4
}