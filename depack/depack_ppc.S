// pack data with :	 -l +j

#define r0 0
#define r1 1
#define r2 2
#define r3 3
#define r4 4
#define r5 5
#define r6 6
#define r7 7
#define r8 8
#define r9 9
#define r10 10
#define r11 11
#define r12 12
#define r13 13
#define r14 14
#define r15 15
#define r16 16
#define r17 17
#define r18 18
#define r19 19
#define r20 20
#define r21 21
#define r22 22
#define r23 23
#define r24 24
#define r25 25
#define r26 26
#define r27 27
#define r28 28
#define r29 29
#define r30 30
#define r31 31

	.text
	.globl	_start
_start:
	bl	1f
1:
	mflr	r1
	addi	r1,r1,-8

	lis	r3,0x010000@h
	li	r4,0x1000
	li	r0,0
	mtctr	r4
	addi	r3,r3,-4
1:
	stwu	r0,4(r3)
	bdnz	1b

	lis	r5,(packed+4)@ha
	lwzu	r3,(packed+4)@l(r5)
	addi	r5,r5,4
	lis	r4,0x10000@h
	bl	depack
1:
	b	1b


// depack(__u32 size,char *dst,char *src)
// r3 - size
// r4 - dst
// r5 - src

	.text
	.globl	depack
depack:
	add	r10,r4,r3		// last unpacked byte
	addi	r5,r5,-1		// prepare pre-increment
	addi	r4,r4,-1		// prepare pre-increment
	lis	r9,copy@ha
	addi	r9,r9,copy@l
depack1:
	cmplw	r10,r4
	lbzu	r6,1(r5)		// pack-byte
	blelr
	li	r7,8			// 8 bits
	slwi.	r6,r6,23		// shift up and test for zero
	beq	copy8
depack2:
	lbzu	r0,1(r5)		// data or hi-offset::count
	slwi.	r6,r6,1
	bge	depack3
	lbzu	r8,1(r5)		// low-offset

	rlwinm	r3,r0,3,25,28		// r3:= count_offset * 8
	add	r3,r3,r9
	mtctr	r3

	rlwimi	r8,r0,4,20,23		// r8:= offset
	sub	r8,r4,r8
	lbzu	r0,1(r8)		// load 1st byte
	addic.	r7,r7,-1		// pack-byte counter
	bctr

copy:
_18:
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)

	lbzu	r0,1(r8)
_10:
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)

	lbzu	r0,1(r8)
_2:
	stbu	r0,1(r4)
	lbzu	r0,1(r8)
	stbu	r0,1(r4)

	bne	depack2
	b	depack1

depack3:
	addic.	r7,r7,-1
	stbu	r0,1(r4)
	bne	depack2
	b	depack1

copy8:
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	lbzu	r0,1(r5)
	stbu	r0,1(r4)
	b	depack1

packed:
	.incbin "tp.pck"
	.align	2
