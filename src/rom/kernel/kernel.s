; elmerucr - 16/06/2020
; compiles with vasmm68k_mot

	INCLUDE "kernel_definitions.s"

	ORG	KERNEL_LOC

	DC.L	$00E00000		; vector 0 - supervisor stackpointer
	DC.L	kernel_main		; vector 1 - reset vector


; start of main kernel code

kernel_main

	BSR	setup_vector_table
	BSR	reset_sids
	BSR	copy_charrom_to_charram


	; set up timer0 interrupts (cursor flashing)

	MOVE.W	#$E10,TIMER_BASE+2	; load value 3600 ($E10 = 3600bpm = 60Hz)
	ORI.B	#%00000001,TIMER_BASE+1	; turn on interrupt generation by clock0


	; set up timer1 interrupts (ting sound)

	MOVE.W	#$3C,TIMER_BASE+2	; load value 60 ($3c = 60bpm = 1Hz)
	ORI.B	#%00000010,TIMER_BASE+1	; turn on interrupt generation by clock0


	; set up timer3 interrupts at 50.125Hz for music / sid tunes

	;MOVE.W	#$BC0,TIMER_BASE+2		; 3008bpm (=50.125Hz)
	;ORI.B	#%00001000,TIMER_BASE+1	; turn on interrupt generation by clock3


	; set ipl to level 1 (all interrupts levels of >= 2 will be acknowledged)

	MOVE.W	SR,D0
	ANDI.W	#%1111100011111111,D0
	ORI.W	#%0000000100000000,D0
	MOVE.W	D0,SR

	; CIA start generating keyboard events
	MOVE.B	#%00000001,CIA_CONTROL


	; set color and size of border
	MOVE.W	#C64_BLACK,VICV_BORDER_COLOR
	MOVE.B	#$10,VICV_BORDER_SIZE

	; set clear color ('background')
	MOVE.W	#C64_BLUE,BLITTER_CLEAR_COLOR

	; copy the screen blit struct from rom to appropriate ram area
	LEA	screen_blit_structure,a0
	LEA	KERNEL_TEXT_SCR,a1
	MOVE.L	#$20,D0
	JSR	memcopy

	MOVE.L	#KERNEL_TEXT_SCR,CURRENT_TXT_SCR	; set current text screen


	; set txt pointer  -  deprecated!
	MOVE.L	#$00F00000,VICV_TXT
	MOVE.L	#$00F00800,VICV_COL


	; reset cursor position  -  deprecated
	MOVE.W	#$0,CURSOR_POS


	; clear screen and print welcome

	BSR	clear_screen
	LEA	welcome,A0
	BSR	put_string


	; play a welcome sound on SID0

	LEA	SID0_BASE,A0
	LEA	notes,A1
	MOVE.W	(N_D3_,A1),(A0)		; set frequency of voice 1
	MOVE.B	#%00001001,($5,A0)	; attack and decay of voice 1
	MOVE.W	#$F0F,($02,A0)		; pulse width of voice 1
	MOVE.B	#$FF,(SID0_LEFT,A0)	; left channel mix
	MOVE.B	#$10,(SID0_RGHT,A0)	; right channel mix
	MOVE.B	#%01000001,($4,A0)	; pulse (bit 6) and open gate (bit 0)


	; play a welcome sound on SID1

	LEA	SID1_BASE,A0
	LEA	notes,A1
	MOVE.W	(N_A3_,A1),(A0)		; set frequency of voice 1
	MOVE.B	#%00001001,($5,A0)	; attack and decay of voice 1
	MOVE.W	#$F0F,($2,A0)		; pulse width of voice 1
	MOVE.B	#$10,(SID1_LEFT,A0)	; left channel mix
	MOVE.B	#$FF,(SID1_RGHT,A0)	; right channel mix
	MOVE.B	#%01000001,($4,A0)	; pulse (bit 6) and open gate (bit 0)


mainloop

	; put something in the usp
	LEA	$C00000,A0
	MOVE	A0,USP

	; copy keyboard state in to screen
.1	MOVEQ	#$0,D0
	MOVEA.L	VICV_TXT,A0
	LEA	($400,A0),A0
	LEA	CIA_KEYBOARD,A1
.2	MOVE.B	(A1,D0),(A0,D0)
	ADDQ	#$1,D0
	CMP.B	#$49,D0
	BNE	.2

	MOVE.B	CIA_ASCII,D0		; scan for a keyboard event/ascii
	BEQ.S	.1			; if 0 (nothing), jump to .1
	JSR	put_char		; print character

	BRA.S	.1


clear_screen

	MOVEM.L	D0-D1/A0-A2,-(SP)
	MOVEA.L	(VICV_TXT),A0
	MOVEA.L	(VICV_COL),A1
	MOVEA.L	A0,A2
	LEA	($800,A2),A2
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
	MOVEM.L	(SP)+,D0-D1/A0-A2
	RTS


; put_char expects an ascii value in register D0

put_char

	MOVEM.L	D1-D2/A0-A2,-(SP)	; save registers
	ANDI.W	#$FF,D0			; clear bits 8-15 from D0
	MOVE.W	CURSOR_POS,D1		; load current cursor position into D1
	MOVE.B	CURR_TEXT_COLOR,D2	; load current text colour into D2
	MOVEA.L	VICV_TXT,A0		; load pointer to current text screen into A0
	MOVEA.L	VICV_COL,A1		; load pointer to current color screen into A1
	LEA	ascii_to_screencode,A2	; A2 now points to ascii-screencode table
	CMP.B	#ASCII_LF,D0		; do we have a line feed as the next ascii?
	BEQ	.1
	MOVE.B	(A2,D0),D0		; change the ascii value to a screencode value
	MOVE.B	D0,(A0,D1)
	MOVE.B	D2,(A1,D1)
	ADDQ	#$1,CURSOR_POS
	ANDI.W	#$7FF,CURSOR_POS
	MOVEM.L	(SP)+,D1-D2/A0-A2	; restore registers
	RTS
.1	ADDI.W	#$40,D1			; add 64 positions to current cursor pos
	ANDI.W	#%1111111111000000,D1	; move cursor pos to beginning of line
	MOVE.W	D1,CURSOR_POS		; store new value
	MOVEM.L	(SP)+,D1-D2/A0-A2	; restore registers
	RTS



put_string

	;
	; put_string expects a pointer to a string in A0
	;

	MOVE.L	A0,-(SP)
.1	MOVE.B	(A0)+,D0	; move the first ascii value of string into D0
	CMP.B	#ASCII_NULL,D0	; did we reach the end of the string?
	BEQ	.2		; yes, go to end of function
	BSR	put_char	; no, put char
	BRA	.1
.2	MOVE.L	(SP)+,A0
	RTS


; fake exception handler

exception_handler

	MOVE.L	D0,-(SP)
	MOVE.L	#$DEADBEEF,D0
	MOVE.L	(SP)+,D0
	RTE


; level 2 interrupt autovector (vicv start of vblank)

interrupt_2_autovector

	MOVE.B	#%00000001,VICV_ISR				; acknowledge VBLANK interrupt

	MOVE.B	#%00000001,VICV_BUFFERSWAP			; switch front- and backbuffer

	MOVE.B	#%00000001,BLITTER_CONTROL			; clear the backbuffer

	; add the blits (to be replaced by a kernel linked list)
	MOVE.L	#KERNEL_TEXT_SCR,BLITTER_DATA_32_BIT
	MOVE.B	#%00000010,BLITTER_CONTROL

	RTE


; level 4 interrupt autovector (timer)

interrupt_4_autovector

	MOVE.L	A0,-(SP)		; save a0
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
	MOVE.L	(SP)+,A0		; restore a0
	RTE


; level 5 interrupt autovector

interrupt_5_autovector

	MOVE.L	A0,-(SP)
	MOVE.L	(SP)+,A0
	RTE


; level 6 interrupt autovector

interrupt_6_autovector

	RTE


; level 7 interrupt autovector

interrupt_7_autovector

	RTE


timer0_handler

	;
	MOVEA.L	VICV_TXT,A0
	ADDQ.B	#1,(A0)

	BRA	timer1_check


timer1_handler

	LEA	SID0_BASE,A0
	MOVE.B	#%00100000,$4(A0)
	ORI.B	#%00100001,$4(A0)	; pulse (bit 6) and open gate (bit 0)

	BRA	timer2_check


timer2_handler

	;
	BRA	timer3_check


timer3_handler

	;
	BRA	timer_finish


memcopy

	;
	;	bytewise memory copy - probably slow?
	;
	;	Arguments
	;
	;	A0	source_start_address
	;	D0	no_of_bytes
	;	A1	destination_start_address
	;

	MOVE.L	D1,-(SP)
	MOVEQ	#$0,D1
.1	MOVE.B	(A0,D1.L),(A1,D1.L)
	ADDQ.L	#$1,D1
	CMP.L	D1,D0
	BNE	.1

	MOVE.L	(SP)+,D1
	RTS


copy_charrom_to_charram

	; Copy char rom to ram (go from 2k to 32k)
	; Note: this is a very special copy routine
	; that expands a charset from 1 bit into 16 bit
	; format.
	;
	;	Register Usage
	;
	;	D0	current_byte, holds a byte from the original rom charset
	;	D1	i, counter from 7 to 0 (8 bits per byte have to be processed)
	;	A0	*char_ram, pointer
	;	A1	*char_rom, pointer
	;
	MOVEM.L	D0-D1/A0-A1,-(SP)

	MOVEQ	#0,D0			;    current_byte = 0;
	LEA	CHAR_RAM,A0		;    char_ram = CHAR_RAM;
	LEA	CHAR_ROM,A1		;    char_rom = CHAR_ROM;

.1	CMPA.L	#CHAR_ROM+$800,A1	;    while(char_ram != CHAR_ROM+$800)
	BEQ	.5			;    {   //	branch to end of compound statement
					;        // load a byte from charset and incr pntr
	MOVE.B	(A1)+,D0		;        current_byte = char_rom++;
	MOVEQ	#8,D1			;        i = 8;
.2	BTST	#$7,D0
	BEQ	.3			;    bit 7 not set
	MOVE.W	#C64_GREY,(A0)+		;    bit 7 is set, so set color
	BRA	.4
.3	MOVE.W	#$0000,(A0)+		;    bit 7 not set, make empty
.4	LSL.B	#$01,D0			;    move all the bits one place to the left
	SUBQ	#$01,D1			;    i = i - 1;
	BEQ	.1			;    did i reach zero? goto .1
	BRA	.2
					;    }
.5	MOVEM.L	(SP)+,D0-D1/A0-A1
	RTS


setup_vector_table

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

	RTS

reset_sids
	; very basic, needs work
	; max volume for both sids
	LEA	SID0_BASE,A0
	MOVE.B	#$0F,$18(A0)
	LEA	SID1_BASE,A0
	MOVE.B	#$0F,$18(A0)
	RTS


; kernel text screen blit desciption (rom description, copied to kernel ram area, also 32 byte aligned)

	ALIGN	5
screen_blit_structure
	DC.B	%00000100	; flags 0 - tile mode, multicolor
	DC.B	%00000000	; flags 1 - no stretching, mirroring etc
	DC.B	%01010110	; height 2^%101 = 32 chars = 256 pixels, width 2^%110 = 64 chars  = 512 pixels
	DC.B	%00000000	; currently unused.... :-)
	DC.W	$0		; x (0)
	DC.W	$10		; y (16)
	DC.W	$F0A0		; foreground color
	DC.W	$F222		; background color
	DC.L	CHAR_RAM	; pixel_data
	DC.L	$F00000		; character_data
	DC.L	$FF0000		; character_color_data
	DC.L	$FF0000		; background_color_data
	DC.L	$0		; user_data


; string data

welcome
	DC.B	"E64-II (C)2019-2020 kernel 0.1.20200715",ASCII_LF,ASCII_NULL

	ALIGN	1

	INCLUDE	"kernel_tables.s"

	ORG	KERNEL_LOC+$FFFC
	DC.L	$DEADBEEF