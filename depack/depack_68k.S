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

	moveq	#0,d4
	move.b	(a1)+,d4	// hi-offset::count

	moveq	#$0f,d2
	and.w	d4,d2		; d2 = count
	add.w	d2,d2
	lsl.w	#4,d4
	move.b	(a1)+,d4	; d4 = offset

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
