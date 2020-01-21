; E64 definitions - 2019-11-21 elmerucr

KERNEL_LOC	equ	$ff0000
VEC_04_ILLEGAL_INSTRUCTION		equ	$00000010
VEC_10_UNIMPL_INSTRUCTION		equ	$00000028
VEC_11_UNIMPL_INSTRUCTION		equ	$0000002c
VEC_26_LEVEL2_IRQ_AUTOVECT		equ	$00000068
VEC_27_LEVEL3_IRQ_AUTOVECT		equ	$0000006c
VEC_28_LEVEL4_IRQ_AUTOVECT		equ	$00000070
VEC_29_LEVEL5_IRQ_AUTOVECT		equ	$00000074
VEC_30_LEVEL6_IRQ_AUTOVECT		equ	$00000078
VEC_31_LEVEL7_IRQ_AUTOVECT		equ	$0000007c

; ascii definitions
ASCII_NULL	equ	$00
ASCII_LF	equ	$0a

; cia
CIA_BASE	equ	$fe0300

; timer
TIMER_BASE	equ $fe0600

; vicv
VICV_BASE	equ	$fe0400 	; vicv base register
VICV_TXT	equ VICV_BASE+$02
VICV_COL	equ VICV_BASE+$06

; sound
SOUND_BASE	equ $fe0500	; sound base register
SID0_BASE	equ	SOUND_BASE
SID1_BASE	equ	SOUND_BASE+$20
; sound indices
SID0_LEFT	equ $80
SID0_RGHT	equ $81
SID1_LEFT	equ $82
SID1_RGHT	equ	$83

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

CURR_TEXT_COLOR	equ $008000	; byte
CURSOR_POS		equ	$008002	; word
TIMER0_VECTOR	equ	$008004	; long
TIMER1_VECTOR	equ	$008008	; long
TIMER2_VECTOR	equ	$00800c	; long
TIMER3_VECTOR	equ	$008010	; long