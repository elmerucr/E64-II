; elmerucr - 18/12/2019
; compiles with vasmm68k_mot

	include 'E64-II_kernel_definitions.asm'

	org		KERNEL_LOC

	dc.l	$00d00000				; vector 0 - supervisor stackpointer
	dc.l	kernel_main				; vector 1 - reset vector

; start of main kernel code
kernel_main
	lea		exception_handler,a0
	move.l	a0,VEC_04_ILLEGAL_INSTRUCTION
	move.l	a0,VEC_10_UNIMPL_INSTRUCTION
	move.l	a0,VEC_11_UNIMPL_INSTRUCTION
	lea		interrupt_2_autovector,a0
	move.l	a0,VEC_26_LEVEL2_IRQ_AUTOVECT
	lea		interrupt_5_autovector,a0
	move.l	a0,VEC_29_LEVEL5_IRQ_AUTOVECT
	lea		interrupt_6_autovector,a0
	move.l	a0,VEC_30_LEVEL6_IRQ_AUTOVECT
	lea		interrupt_7_autovector,a0
	move.l	a0,VEC_31_LEVEL7_IRQ_AUTOVECT
	lea		timer0_irq_handler,a0
	move.l	a0,TIMER0_VECTOR
	lea		timer1_irq_handler,a0
	move.l	a0,TIMER1_VECTOR

	; set up timer0 interrupt
	move.w	#$003c,TIMER_BASE+2		; load value 60 ($003c = 60bpm = 1Hz) into high and low bytes
	ori.b	#%00000001,TIMER_BASE+1	; turn on interrupt generation by clock0
	; set up timer1 interrupt
	move.w	#$00f0,TIMER_BASE+2		; load value 240
	ori.b	#%00000010,TIMER_BASE+1	; turn on interrupt generation by clock1


	; set screen colors
	move.b	#$00,VICV_BASE			; c64 black
	move.b	#$06,VICV_BASE+1		; c64 blue
	; set text color
	move.b	#$0c,CURR_TEXT_COLOR	; c64 grey

	; set txt pointer
	move.l	#$00f00000,VICV_TXT
	move.l	#$00f00800,VICV_COL

	; reset cursor position
	move.w	#$0,CURSOR_POS

	; clear screen
	bsr		clear_screen

	lea		welcome,a0
	bsr		put_string

	; set ipl to level 1
	move.w	sr,d0
	andi.w	#%1111100011111111,d0
	ori.w	#%0000000100000000,d0
	move.w	d0,sr

	; play a welcome sound on SID0
	lea		SID0_BASE,a0
	; frequency of voice 1
	lea		notes,a1
	move.w	(N_D3_,a1),(a0)
	; attack and decay of voice 1
	move.b	#%00001001,($05,a0)
	; freq of voice 3
	move.w	#$1e00,($0e,a0)
	; max volume
	move.b	#$0f,($18,a0)
	; left channel mix
	move.b	#$ff,(SID0_LEFT,a0)
	; right channel mix
	move.b	#$10,(SID0_RGHT,a0)
	; play bell by opening gate on bit0 register #4
	; bit 4 is for a triangle wave form
    ; bit 2 is for a ring modulation connected to voice 3
	move.b	#%00100001,($04,a0)

	; play a welcome sound on SID1
	lea		SID1_BASE,a0
	; frequency of voice 1
	lea		notes,a1
	move.w	(N_A3_,a1),(a0)
	;move.w	#N_A3_,(a0)
	; attack and decay of voice 1
	move.b	#%00001001,($05,a0)
	; freq of voice 3
	move.w	#$1e00,($0e,a0)
	; max volume
	move.b	#$0f,($18,a0)
	; left channel mix
	move.b	#$10,(SID1_LEFT,a0)
	; right channel mix
	move.b	#$ff,(SID1_RGHT,a0)
	; play bell by opening gate on bit0 register #4
	; bit 4 is for a triangle wave form
    ; bit 2 is for a ring modulation connected to voice 3
	move.b	#%00100001,($04,a0)

mainloop
	; put something in the usp
	movea.l	#$c00000,a0
	move	a0,usp

	; copy keyboard state in to screen
.1	moveq	#$0,d0
	movea.l	VICV_TXT,a0
	lea		$400(a0),a0
	lea		CIA_BASE,a1
	lea		$80(a1),a1
.2	move.b	(a1,d0),(a0,d0)
	addq	#$1,d0
	cmp.b	#$49,d0
	bne		.2

	addq.b	#$1,$00f00080
	bra.s	.1

clear_screen
	movem.l	d0-d1/a0-a2,-(a7)
	movea.l	(VICV_TXT),a0
	movea.l	(VICV_COL),a1
	movea.l	a0,a2
	lea		$800(a2),a2
	move.l	#$20202020,d0
	moveq	#$00,d1
	move.b	CURR_TEXT_COLOR,d1
	lsl.l	#$8,d1
	move.b	CURR_TEXT_COLOR,d1
	lsl.l	#$8,d1
	move.b	CURR_TEXT_COLOR,d1
	lsl.l	#$8,d1
	move.b	CURR_TEXT_COLOR,d1
.1	move.l	d0,(a0)+
	move.l	d1,(a1)+
	cmp.l	a0,a2
	bne		.1
	movem.l	(a7)+,d0-d1/a0-a2
	rts

; put_char expects an ascii value in d0
put_char
	movem.l	d1-d2/a0-a2,-(a7)			; save registers
	move.w	CURSOR_POS,d1				; load current cursor position into d1
	move.b	CURR_TEXT_COLOR,d2			; load current text colour into d2
	movea.l	VICV_TXT,a0					; load pointer to current text screen into a0
	movea.l	VICV_COL,a1					; load pointer to current color screen into a1
	lea		ascii_to_screencode,a2		; a2 now points to ascii-screencode table
	cmp.b	#ASCII_LF,d0				; do we have a line feed as the next ascii?
	beq		.1
	move.b	(a2,d0),d0					; change the ascii value to a screencode value
	move.b	d0,(a0,d1)
	move.b	d2,(a1,d1)
	addq	#$1,CURSOR_POS
	andi.w	#$07ff,CURSOR_POS
	movem.l	(a7)+,d1-d2/a0-a2			; restore registers
	rts
.1	addi.w	#$40,d1						; add 64 positions to current cursor pos
	andi.w	#%1111111111000000,d1		; move cursor pos to beginning of line
	move.w	d1,CURSOR_POS				; store new value
	movem.l	(a7)+,d1-d2/a0-a2			; restore registers
	rts

; put_string expects a pointer to a string in a0
put_string
	move.l	a0,-(a7)
.1	move.b	(a0)+,d0						; move the first ascii value of string into d0
	cmp.b	#ASCII_NULL,d0					; did we reach the end of the string?
	beq		.2									; yes, go to end of function
	bsr		put_char						; no, print char
	bra		.1
.2	move.l	(a7)+,a0
	rts

; fake exception handler
exception_handler
	move.l #$deadbeef,d0
	rte

; level 2 interrupt autovector (timer)
interrupt_2_autovector
	move.l	a0,-(a7)				; save a0
timer0_check
	btst	#0,TIMER_BASE			; did timer 0 cause the interrupt?
	beq		timer1_check			; no, go to next timer
	move.b	#%00000001,TIMER_BASE	; yes, acknowledge interrupt
	movea.l	TIMER0_VECTOR,a0
	jmp		(a0)
timer1_check
	btst	#1,TIMER_BASE
	beq		timer2_check
	move.b	#%00000010,TIMER_BASE
	movea.l	TIMER1_VECTOR,a0
	jmp		(a0)
timer2_check
	move.l	(a7)+,a0				; restore a0
	rte

; level 5 interrupt autovector
interrupt_5_autovector
	move.l	a0,-(a7)
	move.l	(a7)+,a0
	rte

; level 6 interrupt autovector
interrupt_6_autovector
	rte

; level 7 interrupt autovector
interrupt_7_autovector
	rte

timer0_irq_handler
	move.l	a0,-(a7)
	movea.l	VICV_COL,a0
	addq.b	#$1,(a0)
	andi.b	#%00001111,(a0)
	movea.l	(a7)+,a0
	bra		timer1_check

timer1_irq_handler
	move.l	a0,-(a7)
	movea.l	VICV_COL,a0
	addq.b	#$1,(1,a0)
	andi.b	#%00001111,(1,a0)
	movea.l	(a7)+,a0
	bra		timer2_check

; string data
welcome
	dc.b	"E64-II (C)2019 kernel version 0.1.20200115",ASCII_LF,ASCII_NULL

	align 1
	include "E64-II_kernel_tables.asm"

	org		KERNEL_LOC+$fffc
	dc.l	$deadbeef