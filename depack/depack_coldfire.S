// pack with :	 -l +j
// depack(__u32 size,char *dst,char *src)

	.text
	.globl _start
_start:
	lea	(_start,pc),a7
	move.l	#0x1000,d0
	lea	0x10000,a0
clr:
	clr.l	(a0)+
	subq.l	#1,d0
	bne.s	clr

	lea	packed+8(pc),a0
	pea	(a0)
	pea	0x10000
	move.l	-(a0),-(sp)


	bsr	depack
1:
	bra.s	1b


// d0 - packed size
// a0 - dst for unpacked data
// a1 - src of packed data
	.text
	.globl	depack
depack:
	movem.l 4(sp),d0/a0-a1
	lea	-20(sp),sp
	movem.l	d2-d4/a2-a3,(sp)

	lea	(a0,d0.l),a3	// last unpacked byte

depack0:
	cmpa.l	a3,a0
	bhs.s	depack9

	move.b	(a1)+,d1	// pack-byte
	beq	copy8
	moveq	#7,d0
depack1:
	btst	d0,d1
	beq.s	depack2

	move.b	(a1)+,d2	// hi-offset::count
	move.b	(a1)+,d3	// low-offset

	move.l	#0xf0,d4
	and.l	d2,d4		// r4:= hi-offset
	andi.l	#0x0F,d2	// r2:= count
	lsl.l	#4,d4
	move.b	d3,d4		// r4:= offset
	neg.l	d4
	lea	(a0,d4.l),a2
	jmp	copy(pc,d2*2)

copy:
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+

	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+

	move.b	(a2)+,(a0)+
	move.b	(a2)+,(a0)+

	dc.w	0x51f8+2		// tpf.w (skip next opcode)
depack2:
	move.b	(a1)+,(A0)+

	subq.l	#1,d0
	bpl.s	depack1
	bra.s	depack0

copy8:
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	move.b	(a1)+,(A0)+
	bra.s	depack0

depack9:
	movem.l	(sp),d2-d4/a2-a3
	lea	20(sp),sp
	rts

	.data
packed:
	.incbin "tp.pck"
	.align
packed_end:
