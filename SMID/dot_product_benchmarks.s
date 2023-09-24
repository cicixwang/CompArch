	.file	"dot_product_benchmarks.c"
	.text
	.globl	dot_product_C
	.type	dot_product_C, @function
dot_product_C:
.LFB4741:
	.cfi_startproc
	testq	%rdi, %rdi
	jle	.L4
	movl	$0, %ecx
	movl	$0, %eax
.L3:
	movslq	%ecx, %r9
	movzwl	(%rsi,%r9,2), %r8d
	movzwl	(%rdx,%r9,2), %r9d
	imull	%r9d, %r8d
	addl	%r8d, %eax
	addl	$1, %ecx
	movslq	%ecx, %r8
	cmpq	%rdi, %r8
	jl	.L3
	ret
.L4:
	movl	$0, %eax
	ret
	.cfi_endproc
.LFE4741:
	.size	dot_product_C, .-dot_product_C
	.globl	dot_product_AVX
	.type	dot_product_AVX, @function
dot_product_AVX:
.LFB4742:
	.cfi_startproc
	testq	%rdi, %rdi
	jle	.L9
	movl	$0, %eax
	vpxor	%xmm1, %xmm1, %xmm1
.L8:
	movslq	%eax, %rcx
	vpmovzxwd	(%rsi,%rcx,2), %ymm2
	vpmovzxwd	(%rdx,%rcx,2), %ymm0
	vpmulld	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm1
	addl	$8, %eax
	movslq	%eax, %rcx
	cmpq	%rdi, %rcx
	jl	.L8
.L7:
	vmovd	%xmm1, %eax
	vpextrd	$1, %xmm1, %edx
	addl	%edx, %eax
	vpextrd	$2, %xmm1, %edx
	addl	%edx, %eax
	vpextrd	$3, %xmm1, %edx
	addl	%edx, %eax
	vextracti128	$0x1, %ymm1, %xmm1
	vmovd	%xmm1, %edx
	addl	%edx, %eax
	vpextrd	$1, %xmm1, %edx
	addl	%edx, %eax
	vpextrd	$2, %xmm1, %edx
	addl	%eax, %edx
	vpextrd	$3, %xmm1, %eax
	addl	%edx, %eax
	ret
.L9:
	vpxor	%xmm1, %xmm1, %xmm1
	jmp	.L7
	.cfi_endproc
.LFE4742:
	.size	dot_product_AVX, .-dot_product_AVX
	.globl	functions
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"C (local)"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC1:
	.string	"C (compiled with GCC7.2 -O3 -mavx2)"
	.section	.rodata.str1.1
.LC2:
	.string	"dot_product_AVX"
	.data
	.align 32
	.type	functions, @object
	.size	functions, 64
functions:
	.quad	dot_product_C
	.quad	.LC0
	.quad	dot_product_gcc7_O3
	.quad	.LC1
	.quad	dot_product_AVX
	.quad	.LC2
	.quad	0
	.quad	0
	.ident	"GCC: (GNU) 7.1.0"
	.section	.note.GNU-stack,"",@progbits
