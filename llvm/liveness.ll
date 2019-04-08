declare void @strprint(i32)

define void @liveness() #0 {
	%q = alloca i1, align 4 ; dummy variable
	br label %2

; <label>:2:                                     ; preds = %0, %7
	br i1 %q, label %3, label %11

; <label>:3:                                     ; preds = %2
	%w = alloca i32, align 4
	%x = alloca i32, align 4
	%y = alloca i32, align 4
	br i1 %q, label %4, label %8

; <label>:4:                                     ; preds = %3
	call void @strprint(i32 %w)
	br label %5

; <label>:5:                                     ; preds = %4, %6
	call void @strprint(i32 %y)
	br label %6

; <label>:6:                                     ; preds = %5, %9
	br i1 %q, label %5, label %7

; <label>:7:                                     ; preds = %6
	br label %2

; <label>:8:                                     ; preds = %3, %10
	br label %9

; <label>:9:                                     ; preds = %8
	call void @strprint(i32 %x)
	br i1 %q, label %6, label %10

; <label>:10:                                    ; preds = %9
	br label %8

; <label>:11:                                    ; preds = %2
	ret void
}
