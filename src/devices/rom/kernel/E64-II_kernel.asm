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
	lea		interrupt_4_autovector,a0
	move.l	a0,VEC_28_LEVEL4_IRQ_AUTOVECT
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
	lea		timer2_irq_handler,a0
	move.l	a0,TIMER2_VECTOR
	lea		timer3_irq_handler,a0
	move.l	a0,TIMER3_VECTOR

	; set up timer0 interrupts
	move.w	#$003c,TIMER_BASE+2		; load value 60 ($003c = 60bpm = 1Hz) into high and low bytes
	ori.b	#%00000001,TIMER_BASE+1	; turn on interrupt generation by clock0

	; set up timer1 interrupts
	move.w	#$0708,TIMER_BASE+2		; load value
	ori.b	#%00000010,TIMER_BASE+1	; turn on interrupt generation by clock1

	; set up timer3 interrupts at 50.125Hz for music / sid tunes
	move.w	#$0bc0,TIMER_BASE+2		; 3008bpm (=50.125Hz)
	ori.b	#%00001000,TIMER_BASE+1	; turn on interrupt generation by clock3

	; set ipl to level 1 (all interrupts of >=2 level will be acknowledged)
	move.w	sr,d0
	andi.w	#%1111100011111111,d0
	ori.w	#%0000000100000000,d0
	move.w	d0,sr

	; max volume for both sids
	lea		SID0_BASE,a0
	move.b	#$0f,($18,a0)
	lea		SID1_BASE,a0
	move.b	#$0f,($18,a0)

	; copy char rom to ram (go from 2k to 32k)
	bsr		copy_charrom_to_charram

	; copy kernel into ram to make it visible for VICV
	movea.l	#KERNEL_LOC,a0
	movea.l	a0,a1
	move.l	#$10000,d0
	bsr		memcopy

	; set screen colors
	move.w	#C64_BLACK,VICV_BORDER_COLOR
	move.w	#C64_BLUE,VICV_BACKGROUND_COLOR
	; set border size
	move.b	#$0,VICV_BORDER_SIZE
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

	; play a welcome sound on SID0
	lea		SID0_BASE,a0
	; frequency of voice 1
	lea		notes,a1
	move.w	(N_D3_,a1),(a0)
	; attack and decay of voice 1
	move.b	#%00001001,($05,a0)
	; pulse width of voice 1
	move.w	#$f0f,($02,a0)
	; freq of voice 3
	move.w	#$1e00,($0e,a0)
	; left channel mix
	move.b	#$ff,(SID0_LEFT,a0)
	; right channel mix
	move.b	#$10,(SID0_RGHT,a0)
	; play sound by opening gate on bit0 register #4
	; bit 6 is for a pulse wave form
	move.b	#%01000001,($04,a0)

	; play a welcome sound on SID1
	lea		SID1_BASE,a0
	; frequency of voice 1
	lea		notes,a1
	move.w	(N_A3_,a1),(a0)
	; attack and decay of voice 1
	move.b	#%00001001,($05,a0)
	; pulse width of voice 1
	move.w	#$f0f,($02,a0)
	; freq of voice 3
	move.w	#$1e00,($0e,a0)
	; left channel mix
	move.b	#$10,(SID1_LEFT,a0)
	; right channel mix
	move.b	#$ff,(SID1_RGHT,a0)
	; play sound by opening gate on bit0 register #4
	; bit 6 is for a pulse wave form
	move.b	#%01000001,($04,a0)

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
	move.l	d0,-(a7)
	move.l #$deadbeef,d0
	move.l	(a7)+,d0
	rte

; level 2 interrupt autovector (vicv vblank)
interrupt_2_autovector
	move.b	#%00000001,VICV_ISR				; acknowledge VBLANK interrupt
	move.b	#%00000001,VICV_BUFFERSWAP		; switch front- and backbuffer
	move.w	#C64_BLUE,BLITTER_DATA_16_BIT	; load color blue in data register of blitter
	move.b	#%00000001,BLITTER_CONTROL		; clear the backbuffer
											; plan some other blitter stuff (text screen, ...)
	rte

; level 4 interrupt autovector (timer)
interrupt_4_autovector
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
	btst	#2,TIMER_BASE			; did timer 2 cause the interrupt?
	beq		timer3_check			; no, go to next timer
	move.b	#%00000100,TIMER_BASE	; yes, acknowledge interrupt
	movea.l	TIMER2_VECTOR,a0
	jmp		(a0)
timer3_check
	btst	#3,TIMER_BASE
	beq		timer_finish			; no, go to finish
	move.b	#%00001000,TIMER_BASE	; yes, acknowledge
	movea.l	TIMER3_VECTOR,a0
	jmp		(a0)
timer_finish
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

timer2_irq_handler
	;
	;
	bra		timer3_check

timer3_irq_handler
	;
	addq.b	#1,VICV_BORDER_SIZE
	;
	bra		timer_finish

memcopy
	;
	;	bytewise memory copy - probably very slow...
	;
	;	Arguments
	;
	;	a0	source_start_address
	;	d0	no_of_bytes
	;	a1	destination_start_address
	;
	move.l	d1,-(a7)			; save d1
	moveq	#$0,d1
.1	move.b	(a0,d1.l),(a1,d1.l)
	add.l	#$1,d1
	cmp.l	d1,d0
	bne		.1

	move.l	(a7)+,d1			; restore d1
	rts




copy_charrom_to_charram
	;
	;	Register Usage
	;
	;	d0	current_byte, holds a byte from the original rom charset
	;	d1	i, counter from 7 to 0 (8 bits per byte have to be processed)
	;	a0	*char_ram, pointer
	;	a1	*char_rom, pointer
	;
	movem	d0-d1/a0-a1,-(a7)

	moveq	#0,d0				;	current_byte = 0;
	lea		CHAR_RAM,a0			;	char_ram = CHAR_RAM;
	lea		CHAR_ROM,a1			;	char_rom = CHAR_ROM;

.1	cmpa.l	#CHAR_ROM+$800,a1	;	while(char_ram != CHAR_ROM+$800)	//	if we're not at the end of char rom
	beq		.5					;	{									//	branch to end of compound statement
	move.b	(a1)+,d0			;		current_byte = char_rom++;		//	load a byte from charset and incr pntr
	moveq	#8,d1				;		i = 8;
.2	btst	#$7,d0
	beq		.3					; bit 7 not set
	move.w	#C64_GREY,(a0)+		; bit 7 is set, so set color
	bra		.4
.3	move.w	#$0000,(a0)+		; bit 7 not set, make empty
.4	lsl.b	#$01,d0				; move all the bits one place to the left
	subq	#$01,d1				;	i = i - 1;
	beq		.1					;	did i reach zero? goto .1
	bra		.2
								;	}
.5	movem	(a7)+,d0-d1/a0-a1	;
	rts							;

; logo blit description
logo_blit_structure
	dc.b	%00000011	; multicolor and bitmap mode
	dc.b	%00000000
	dc.w	%00000011	; width 2^3 = 8 chars = 64 pixels
	dc.w	%00000000	; height 2^0 = 1 char =  8 pixels
	dc.w	$0010		; x_pos
	dc.w	$0010		; y_pos

; logo blit bitmap data
logo_bitmap
	dc.w	$0000,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$0000

	dc.w	$f444,$f444,$f733,$f733,$f733,$f733,$f733,$f733
	dc.w	$f733,$f733,$f733,$f733,$f733,$f733,$f733,$f733
	dc.w	$f733,$f733,$f444,$faaa,$faaa,$faaa,$faaa,$f444
	dc.w	$f444,$faaa,$faaa,$faaa,$f444,$f444,$f444,$f444
	dc.w	$faaa,$faaa,$f444,$f444,$f444,$f444,$faaa,$faaa
	dc.w	$faaa,$f444,$faaa,$faaa,$faaa,$f444,$f733,$f733
	dc.w	$f733,$f733,$f733,$f733,$f733,$f733,$f733,$f733
	dc.w	$f733,$f733,$f733,$f733,$f733,$f733,$f444,$f444

	dc.w	$f444,$f853,$f853,$f853,$f853,$f853,$f853,$f853
	dc.w	$f853,$f853,$f853,$f853,$f853,$f853,$f853,$f853
	dc.w	$f853,$f853,$f444,$faaa,$f444,$f444,$f444,$f444
	dc.w	$faaa,$f444,$f444,$f444,$f444,$f444,$f444,$faaa
	dc.w	$f444,$faaa,$f444,$f444,$f444,$f444,$f444,$faaa
	dc.w	$f444,$f444,$f444,$faaa,$f444,$f444,$f853,$f853
	dc.w	$f853,$f853,$f853,$f853,$f853,$f853,$f853,$f853
	dc.w	$f853,$f853,$f853,$f853,$f853,$f853,$f853,$f444

	dc.w	$f444,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8
	dc.w	$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8
	dc.w	$fee8,$fee8,$f444,$faaa,$faaa,$faaa,$f444,$f444
	dc.w	$faaa,$faaa,$faaa,$faaa,$f444,$f444,$faaa,$f444
	dc.w	$f444,$faaa,$f444,$faaa,$faaa,$f444,$f444,$faaa
	dc.w	$f444,$f444,$f444,$faaa,$f444,$f444,$fee8,$fee8
	dc.w	$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8
	dc.w	$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$f444

	dc.w	$f444,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa
	dc.w	$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa
	dc.w	$fbfa,$fbfa,$f444,$faaa,$f444,$f444,$f444,$f444
	dc.w	$faaa,$f444,$f444,$f444,$faaa,$f444,$faaa,$faaa
	dc.w	$faaa,$faaa,$f444,$f444,$f444,$f444,$f444,$faaa
	dc.w	$f444,$f444,$f444,$faaa,$f444,$f444,$fbfa,$fbfa
	dc.w	$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa
	dc.w	$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$f444

	dc.w	$f444,$f444,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d
	dc.w	$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d
	dc.w	$f67d,$f67d,$f444,$faaa,$faaa,$faaa,$faaa,$f444
	dc.w	$f444,$faaa,$faaa,$faaa,$f444,$f444,$f444,$f444
	dc.w	$f444,$faaa,$f444,$f444,$f444,$f444,$faaa,$faaa
	dc.w	$faaa,$f444,$faaa,$faaa,$faaa,$f444,$f67d,$f67d
	dc.w	$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d
	dc.w	$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f444,$f444

	dc.w	$0000,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	dc.w	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$0000

	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	dc.w	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000

; string data
welcome
	dc.b	"E64-II (C)2019-2020 kernel version 0.1.20200420",ASCII_LF,ASCII_NULL

	align 1
	include "E64-II_kernel_tables.asm"

	org		KERNEL_LOC+$fffc
	dc.l	$deadbeef