; asm output: =r (wow), =l (uh)
; asm input:   a (j), a (k)

; int i = 5;
; int retvar() {
;     int wow, uh, j = i + 1, k = j + 2;
;     asm (
;         "$a0 + $a1 -> $q"
;         : "=r" (wow), "=l" (uh)
;         : "a" (j), "a" (k)
;         : "cc"
;     );
;     return wow;
; }

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @retvar() #0 {
	%1 = alloca i32, align 4  ; int *wow;
	%2 = alloca i32, align 4  ; int *uh;
	%3 = alloca i32, align 4  ; int *j;
	%4 = alloca i32, align 4  ; int *k;
	%5 = load i32, i32* @i, align 4  ; \
	%6 = add nsw i32 %5, 1           ; | *j = *i + 1 
	store i32 %6, i32* %3, align 4   ; /
	%7 = load i32, i32* %3, align 4  ; \
	%8 = add nsw i32 %7, 2           ; | *k = *j + 2
	store i32 %8, i32* %4, align 4   ; /
	%9 = load i32, i32* %3, align 4  ; %9  = *j
	%10 = load i32, i32* %4, align 4 ; %10 = *k

	%11 = call { i32, i32 } asm
		"$$a0 + $$a1 -> $$q",
		"=r,=l,{ax},{ax},~{cc},~{dirflag},~{fpsr},~{flags}"
		(i32 %9, i32 %10)          ; *j, *k
		#2, !srcloc !3
	%12 = extractvalue { i32, i32 } %11, 0 ; (wow = %12)
	%13 = extractvalue { i32, i32 } %11, 1 ; (uh  = %13)
	store i32 %12, i32* %1, align 4  ; *wow = %12
	store i32 %13, i32* %2, align 4  ; *uh  = %13
	%14 = load i32, i32* %1, align 4 ; \ return *wow
	ret i32 %14                      ; /
}

