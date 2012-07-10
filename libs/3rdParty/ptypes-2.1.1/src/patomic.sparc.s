
	.section ".text"
	.align 4
	.global __patomic_add
	.type __patomic_add, #function
__patomic_add:
	ld	[%o0], %o2
1:
	add	%o2, %o1, %o3
	cas	[%o0], %o2, %o3
	cmp	%o2, %o3
	bne,a,pn %icc, 1b
	mov	%o3, %o2
	retl
	add	%o2, %o1, %o0		! return new value
	.size __patomic_add, .-__patomic_add

	.section ".text"
	.align 4
	.global __patomic_swap
	.type __patomic_swap, #function
__patomic_swap:
	ld	[%o0], %o2
1:
	mov	%o1, %o3
	cas	[%o0], %o2, %o3
	cmp	%o2, %o3
	bne,a,pn %icc, 1b
    mov	%o3, %o2
	retl
	mov	%o3, %o0
	.size __patomic_swap, .-__patomic_swap

	.section ".text"
	.align 4
	.global __patomic_cas
	.type __patomic_cas, #function
__patomic_cas:
	cas 	[%o0], %o1, %o2
	retl
	mov 	%o2, %o0
	.size __patomic_cas, .-__patomic_cas
