; asm output: =r (wow), +l (uh)
; asm input:   a (j), a (k)

; int i = 5;
; int retvar() {
;     int wow, uh, j = i + 1, k = j + 2;
;     asm (
;         "$a0 + $a1 -> $q"
;         : "=r" (wow), "+l" (uh)
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
	%9 = load i32, i32* %2, align 4  ; %9  = *uh
	%10 = load i32, i32* %3, align 4 ; %10 = *j
	%11 = load i32, i32* %4, align 4 ; %11 = *k
	%12 = call { i32, i32 } asm
		"$$a0 + $$a1 -> $$q",
		"=r,=l,{ax},{ax},1,~{cc},~{dirflag},~{fpsr},~{flags}"
		(i32 %10, i32 %11, i32 %9) ; *j, *k, *uh
		#2, !srcloc !3
	%13 = extractvalue { i32, i32 } %12, 0 ; (wow = %13)
	%14 = extractvalue { i32, i32 } %12, 1 ; (uh  = %14)
	store i32 %13, i32* %1, align 4  ; *wow = %13
	store i32 %14, i32* %2, align 4  ; *uh  = %14
	%15 = load i32, i32* %1, align 4 ; \ return *wow
	ret i32 %15                      ; /
}
