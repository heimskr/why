@.byte  = private unnamed_addr constant i8  42, align 1
@.short = private unnamed_addr constant i16 42, align 2

declare void @_int(i16, i16)

define void @_main() #0 {
	%a = load i8, i8* @.byte, align 1
	%d = load i8, i8* @.byte, align 1
	br i1 0, label %1, label %2

; <label>:1: ; preds = %0
	%e1 = load i16, i16* @.short, align 2
	%c1 = zext i8 %d to i16
	br label %3

; <label>:2: ; preds = %0
	%b = load i8, i8* @.byte, align 1
	%c2 = zext i8 %a to i16
	%e2 = zext i8 %b to i16
	br label %3

; <label>:3: ; preds = %1, %2
	%e = phi i16 [%e1, %1], [%e2, %2]
	%c = phi i16 [%c1, %1], [%c2, %2]
	call void @_int(i16 %e, i16 %c)
	ret void
}
