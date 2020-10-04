; elmerucr - 01/08/2020
; compiles with vasmm68k_mot

	INCLUDE "kernel_definitions.s"

	ORG	$0

vec_00				DS.L	1	; vector 0 - supervisor stackpointer
vec_01				DS.L	1	; vector 1 - reset vector
vec_02				DS.L	1	; vector 2
vec_03				DS.L	1	; vector 3
vec_04_illegal_instruction	DS.L	1	; vector 4 - illegal instruction
vec_05				DS.L	1
vec_06				DS.L	1
vec_07				DS.L	1
vec_08				DS.L	1
vec_09				DS.L	1
vec_10_unimpl_instruction	DS.L	1	; vector 10
vec_11_unimpl_instruction	DS.L	1	; vector 11
vec_12				DS.L	1
vec_13				DS.L	1
vec_14				DS.L	1
vec_15				DS.L	1
vec_16				DS.L	1
vec_17				DS.L	1
vec_18				DS.L	1
vec_19				DS.L	1
vec_20				DS.L	1
vec_21				DS.L	1
vec_22				DS.L	1
vec_23				DS.L	1
vec_24				DS.L	1
vec_25				DS.L	1
vec_26_level2_irq_autovect	DS.L	1	; vector 26
vec_27_level3_irq_autovect	DS.L	1	; vector 27
vec_28_level4_irq_autovect	DS.L	1	; vector 28
vec_29_level5_irq_autovect	DS.L	1	; vector 29
vec_30_level6_irq_autovect	DS.L	1	; vector 30
vec_31_level7_irq_autovect	DS.L	1	; vector 31


	ORG	KERNEL_VARS

curr_text_color	DS.W	1
cursor_pos	DS.W	1
current_txt_scr	DS.L	1

timer0_vector	DS.L	1
timer1_vector	DS.L	1
timer2_vector	DS.L	1
timer3_vector	DS.L	1

	ALIGN	5
kernel_text_scr	DS.B	32

se_crs_blink	DS.B	1	; byte (actually least significant bit), 0=off, 1=currently blinking
se_crs_cntdwn	DS.B	1	; byte, counter for blinking interval
se_crs_interval	DS.B	1	; byte, duration of blinking. e.g. @60Hz value 20 means 0.33s on, 0.33s off
se_orig_char	DS.B	1	; byte, original value of the char behind the cursor
se_orig_color	DS.W	1	; word, original value of the color value behind the cursor
se_command_buf	DS.B	64	; 64 bytes, hold a string of max. 63 chars followed by a NULL


	ORG	KERNEL_LOC

	DC.L	SUPERV_STACK		; vector 0 - supervisor stackpointer
	DC.L	kernel_main		; vector 1 - reset vector


; start of main kernel code

kernel_main

	BSR	setup_vector_table
	BSR	reset_sids
	BSR	copy_charrom_to_charram

	; put something in the usp
	LEA	$C00000,A0
	MOVE.L	A0,USP

	; set up timer0 interrupts (cursor flashing)

	MOVE.W	#$E10,TIMER_BASE+2	; load value 3600 ($E10 = 3600bpm = 60Hz)
	ORI.B	#%00000001,TIMER_BASE+1	; turn on interrupt generation by clock0


	; set up timer1 interrupts (ting sound)

	;MOVE.W	#$3C,TIMER_BASE+2	; load value 60 ($3c = 60bpm = 1Hz)
	;ORI.B	#%00000010,TIMER_BASE+1	; turn on interrupt generation by clock0


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
	LEA	kernel_text_scr,a1
	MOVE.L	#$20,D0			; 32 bytes
	JSR	memcopy

	MOVE.L	#kernel_text_scr,current_txt_scr	; set current text screen


	; set txt & color pointer  -  deprecated!
	MOVE.L	#$00F00000,VICV_TXT
	MOVE.L	#$00F00800,VICV_COL

	MOVE.W	#C64_LIGHTBLUE,curr_text_color

	; play a welcome sound on SID0

	LEA	SID0_BASE,A0
	LEA	notes,A1
	MOVE.W	N_D3_(A1),(A0)		; set frequency of voice 1
	MOVE.B	#%00001001,$5(A0)	; attack and decay of voice 1
	MOVE.W	#$F0F,$02(A0)		; pulse width of voice 1
	MOVE.B	#$FF,SID0_LEFT		; left channel mix
	MOVE.B	#$10,SID0_RGHT		; right channel mix
	MOVE.B	#%01000001,$4(A0)	; pulse (bit 6) and open gate (bit 0)


	; play a welcome sound on SID1

	LEA	SID1_BASE,A0
	LEA	notes,A1
	MOVE.W	N_A3_(A1),(A0)		; set frequency of voice 1
	MOVE.B	#%00001001,$5(A0)	; attack and decay of voice 1
	MOVE.W	#$F0F,$2(A0)		; pulse width of voice 1
	MOVE.B	#$10,SID1_LEFT		; left channel mix
	MOVE.B	#$FF,SID1_RGHT		; right channel mix
	MOVE.B	#%01000001,$4(A0)	; pulse (bit 6) and open gate (bit 0)


mainloop

	LEA	.main0,A0
	MOVE.L	A0,-(SP)		; put effective address of .main0 onto stack
	MOVE.W	#$0100,-(SP)		; put new Status Reg onto stack
	RTE				; jump to user mode



.main0	BSR	se_clear_screen
	MOVE.W	#$0,cursor_pos		; reset cursor position
	MOVE.B	#$14,se_crs_interval	; blinking interval at 20 (0.33s)
	LEA	welcome,A0
	BSR	put_string
	LEA	.prompt,A0
	BSR	put_string
	MOVE.B	#ASCII_LF,D0
	BSR	put_char

	BSR	se_activate_cursor


.main1	CLR.L	D0
	MOVE.B	CIA_ASCII,D0		; scan for a keyboard event/ascii value
	BEQ.S	.main1			; if 0 (nothing), jump to .main1

	BSR	se_deactivate_cursor

	CMP.B	#ASCII_LF,D0		; did we have a return as keypress?
	BNE	.main2			; no

	LEA	.mes2,A0		; yes, process command
	; extract string here...
	BSR	put_string
	LEA	.prompt,A0
	BSR	put_string

.main2	BSR	put_char		; process input
	BSR	se_activate_cursor

	BRA.S	.main1

.prompt	DC.B	"ready.",ASCII_NULL
.mes2	DC.B	ASCII_LF,ASCII_LF,"error: illegal command",ASCII_LF,ASCII_NULL


	ALIGN	1
se_clear_screen

	MOVEM.L	D0-D1/A0,-(SP)

	MOVEA.L	(VICV_TXT),A0
	MOVE.L	#$800,D0
	MOVE.B	#SCRN_SPACE,D1			; space screencode
	JSR	blockfill_bytes

	MOVEA.L	(VICV_COL),A0
	MOVE.L	#$800,D0
	MOVE.W	#C64_LIGHTBLUE,D1
	JSR	blockfill_words

	MOVEM.L	(SP)+,D0-D1/A0
	RTS


se_scroll_up

	MOVEM.L	D0/A0-A1,-(SP)

	MOVE.L	#$7C0,D0

	MOVEA.L	(VICV_TXT),A0
	MOVEA.L	(VICV_COL),A1

.1	MOVE.B	$40(A0),(A0)
	MOVE.W	$80(A1),(A1)
	ADDA	#$1,A0
	ADDA	#$2,A1
	SUBQ	#$1,D0
	BNE	.1

.2	MOVE.B	#ASCII_SPACE,(A0)
	MOVE.W	curr_text_color,(A1)
	ADDA	#$1,A0
	ADDA	#$2,A1
	ADDQ	#$1,D0
	CMPI.W	#$40,D0
	BNE	.2

	MOVEM.L	(SP)+,D0/A0-A1
	RTS

; put_char expects an ascii value in register D0

put_char

	MOVEM.L	D0-D3/A0-A2,-(SP)	; save registers
	ANDI.W	#$00FF,D0		; clear bits 8-15 from D0
	MOVE.W	cursor_pos,D1		; load current cursor position into D1
	MOVE.W	curr_text_color,D2	; load current text colour into D2
	MOVEA.L	VICV_TXT,A0		; load pointer to current text screen into A0
	MOVEA.L	VICV_COL,A1		; load pointer to current color screen into A1
	LEA	ascii_to_screencode,A2	; A2 now points to ascii-screencode table
	CMP.B	#ASCII_LF,D0		; do we have a line feed as the next ascii?
	BEQ	.lf
	CMP.B	#ASCII_CURSOR_DOWN,D0
	BEQ	.down
	CMP.B	#ASCII_CURSOR_RIGHT,D0
	BEQ	.right
	CMP.B	#ASCII_CURSOR_UP,D0
	BEQ	.up
	CMP.B	#ASCII_CURSOR_LEFT,D0
	BEQ	.left
	CMP.B	#ASCII_BACKSPACE,D0
	BEQ	.bs

	; it's not a control character so print it
.char	MOVE.B	(A2,D0),D0		; change ascii value into screencode value
	MOVE.B	D0,(A0,D1)		; copy the char into screen
	MOVE.W	D1,D3			; copy cursor position into D3
	LSL.W	#$1,D3			; multiply index by two (color values are words contrary to tiles)
	MOVE.W	D2,(A1,D3)		; copy the color value
	ADDQ	#$1,D1			; increase the cursor position by one

	MOVE.W	D1,D3
	ANDI.W	#$F800,D3		; are we outside screen memory?
	BEQ	.char2			; no, go to .char2
	JSR	se_scroll_up		; yes, scroll 1 row upwards
	SUBI.W	#$40,D1			; subtract 64 positions from the cursor position
.char2	MOVE.W	D1,cursor_pos
	BRA	.end

.lf	ADDI.W	#$40,D1			; line feed, add 64 positions to current cursor pos
	ANDI.W	#%1111111111000000,D1	; move cursor pos to beginning of line
	MOVE.W	D1,D3
	ANDI.W	#$F800,D3		; are we outside screen memory?
	BEQ	.lf2			; no, go to .lf2
	JSR	se_scroll_up		; yes, scroll 1 row upwards
	SUBI.W	#$40,D1			; subtract 64 positions from the cursor position
.lf2	MOVE.W	D1,cursor_pos		; store new value
	BRA	.end

.down	ADDI.W	#$40,D1			; cursor down, add 64 positions to current cursor pos
	MOVE.W	D1,D2
	ANDI.W	#$F800,D2
	BEQ	.down2
	BSR	se_scroll_up
	BRA	.end
.down2	MOVE.W	D1,cursor_pos		; store new value
	BRA	.end

.right	ADDI.W	#$1,D1			; cursor right
	;ANDI.W	#$7FF,D1
	MOVE.W	D1,D2
	ANDI.W	#$F800,D2
	BEQ	.right2
	BSR	se_scroll_up
	SUBI.W	#$40,D1
.right2	MOVE.W	D1,cursor_pos
	BRA	.end

.up	SUBI.W	#$40,D1			; cursor up
	BMI	.end			; stop if cursor out of screen, don't store position
	ANDI.W	#$7FF,D1
	MOVE.W	D1,cursor_pos
	BRA	.end

.left	SUBI.W	#$1,D1			; cursor left
	BMI	.end			; stop if cursor out of screen, don't store position
	ANDI.W	#$7FF,D1
	MOVE.W	D1,cursor_pos
	BRA	.end

.bs	SUBI.W	#$1,D1			; backspace
	BMI	.end			; stop if cursor out of screen, don't store position
	ANDI.W	#$7FF,D1
	MOVE.W	D1,cursor_pos		; store the new cursor position
.bs1	MOVE.W	D1,D3
	ADDQ	#$1,D3
	ANDI.W	#%0000000000111111,D3	; are we at positon $3f?
	BNE	.bs2			; not yet
	MOVE.B	#ASCII_SPACE,(A0,D1)	; yes, place a space character
	BRA	.end
.bs2	MOVE.B	$1(A0,D1),(A0,D1)
	ADDQ	#$1,D1
	BRA	.bs1

.end	MOVEM.L	(SP)+,D0-D3/A0-A2	; restore registers, including original ascii in D0
	RTS


put_string

	;
	; put_string expects a pointer to a string in A0
	;

	MOVEM.L	A0/D0,-(SP)

.start	MOVE.B	(A0)+,D0	; move ascii value into D0, and move pointer to next char
	BEQ	.end
	BSR	put_char	; no, put char
	BRA	.start

.end	MOVEM.L	(SP)+,A0/D0
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
	MOVE.L	#kernel_text_scr,BLITTER_DATA_32_BIT
	MOVE.B	#%00000010,BLITTER_CONTROL

	RTE


; level 4 interrupt autovector (timer)

interrupt_4_autovector

	MOVE.L	A0,-(SP)		; save a0
timer0_check
	BTST	#0,TIMER_BASE		; did timer 0 cause the interrupt?
	BEQ	timer1_check		; no, go to next timer
	MOVE.B	#%00000001,TIMER_BASE	; yes, acknowledge interrupt
	MOVEA.L	timer0_vector,A0
	JMP	(A0)
timer1_check
	BTST	#1,TIMER_BASE
	BEQ	timer2_check
	MOVE.B	#%00000010,TIMER_BASE
	MOVEA.L	timer1_vector,A0
	JMP	(A0)
timer2_check
	BTST	#2,TIMER_BASE		; did timer 2 cause the interrupt?
	BEQ	timer3_check		; no, go to next timer
	MOVE.B	#%00000100,TIMER_BASE	; yes, acknowledge interrupt
	MOVEA.L	timer2_vector,A0
	JMP	(A0)
timer3_check
	BTST	#3,TIMER_BASE
	BEQ	timer_finish		; no, go to timer finish
	MOVE.B	#%00001000,TIMER_BASE	; yes, acknowledge
	MOVEA.L	timer3_vector,A0
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

	; cursor flash
	MOVEM.L	D0/A0,-(SP)

	BTST.B	#$0,se_crs_blink
	BEQ	.end

	MOVEA.L	VICV_TXT,A0		; load pointer to current text screen into A0
	MOVE.W	cursor_pos,D0
	SUBQ.B	#$1,se_crs_cntdwn
	BNE	.end
	EORI.B	#%10000000,(A0,D0)
	MOVE.B	se_crs_interval,se_crs_cntdwn

.end	MOVEM.L	(SP)+,D0/A0
	BRA	timer1_check


timer1_handler

	;
	BRA	timer2_check


timer2_handler

	;
	BRA	timer3_check


timer3_handler

	;
	BRA	timer_finish


blockfill_bytes

	;
	;	Arguments
	;
	;	A0	start address
	;	D0	number of bytes
	;	D1	byte value
	;

	MOVE.L	D2,-(SP)	; save D2

	MOVEQ	#$0,D2

.1	MOVE.B	D1,(A0,D2.L)
	ADDQ	#$1,D2
	CMP.L	D2,D0
	BNE	.1

	MOVE.L	(SP)+,D2
	RTS

blockfill_words

	;
	;	Arguments
	;
	;	A0	start address
	;	D0	number of words
	;	D1	word value
	;

	MOVEM.L	D0/D2,-(SP)

	MOVEQ	#$0,D2
	LSL.L	#$1,D0

.1	MOVE.W	D1,(A0,D2.L)
	ADDQ.L	#$2,D2
	CMP.L	D2,D0
	BNE	.1

	MOVEM.L	(SP)+,D0/D2
	RTS

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
	MOVE.L	A0,vec_04_illegal_instruction
	MOVE.L	A0,vec_10_unimpl_instruction
	MOVE.L	A0,vec_11_unimpl_instruction

	LEA	interrupt_2_autovector,A0
	MOVE.L	A0,vec_26_level2_irq_autovect

	LEA	interrupt_4_autovector,A0
	MOVE.L	A0,vec_28_level4_irq_autovect

	LEA	interrupt_5_autovector,A0
	MOVE.L	A0,vec_29_level5_irq_autovect

	LEA	interrupt_6_autovector,A0
	MOVE.L	A0,vec_30_level6_irq_autovect

	LEA	interrupt_7_autovector,A0
	MOVE.L	A0,vec_31_level7_irq_autovect

	LEA	timer0_handler,A0
	MOVE.L	A0,timer0_vector

	LEA	timer1_handler,A0
	MOVE.L	A0,timer1_vector

	LEA	timer2_handler,A0
	MOVE.L	A0,timer2_vector

	LEA	timer3_handler,A0
	MOVE.L	A0,timer3_vector

	RTS


reset_sids

	MOVEM.L	D0-D1/A0,-(SP)

	LEA	SOUND_BASE,A0
	MOVE.L	#$100,D0
	MOVE.B	#$00,D1
	JSR	blockfill_bytes

	; max volume for both sids
	MOVE.B	#$0F,SID0_VOLUME
	MOVE.B	#$0F,SID1_VOLUME

	LEA	SOUND_BASE,A0
	MOVE.B	#$FF,D0
	MOVE.B	D0,SID0_LEFT		; left channel mix
	MOVE.B	D0,SID0_RGHT		; right channel mix
	MOVE.B	D0,SID1_LEFT		; left channel mix
	MOVE.B	D0,SID1_RGHT		; right channel mix

	MOVEM.L	(SP)+,D0-D1/A0

	RTS


se_activate_cursor
	MOVEM.L	D0/A0-A1,-(SP)

	MOVEA.L	VICV_TXT,A0		; load pointer to current text screen into A0
	MOVEA.L	VICV_COL,A1
	MOVE.W	cursor_pos,D0
	MOVE.B	(A0,D0),se_orig_char
	EORI.B	#%10000000,(A0,D0)
	MOVE.B	se_crs_interval,se_crs_cntdwn
	MOVE.B	#$1,se_crs_blink	; turn on cursor flash

	MOVEM.L	(SP)+,D0/A0-A1
	RTS

se_deactivate_cursor
	MOVEM.L	D0/A0-A1,-(SP)

	MOVE.B	#$0,se_crs_blink	; turn off cursor flash
	MOVEA.L	VICV_TXT,A0		; load pointer to current text screen into A0
	MOVEA.L	VICV_COL,A1
	MOVE.W	cursor_pos,D0
	MOVE.B	se_orig_char,(A0,D0)

	MOVEM.L	(SP)+,D0/A0-A1
	RTS

; kernel text screen blit desciption (rom description, copied to kernel ram area, also 32 byte aligned)

	ALIGN	5
screen_blit_structure
	DC.B	%00001000	; flags 0 - tile mode, simple color, color per tile
	DC.B	%00000000	; flags 1 - no stretching, mirroring etc
	DC.B	%01010110	; height 2^%101 = 32 chars = 256 pixels, width 2^%110 = 64 chars  = 512 pixels
	DC.B	%00000000	; currently unused.... :-)
	DC.W	$0		; x (0)
	DC.W	$10		; y (16)
	DC.W	$F0A0		; foreground color
	DC.W	$F222		; background color
	DC.L	CHAR_RAM	; pixel_data
	DC.L	$F00000		; character_data
	DC.L	$F00800		; character_color_data
	DC.L	$FF0000		; background_color_data
	DC.L	$0		; user_data


; string data

welcome
	DC.B	"E64-II (C)2019-2020 kernel 0.2.20200915",ASCII_LF,ASCII_LF,ASCII_NULL

	ALIGN	1

	INCLUDE	"kernel_tables.s"

	ORG	KERNEL_LOC+$FFFC
	DC.L	$DEADBEEF