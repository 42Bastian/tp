// pack with :	 +l +j
// depack(__u32 size,char *dst,char *src)

// r0 - packed size
// r1 - dst for unpacked data
// r2 - src of packed data

	.text
	.code 32
	.globl _start
_start:
	ldr	r0,=0x1000
	ldr	r1,=0x10000
clr:
	mov	r2,#0
	str	r2,[r1],#4
	subs	r0,r0,#1
	bne	clr

	ldr	r1,=0x10000
	adr	r2,packed+8
	ldr	r0,[r2,#-4]
	bl	depack
1:
	b	1b


	.globl	depack
	.code 32
depack:
	stmfd	sp!,{r4-r7,lr}
	add	lr,r1,r0	// lr:= last unpacked byte
depack0:
	cmp	lr,r1
	blt	depack9
	ldrb	r3,[r2],#1
	mov	r4,#9
	movs	r3,r3,lsl #24
	beq	copy8

depack1:
	subs	r4,r4,#1
	beq	depack0
	ldrb	r5,[r2],#1
	adds	r3,r3,r3
	strccb	r5,[r1],#1
	bcc	depack1
	ldrb	r6,[r2],#1
	and	r7,r5,#0x0f	// r7 := count
	and	r5,r5,#0xf0
	orr	r6,r6,r5,lsl #4	// r6 := offset
	sub	r6,r1,r6
	add	pc,pc,r7,lsl #3
	nop
_16:
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
_8:
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1

	ldrb	r5,[r6],#1
	strb	r5,[r1],#1
	ldrb	r5,[r6],#1
	strb	r5,[r1],#1

	b	depack1

copy8:
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	ldrb	r5,[r2],#1
	strb	r5,[r1],#1
	b	depack0


depack9:
	ldmfd	sp!,{r4-r7}
	ldmfd	sp!,{r3}
	bx	r3

	.ltorg
packed:
	.incbin "tp.pck"
packed_end:
