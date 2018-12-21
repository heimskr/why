	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 14
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	movq	%rsi, %r14
	movl	%edi, %ebx
	movl	$16777215, %edi         ## imm = 0xFFFFFF
	callq	_wvm_init
	testb	%al, %al
	je	LBB0_12
## %bb.1:
	cmpl	$3, %ebx
	je	LBB0_6
## %bb.2:
	cmpl	$2, %ebx
	je	LBB0_5
## %bb.3:
	cmpl	$1, %ebx
	jne	LBB0_9
## %bb.4:
	leaq	L_.str.1(%rip), %rdi
	jmp	LBB0_8
LBB0_5:
	movq	8(%r14), %rdi
	jmp	LBB0_8
LBB0_6:
	movq	8(%r14), %rdi
	leaq	L_.str.2(%rip), %rsi
	callq	_strcmp
	testl	%eax, %eax
	jne	LBB0_9
## %bb.7:
	movq	16(%r14), %r14
	movq	%r14, %rdi
	callq	_strlen
	leaq	25(%rax), %rdi
	movl	$1, %esi
	callq	_calloc
	movq	%rax, %rbx
	leaq	L_.str.3(%rip), %rcx
	movl	$0, %esi
	movq	$-1, %rdx
	xorl	%eax, %eax
	movq	%rbx, %rdi
	movq	%r14, %r8
	callq	___sprintf_chk
	movq	%rbx, %rdi
LBB0_8:
	callq	_wvm_load
LBB0_9:
	xorl	%eax, %eax
	callq	_wvm_init_vm
	xorl	%eax, %eax
	callq	_wvm_print_memory
	.p2align	4, 0x90
LBB0_10:                                ## =>This Inner Loop Header: Depth=1
	xorl	%eax, %eax
	callq	_wvm_tick
	testb	%al, %al
	jne	LBB0_10
## %bb.11:
	movq	_cycles@GOTPCREL(%rip), %rax
	movl	(%rax), %esi
	cmpl	$1, %esi
	leaq	L_.str.5(%rip), %rax
	leaq	L_.str.6(%rip), %rdx
	cmoveq	%rax, %rdx
	leaq	L_.str.4(%rip), %rdi
	xorl	%eax, %eax
	callq	_printf
	xorl	%eax, %eax
	callq	_wvm_free
	xorl	%eax, %eax
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB0_12:
	movq	___stderrp@GOTPCREL(%rip), %rax
	movq	(%rax), %rcx
	leaq	L_.str(%rip), %rdi
	movl	$26, %esi
	movl	$1, %edx
	callq	_fwrite
	movl	$1, %edi
	callq	_exit
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Couldn't allocate memory.\n"

L_.str.1:                               ## @.str.1
	.asciz	"../../wasm/compiled/memory.why"

L_.str.2:                               ## @.str.2
	.asciz	"-c"

L_.str.3:                               ## @.str.3
	.asciz	"../../wasm/compiled/%s.why"

L_.str.4:                               ## @.str.4
	.asciz	"\n\033[31mExecution halted after %d cycle%s.\033[0m\n"

	.comm	_cycles,4,2             ## @cycles
L_.str.5:                               ## @.str.5
	.space	1

L_.str.6:                               ## @.str.6
	.asciz	"s"

	.comm	_memory,8,3             ## @memory
	.comm	_memsize,8,3            ## @memsize
	.comm	_pc,8,3                 ## @pc
	.comm	_offset_symtab,8,3      ## @offset_symtab
	.comm	_offset_handlers,8,3    ## @offset_handlers
	.comm	_offset_data,8,3        ## @offset_data
	.comm	_offset_code,8,3        ## @offset_code
	.comm	_offset_end,8,3         ## @offset_end
	.comm	_registers,1024,4       ## @registers
	.comm	_membytes,8,3           ## @membytes
	.comm	_alive,1,0              ## @alive

.subsections_via_symbols
