	.68000

	lea	code+4(pc),a1
	move.l	(a1)+,d0
	move.l	#$40000,a0
	bsr	depack
	jmp	$40000

depack:
	lea	(a0,d0.l),a3	// last unpacked byte

	moveq	#0,d2
depack0:
	cmpa.l	a3,a0
	bhs.s	depack9

	move.b	(a1)+,d1	// pack-byte
	beq.s	copy8
	moveq	#7,d0
depack1:
	add.b	d1,d1
	bcc.s	depack2

	move.b	(a1)+,d2	// hi-offset::count
	move.b	(a1)+,d3	// low-offset

	move.w	#$f0,d4
	and.w	d2,d4
	andi.w	#$0F,d2		// r2:= count
	add.w	d2,d2
	lsl.w	#4,d4
	move.b	d3,d4		// r4:= offset

	neg.w	d4
	lea	(a0,d4.w),a2
	jmp	copy(pc,d2.w)

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
	dbra	d0,depack1
	bra.s	depack0

depack2:
	move.b	(a1)+,(A0)+
	dbra	d0,depack1
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
	rts
	;;
code:
	;; packed file
