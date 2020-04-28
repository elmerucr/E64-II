; elmerucr - 28/04/2020
; compiles with vasmm68k_mot

	INCLUDE 'E64-II_kernel_definitions.asm'

	ORG	KERNEL_LOC

	DC.L	$00d00000		; vector 0 - supervisor stackpointer
	DC.L	kernel_main		; vector 1 - reset vector


; start of main kernel code

kernel_main

	; populate vector table

	LEA	exception_handler,A0
	MOVE.L	A0,VEC_04_ILLEGAL_INSTRUCTION
	MOVE.L	A0,VEC_10_UNIMPL_INSTRUCTION
	MOVE.L	A0,VEC_11_UNIMPL_INSTRUCTION

	LEA	interrupt_2_autovector,A0
	MOVE.L	A0,VEC_26_LEVEL2_IRQ_AUTOVECT

	LEA	interrupt_4_autovector,A0
	MOVE.L	A0,VEC_28_LEVEL4_IRQ_AUTOVECT

	LEA	interrupt_5_autovector,A0
	MOVE.L	A0,VEC_29_LEVEL5_IRQ_AUTOVECT

	LEA	interrupt_6_autovector,A0
	MOVE.L	A0,VEC_30_LEVEL6_IRQ_AUTOVECT

	LEA	interrupt_7_autovector,A0
	MOVE.L	A0,VEC_31_LEVEL7_IRQ_AUTOVECT

	LEA	timer0_handler,A0
	MOVE.L	A0,TIMER0_VECTOR

	LEA	timer1_handler,A0
	MOVE.L	A0,TIMER1_VECTOR

	LEA	timer2_handler,A0
	MOVE.L	A0,TIMER2_VECTOR

	LEA	timer3_handler,A0
	MOVE.L	A0,TIMER3_VECTOR


	; set up timer0 interrupts

	MOVE.W	#$003c,TIMER_BASE+2	; load value 60 ($3c = 60bpm = 1Hz)
	ORI.B	#%00000001,TIMER_BASE+1	; turn on interrupt generation by clock0


	; set up timer1 interrupts

	MOVE.W	#$0708,TIMER_BASE+2		; load value
	ORI.B	#%00000010,TIMER_BASE+1	; turn on interrupt generation by clock1


	; set up timer3 interrupts at 50.125Hz for music / sid tunes

	MOVE.W	#$0bc0,TIMER_BASE+2		; 3008bpm (=50.125Hz)
	ORI.B	#%00001000,TIMER_BASE+1	; turn on interrupt generation by clock3


	; set ipl to level 1 (all interrupts of >=2 level will be acknowledged)

	MOVE.W	SR,D0
	ANDI.W	#%1111100011111111,D0
	ORI.W	#%0000000100000000,D0
	MOVE.W	D0,SR


	; max volume for both sids

	LEA	SID0_BASE,A0
	MOVE.B	#$0f,($18,A0)
	LEA	SID1_BASE,A0
	MOVE.B	#$0f,($18,A0)


	; copy char rom to ram (go from 2k to 32k)

	BSR	copy_charrom_to_charram


	; copy kernel into ram to make it visible for VICV

	MOVEA.L	#KERNEL_LOC,A0
	MOVEA.L	A0,A1
	MOVE.L	#$10000,D0
	BSR	memcopy


	; set screen colors

	MOVE.W	#C64_BLACK,VICV_BORDER_COLOR
	MOVE.W	#C64_BLUE,VICV_BACKGROUND_COLOR


	; set border size

	MOVE.B	#$0,VICV_BORDER_SIZE


	; set text color

	MOVE.B	#$0c,CURR_TEXT_COLOR	; c64 grey


	; set txt pointer
	MOVE.L	#$00f00000,VICV_TXT
	MOVE.L	#$00f00800,VICV_COL


	; reset cursor position
	MOVE.W	#$0,CURSOR_POS


	; clear screen and print welcome

	BSR	clear_screen
	LEA	welcome,A0
	BSR	put_string


	; play a welcome sound on SID0

	LEA	SID0_BASE,A0
	LEA	notes,A1
	MOVE.W	(N_D3_,A1),(A0)		; set frequency of voice 1
	MOVE.B	#%00001001,($05,A0)	; attack and decay of voice 1
	MOVE.W	#$f0f,($02,A0)		; pulse width of voice 1
	MOVE.B	#$ff,(SID0_LEFT,A0)	; left channel mix
	MOVE.B	#$10,(SID0_RGHT,A0)	; right channel mix
	MOVE.B	#%01000001,($04,A0)	; pulse (bit 6) and open gate (bit 0)


	; play a welcome sound on SID1

	LEA	SID1_BASE,A0
	LEA	notes,A1
	MOVE.W	(N_A3_,A1),(A0)		; set frequency of voice 1
	MOVE.B	#%00001001,($05,A0)	; attack and decay of voice 1
	MOVE.W	#$f0f,($02,A0)		; pulse width of voice 1
	MOVE.B	#$10,(SID1_LEFT,A0)	; left channel mix
	MOVE.B	#$ff,(SID1_RGHT,A0)	; right channel mix
	MOVE.B	#%01000001,($04,A0)	; pulse (bit 6) and open gate (bit 0)


mainloop

	; put something in the usp
	MOVEA.L	#$c00000,A0
	MOVE	A0,USP

	; copy keyboard state in to screen
.1	MOVEQ	#$0,D0
	MOVEA.L	VICV_TXT,A0
	LEA	$400(A0),A0
	LEA	CIA_BASE,A1
	LEA	$80(A1),A1
.2	MOVE.B	(A1,D0),(A0,D0)
	ADDQ	#$1,D0
	CMP.B	#$49,D0
	BNE	.2

	ADDQ.B	#$1,$00f00080
	BRA.S	.1


clear_screen
	MOVEM.L	D0-D1/A0-A2,-(A7)
	MOVEA.L	(VICV_TXT),A0
	MOVEA.L	(VICV_COL),A1
	MOVEA.L	A0,A2
	LEA	$800(A2),A2
	MOVE.L	#$20202020,D0
	MOVEQ	#$00,D1
	MOVE.B	CURR_TEXT_COLOR,D1
	LSL.L	#$8,D1
	MOVE.B	CURR_TEXT_COLOR,D1
	LSL.L	#$8,D1
	MOVE.B	CURR_TEXT_COLOR,D1
	LSL.L	#$8,D1
	MOVE.B	CURR_TEXT_COLOR,D1
.1	MOVE.L	D0,(A0)+
	MOVE.L	D1,(A1)+
	CMP.L	A0,A2
	BNE	.1
	MOVEM.L	(A7)+,D0-D1/A0-A2
	RTS


; put_char expects an ascii value in register D0

put_char

	MOVEM.L	D1-D2/A0-A2,-(A7)	; save registers
	MOVE.W	CURSOR_POS,D1		; load current cursor position into D1
	MOVE.B	CURR_TEXT_COLOR,D2	; load current text colour into D2
	MOVEA.L	VICV_TXT,A0		; load pointer to current text screen into A0
	MOVEA.L	VICV_COL,A1		; load pointer to current color screen into A1
	LEA	ascii_to_screencode,A2	; A2 now points to ascii-screencode table
	CMP.B	#ASCII_LF,D0		; do we have a line feed as the next ascii?
	beq	.1
	MOVE.B	(A2,D0),D0		; change the ascii value to a screencode value
	MOVE.B	D0,(A0,D1)
	MOVE.B	D2,(A1,D1)
	ADDQ	#$1,CURSOR_POS
	ANDI.W	#$07ff,CURSOR_POS
	MOVEM.L	(A7)+,D1-D2/A0-A2	; restore registers
	RTS
.1	ADDI.W	#$40,D1			; add 64 positions to current cursor pos
	ANDI.W	#%1111111111000000,D1	; move cursor pos to beginning of line
	MOVE.W	D1,CURSOR_POS		; store new value
	MOVEM.L	(A7)+,D1-D2/A0-A2	; restore registers
	RTS


; put_string expects a pointer to a string in a0

put_string

	MOVE.L	A0,-(A7)
.1	MOVE.B	(A0)+,D0	; move the first ascii value of string into D0
	CMP.B	#ASCII_NULL,D0	; did we reach the end of the string?
	BEQ	.2		; yes, go to end of function
	BSR	put_char	; no, print char
	BRA	.1
.2	MOVE.L	(A7)+,A0
	RTS


; fake exception handler

exception_handler

	MOVE.L	D0,-(A7)
	MOVE.L #$deadbeef,D0
	MOVE.L	(A7)+,D0
	RTE


; level 2 interrupt autovector (vicv vblank)

interrupt_2_autovector

	MOVE.B	#%00000001,VICV_ISR		; acknowledge VBLANK interrupt
	MOVE.B	#%00000001,VICV_BUFFERSWAP	; switch front- and backbuffer
	MOVE.W	#C64_BLUE,BLITTER_DATA_16_BIT	; load color blue in data register of blitter
	MOVE.B	#%00000001,BLITTER_CONTROL	; clear the backbuffer
						; plan other blitter stuff (text scr, ...)
	RTE


; level 4 interrupt autovector (timer)

interrupt_4_autovector

	MOVE.L	A0,-(A7)		; save a0
timer0_check
	BTST	#0,TIMER_BASE		; did timer 0 cause the interrupt?
	BEQ	timer1_check		; no, go to next timer
	MOVE.B	#%00000001,TIMER_BASE	; yes, acknowledge interrupt
	MOVEA.L	TIMER0_VECTOR,A0
	JMP	(A0)
timer1_check
	BTST	#1,TIMER_BASE
	BEQ	timer2_check
	MOVE.B	#%00000010,TIMER_BASE
	MOVEA.L	TIMER1_VECTOR,A0
	JMP	(A0)
timer2_check
	BTST	#2,TIMER_BASE		; did timer 2 cause the interrupt?
	BEQ	timer3_check		; no, go to next timer
	MOVE.B	#%00000100,TIMER_BASE	; yes, acknowledge interrupt
	MOVEA.L	TIMER2_VECTOR,A0
	JMP	(A0)
timer3_check
	BTST	#3,TIMER_BASE
	BEQ	timer_finish		; no, go to timer finish
	MOVE.B	#%00001000,TIMER_BASE	; yes, acknowledge
	MOVEA.L	TIMER3_VECTOR,A0
	JMP	(A0)
timer_finish
	MOVE.L	(A7)+,A0		; restore a0
	RTE


; level 5 interrupt autovector

interrupt_5_autovector

	MOVE.L	A0,-(A7)
	MOVE.L	(A7)+,A0
	RTE


; level 6 interrupt autovector

interrupt_6_autovector

	RTE


; level 7 interrupt autovector

interrupt_7_autovector

	RTE


timer0_handler

	MOVE.L	A0,-(A7)
	MOVEA.L	VICV_COL,A0
	ADDQ.B	#$1,(A0)
	ANDI.B	#%00001111,(A0)
	MOVEA.L	(A7)+,A0
	BRA	timer1_check


timer1_handler

	MOVE.L	A0,-(A7)
	MOVEA.L	VICV_COL,A0
	ADDQ.B	#$1,(1,A0)
	ANDI.B	#%00001111,(1,A0)
	MOVEA.L	(A7)+,A0
	BRA	timer2_check


timer2_handler

	;
	BRA	timer3_check


timer3_handler

	;
	ADDQ.B	#1,VICV_BORDER_SIZE
	;
	BRA	timer_finish


memcopy

	;
	;	bytewise memory copy - probably very slow...
	;
	;	Arguments
	;
	;	A0	source_start_address
	;	D0	no_of_bytes
	;	A1	destination_start_address
	;

	MOVE.L	D1,-(A7)
	MOVEQ	#$0,D1
.1	MOVE.B	(A0,D1.L),(A1,D1.L)
	ADDI.L	#$1,D1
	CMP.L	D1,D0
	BNE	.1

	MOVE.L	(A7)+,D1
	RTS


copy_charrom_to_charram

	;
	;	Register Usage
	;
	;	D0	current_byte, holds a byte from the original rom charset
	;	D1	i, counter from 7 to 0 (8 bits per byte have to be processed)
	;	A0	*char_ram, pointer
	;	A1	*char_rom, pointer
	;
	movem	d0-d1/a0-a1,-(a7)

	moveq	#0,d0			;	current_byte = 0;
	lea	CHAR_RAM,a0		;	char_ram = CHAR_RAM;
	lea	CHAR_ROM,a1		;	char_rom = CHAR_ROM;

.1	cmpa.l	#CHAR_ROM+$800,a1	;	while(char_ram != CHAR_ROM+$800)	//	if we're not at the end of char rom
	beq	.5			;	{								//	branch to end of compound statement
	move.b	(a1)+,d0		;		current_byte = char_rom++;		//	load a byte from charset and incr pntr
	moveq	#8,d1			;		i = 8;
.2	btst	#$7,d0
	beq	.3			; bit 7 not set
	move.w	#C64_GREY,(a0)+		; bit 7 is set, so set color
	bra	.4
.3	move.w	#$0000,(a0)+		; bit 7 not set, make empty
.4	lsl.b	#$01,d0			; move all the bits one place to the left
	subq	#$01,d1			;	i = i - 1;
	beq	.1			;	did i reach zero? goto .1
	bra	.2
					;	}
.5	movem	(a7)+,d0-d1/a0-a1	;
	rts				;

init_song

play_song_frame


; logo blit description
logo_blit_structure
	DC.B	%00000011	; multicolor and bitmap mode
	DC.B	%00000000
	DC.W	%00000011	; width 2^3 = 8 chars = 64 pixels
	DC.W	%00000000	; height 2^0 = 1 char =  8 pixels
	DC.W	$0010		; x_pos
	DC.W	$0010		; y_pos


; logo blit bitmap data

logo_bitmap
	DC.W	$0000,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$0000

	DC.W	$f444,$f444,$f733,$f733,$f733,$f733,$f733,$f733
	DC.W	$f733,$f733,$f733,$f733,$f733,$f733,$f733,$f733
	DC.W	$f733,$f733,$f444,$faaa,$faaa,$faaa,$faaa,$f444
	DC.W	$f444,$faaa,$faaa,$faaa,$f444,$f444,$f444,$f444
	DC.W	$faaa,$faaa,$f444,$f444,$f444,$f444,$faaa,$faaa
	DC.W	$faaa,$f444,$faaa,$faaa,$faaa,$f444,$f733,$f733
	DC.W	$f733,$f733,$f733,$f733,$f733,$f733,$f733,$f733
	DC.W	$f733,$f733,$f733,$f733,$f733,$f733,$f444,$f444

	DC.W	$f444,$f853,$f853,$f853,$f853,$f853,$f853,$f853
	DC.W	$f853,$f853,$f853,$f853,$f853,$f853,$f853,$f853
	DC.W	$f853,$f853,$f444,$faaa,$f444,$f444,$f444,$f444
	DC.W	$faaa,$f444,$f444,$f444,$f444,$f444,$f444,$faaa
	DC.W	$f444,$faaa,$f444,$f444,$f444,$f444,$f444,$faaa
	DC.W	$f444,$f444,$f444,$faaa,$f444,$f444,$f853,$f853
	DC.W	$f853,$f853,$f853,$f853,$f853,$f853,$f853,$f853
	DC.W	$f853,$f853,$f853,$f853,$f853,$f853,$f853,$f444

	DC.W	$f444,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8
	DC.W	$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8
	DC.W	$fee8,$fee8,$f444,$faaa,$faaa,$faaa,$f444,$f444
	DC.W	$faaa,$faaa,$faaa,$faaa,$f444,$f444,$faaa,$f444
	DC.W	$f444,$faaa,$f444,$faaa,$faaa,$f444,$f444,$faaa
	DC.W	$f444,$f444,$f444,$faaa,$f444,$f444,$fee8,$fee8
	DC.W	$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8
	DC.W	$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$fee8,$f444

	DC.W	$f444,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa
	DC.W	$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa
	DC.W	$fbfa,$fbfa,$f444,$faaa,$f444,$f444,$f444,$f444
	DC.W	$faaa,$f444,$f444,$f444,$faaa,$f444,$faaa,$faaa
	DC.W	$faaa,$faaa,$f444,$f444,$f444,$f444,$f444,$faaa
	DC.W	$f444,$f444,$f444,$faaa,$f444,$f444,$fbfa,$fbfa
	DC.W	$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa
	DC.W	$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$fbfa,$f444

	DC.W	$f444,$f444,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d
	DC.W	$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d
	DC.W	$f67d,$f67d,$f444,$faaa,$faaa,$faaa,$faaa,$f444
	DC.W	$f444,$faaa,$faaa,$faaa,$f444,$f444,$f444,$f444
	DC.W	$f444,$faaa,$f444,$f444,$f444,$f444,$faaa,$faaa
	DC.W	$faaa,$f444,$faaa,$faaa,$faaa,$f444,$f67d,$f67d
	DC.W	$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f67d
	DC.W	$f67d,$f67d,$f67d,$f67d,$f67d,$f67d,$f444,$f444

	DC.W	$0000,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$f444
	DC.W	$f444,$f444,$f444,$f444,$f444,$f444,$f444,$0000

	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000
	DC.W	$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000

; string data

welcome
	DC.B	"E64-II (C)2019-2020 kernel version 0.1.20200420",ASCII_LF,ASCII_NULL

	ALIGN	1

	INCLUDE	"E64-II_kernel_tables.asm"

	ORG	KERNEL_LOC+$fffc
	DC.L	$deadbeef