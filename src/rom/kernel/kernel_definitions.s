; E64 definitions - 2019-11-21 elmerucr

KERNEL_LOC	equ	$FF0000
SUPERV_STACK	equ	$E00000

VEC_04_ILLEGAL_INSTRUCTION		equ	$00000010
VEC_10_UNIMPL_INSTRUCTION		equ	$00000028
VEC_11_UNIMPL_INSTRUCTION		equ	$0000002C
VEC_26_LEVEL2_IRQ_AUTOVECT		equ	$00000068
VEC_27_LEVEL3_IRQ_AUTOVECT		equ	$0000006c
VEC_28_LEVEL4_IRQ_AUTOVECT		equ	$00000070
VEC_29_LEVEL5_IRQ_AUTOVECT		equ	$00000074
VEC_30_LEVEL6_IRQ_AUTOVECT		equ	$00000078
VEC_31_LEVEL7_IRQ_AUTOVECT		equ	$0000007C

; ascii definitions
ASCII_NULL		equ	$00
ASCII_BACKSPACE		equ	$08
ASCII_LF		equ	$0A
ASCII_CURSOR_DOWN	equ	$11
ASCII_CURSOR_RIGHT	equ	$1D
ASCII_SPACE		equ	$20
ASCII_CURSOR_UP		equ	$91
ASCII_CURSOR_LEFT	equ	$9D

; scrn_codes
SCRN_SPACE		equ	$20

; char rom
CHAR_ROM		equ	$FD0000
CHAR_RAM		equ	$F10000

; cia
CIA_BASE		equ	$FE0300
CIA_STATUS		equ	CIA_BASE
CIA_CONTROL		equ	CIA_BASE+$01
CIA_DELAY		equ	CIA_BASE+$02
CIA_REPEAT		equ	CIA_BASE+$03
CIA_ASCII		equ	CIA_BASE+$04
CIA_KEYBOARD		equ	CIA_BASE+$80

; vicv
VICV_BASE		equ	$FE0400 	; vicv base register
VICV_BORDER_COLOR	equ	VICV_BASE	; 16 bit value

VICV_TXT		equ	VICV_BASE+$04
VICV_COL		equ	VICV_BASE+$08
VICV_BORDER_SIZE	equ	VICV_BASE+$0C
VICV_ISR		equ	VICV_BASE+$0E
VICV_BUFFERSWAP		equ	VICV_BASE+$10

; timer
TIMER_BASE		equ	$FE0600

; blitter
BLITTER_BASE		equ	$FE0700
BLITTER_CONTROL		equ	BLITTER_BASE
BLITTER_DATA_32_BIT	equ	BLITTER_BASE+$02
BLITTER_CLEAR_COLOR	equ	BLITTER_BASE+$06	; 16 bit word

; color palette
C64_BLACK	equ	$F000
C64_WHITE	equ	$FFFF
C64_RED		equ	$F733
C64_CYAN	equ	$F8CC
C64_PURPLE	equ	$F849
C64_GREEN	equ	$F6A5
C64_BLUE	equ	$F339
C64_YELLOW	equ	$FEE8
C64_ORANGE	equ	$F853
C64_BROWN	equ	$F531
C64_LIGHTRED	equ	$FB77
C64_DARKGREY	equ	$F444
C64_GREY	equ	$F777
C64_LIGHTGREEN	equ	$FBFA
C64_LIGHTBLUE	equ	$F67D
C64_LIGHTGREY	equ	$FAAA

; SOUND
SOUND_BASE	equ	$FE0500			; base register
SID0_BASE	equ	SOUND_BASE
SID0_VOLUME	equ	SID0_BASE+$18
SID1_BASE	equ	SOUND_BASE+$20
SID1_VOLUME	equ	SID1_BASE+$18

; SOUND MIXING
SID0_LEFT	equ	SOUND_BASE+$80
SID0_RGHT	equ	SOUND_BASE+$81
SID1_LEFT	equ	SOUND_BASE+$82
SID1_RGHT	equ	SOUND_BASE+$83


N_C0_	equ	00*2
N_C0S	equ	01*2
N_D0_	equ	02*2
N_D0S	equ	03*2
N_E0_	equ	04*2
N_F0_	equ	05*2
N_F0S	equ	06*2
N_G0_	equ	07*2
N_G0S	equ	08*2
N_A0_	equ	09*2
N_A0S	equ	10*2
N_B0_	equ	11*2

N_C1_	equ	12*2
N_C1S	equ	13*2
N_D1_	equ	14*2
N_D1S	equ	15*2
N_E1_	equ	16*2
N_F1_	equ	17*2
N_F1S	equ	18*2
N_G1_	equ	19*2
N_G1S	equ	20*2
N_A1_	equ	21*2
N_A1S	equ	22*2
N_B1_	equ	23*2

N_C2_	equ	24*2
N_C2S	equ	25*2
N_D2_	equ	26*2
N_D2S	equ	27*2
N_E2_	equ	28*2
N_F2_	equ	29*2
N_F2S	equ	30*2
N_G2_	equ	31*2
N_G2S	equ	32*2
N_A2_	equ	33*2
N_A2S	equ	34*2
N_B2_	equ	35*2

N_C3_	equ	36*2
N_C3S	equ	37*2
N_D3_	equ	38*2
N_D3S	equ	39*2
N_E3_	equ	40*2
N_F3_	equ	41*2
N_F3S	equ	42*2
N_G3_	equ	43*2
N_G3S	equ	44*2
N_A3_	equ	45*2
N_A3S	equ	46*2
N_B3_	equ	47*2

N_C4_	equ	48*2
N_C4S	equ	49*2
N_D4_	equ	50*2
N_D4S	equ	51*2
N_E4_	equ	52*2
N_F4_	equ	53*2
N_F4S	equ	54*2
N_G4_	equ	55*2
N_G4S	equ	56*2
N_A4_	equ	57*2
N_A4S	equ	58*2
N_B4_	equ	59*2

N_C5_	equ	60*2
N_C5S	equ	61*2
N_D5_	equ	62*2
N_D5S	equ	63*2
N_E5_	equ	64*2
N_F5_	equ	65*2
N_F5S	equ	66*2
N_G5_	equ	67*2
N_G5S	equ	68*2
N_A5_	equ	69*2
N_A5S	equ	70*2
N_B5_	equ	71*2

N_C6_	equ	72*2
N_C6S	equ	73*2
N_D6_	equ	74*2
N_D6S	equ	75*2
N_E6_	equ	76*2
N_F6_	equ	77*2
N_F6S	equ	78*2
N_G6_	equ	79*2
N_G6S	equ	80*2
N_A6_	equ	81*2
N_A6S	equ	82*2
N_B6_	equ	83*2

N_C7_	equ	84*2
N_C7S	equ	85*2
N_D7_	equ	86*2
N_D7S	equ	87*2
N_E7_	equ	88*2
N_F7_	equ	89*2
N_F7S	equ	90*2
N_G7_	equ	91*2
N_G7S	equ	92*2
N_A7_	equ	93*2
N_A7S	equ	94*2


; ram locations

CURR_TEXT_COLOR	equ	$008000	; word
CURSOR_POS	equ	$008002	; word

TIMER0_VECTOR	equ	$008004	; long
TIMER1_VECTOR	equ	$008008	; long
TIMER2_VECTOR	equ	$00800C	; long
TIMER3_VECTOR	equ	$008010	; long

CURRENT_TXT_SCR	equ	$008014	; long

KERNEL_TEXT_SCR	equ	$008020 ; 32 bytes (up to and incl $00805F), and 32 byte aligned
LOGO_BLIT	equ	$008040 ; 32 bytes (up to and incl $00807F), and 32 byte aligned

; screen editor variables
SE_CRS_BLINK	equ	$008060	; byte (actually least significant bit), 0=off, 1=currently blinking
SE_CRS_CNTDWN	equ	$008061	; byte, counter for blinking interval
SE_CRS_INTERVAL	equ	$008062	; byte, duration of blinking. e.g. @60Hz value 20 means 0.33s on, 0.33s off
SE_ORIG_CHAR	equ	$008063	; byte, original value of the char behind the cursor
SE_ORIG_COLOR	equ	$008084	; word, original value of the color value behind the cursor