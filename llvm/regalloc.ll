define void @_main() #0 {
	%a = alloca i8
	%d = alloca i8
	br i8 %0, label %1, label %2

; <label>:1: ; preds = %0
	%e1 = alloca i8
	%c1 = zext i8 %d to i8
	br label %3

; <label>:2: ; preds = %0
	%b = alloca i8
	%c2 = zext i8 %a to i8
	%e2 = zext i8 %b to i8
	br label %3

; <label>:3: ; preds = %1, %2
	%e = phi i8 [%e1, %1], [%e2, %2]
	%c = phi i8 [%c1, %1], [%c2, %2]
	call void @_int(i8 %e, i8 %c)
	ret void
}
