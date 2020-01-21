; rom tables

ascii_to_screencode
	dc.b	$80		; 0x00 - NUL	non printable character
	dc.b	$80		; 0x01 - SOH	non printable character
	dc.b	$80		; 0x02 - STX	non printable character
	dc.b	$80		; 0x03 - ETX	non printable character
	dc.b	$80		; 0x04 - EOT	non printable character
	dc.b	$80		; 0x05 - ENQ	non printable character
	dc.b	$80		; 0x06 - ACK	non printable character
	dc.b	$80		; 0x07 - BEL	non printable character
	dc.b	$80		; 0x08 - BS	non printable character
	dc.b	$80		; 0x09 - TAB	non printable character
	dc.b	$80		; 0x0a - LF	non printable character
	dc.b	$80		; 0x0b - VT	non printable character
	dc.b	$80		; 0x0c - FF	non printable character
	dc.b	$80		; 0x0d - CR	non printable character
	dc.b	$80		; 0x0e - SO	non printable character
	dc.b	$80		; 0x0f - SI	non printable character
	dc.b	$80		; 0x10 - DLE	non printable character
	dc.b	$80		; 0x11 - DC1	non printable character
	dc.b	$80		; 0x12 - DC2	non printable character
	dc.b	$80		; 0x13 - DC3	non printable character
	dc.b	$80		; 0x14 - DC4	non printable character
	dc.b	$80		; 0x15 - NAC	non printable character
	dc.b	$80		; 0x16 - SYN	non printable character
	dc.b	$80		; 0x17 - ETB	non printable character
	dc.b	$80		; 0x18 - CAN	non printable character
	dc.b	$80		; 0x19 - EM	non printable character
	dc.b	$80		; 0x1a - SUB	non printable character
	dc.b	$80		; 0x1b - ESC	non printable character
	dc.b	$80		; 0x1c - FS	non printable character
	dc.b	$80		; 0x1d - GS	non printable character
	dc.b	$80		; 0x1e - RS	non printable character
	dc.b	$80		; 0x1f - US	non printable character
	dc.b	$20		; 0x20 - (space)
	dc.b	$21		; 0x21 - !
	dc.b	$22		; 0x22 - "
	dc.b	$23		; 0x23 - #
	dc.b	$24		; 0x24 - $
	dc.b	$25		; 0x25 - %
	dc.b	$26		; 0x26 - &
	dc.b	$27		; 0x27 - '
	dc.b	$28		; 0x28 - (
	dc.b	$29		; 0x29 - )
	dc.b	$2a		; 0x2a - *
	dc.b	$2b		; 0x2b - +
	dc.b	$2c		; 0x2c - ,
	dc.b	$2d		; 0x2d - -
	dc.b	$2e		; 0x2e - .
	dc.b	$2f		; 0x2f - /
	dc.b	$30		; 0x30 - 0
	dc.b	$31		; 0x31 - 1
	dc.b	$32		; 0x32 - 2
	dc.b	$33		; 0x33 - 3
	dc.b	$34		; 0x34 - 4
	dc.b	$35		; 0x35 - 5
	dc.b	$36		; 0x36 - 6
	dc.b	$37		; 0x37 - 7
	dc.b	$38		; 0x38 - 8
	dc.b	$39		; 0x39 - 9
	dc.b	$3a		; 0x3a - :
	dc.b	$3b		; 0x3b - ;
	dc.b	$3c		; 0x3c - <
	dc.b	$3d		; 0x3d - =
	dc.b	$3e		; 0x3e - >
	dc.b	$3f		; 0x3f - ?
	dc.b	$00		; 0x40 - @
	dc.b	$41		; 0x41 - A
	dc.b	$42		; 0x42 - B
	dc.b	$43		; 0x43 - C
	dc.b	$44		; 0x44 - D
	dc.b	$45		; 0x45 - E
	dc.b	$46		; 0x46 - F
	dc.b	$47		; 0x47 - G
	dc.b	$48		; 0x48 - H
	dc.b	$49		; 0x49 - I
	dc.b	$4a		; 0x4a - J
	dc.b	$4b		; 0x4b - K
	dc.b	$4c		; 0x4c - L
	dc.b	$4d		; 0x4d - M
	dc.b	$4e		; 0x4e - N
	dc.b	$4f		; 0x4f - O
	dc.b	$50		; 0x50 - P
	dc.b	$51		; 0x51 - Q
	dc.b	$52		; 0x52 - R
	dc.b	$53		; 0x53 - S
	dc.b	$54		; 0x54 - T
	dc.b	$55		; 0x55 - U
	dc.b	$56		; 0x56 - V
	dc.b	$57		; 0x57 - W
	dc.b	$58		; 0x58 - X
	dc.b	$59		; 0x59 - Y
	dc.b	$5a		; 0x5a - Z
	dc.b	$1b		; 0x5b - [
	dc.b	$5c		; 0x5c - \	patched
	dc.b	$1d		; 0x5d - ]
	dc.b	$5e		; 0x5e - ^	patched
	dc.b	$5f		; 0x5f - _	patched
	dc.b	$63		; 0x60 - `	patched
	dc.b	$01		; 0x61 - a
	dc.b	$02		; 0x62 - b
	dc.b	$03		; 0x63 - c
	dc.b	$04		; 0x64 - d
	dc.b	$05		; 0x65 - e
	dc.b	$06		; 0x66 - f
	dc.b	$07		; 0x67 - g
	dc.b	$08		; 0x68 - h
	dc.b	$09		; 0x69 - i
	dc.b	$0a		; 0x6a - j
	dc.b	$0b		; 0x6b - k
	dc.b	$0c		; 0x6c - l
	dc.b	$0d		; 0x6d - m
	dc.b	$0e		; 0x6e - n
	dc.b	$0f		; 0x6f - o
	dc.b	$10		; 0x70 - p
	dc.b	$11		; 0x71 - q
	dc.b	$12		; 0x72 - r
	dc.b	$13		; 0x73 - s
	dc.b	$14		; 0x74 - t
	dc.b	$15		; 0x75 - u
	dc.b	$16		; 0x76 - v
	dc.b	$17		; 0x77 - w
	dc.b	$18		; 0x78 - x
	dc.b	$19		; 0x79 - y
	dc.b	$1a		; 0x7a - z
	dc.b	$64		; 0x7b - {	patched
	dc.b	$65		; 0x7c - |	patched
	dc.b	$66		; 0x7d - }	patched
	dc.b	$67		; 0x7e - ~	patched
	dc.b	$80		; 0x7f - DEL	non printable character

hex_table
	dc.b	'0123456789abcdef'

notes
	dc.w	$0112,$0123,$0134,$0146,$015a,$016e,$0184,$018b,$01b3,$01cd,$01e9,$0206	; N_C0_ to N_B0_
	dc.w	$0225,$0245,$0268,$028c,$02b3,$02dc,$0308,$0336,$0367,$039b,$03d2,$040c	; N_C1_ to N_B1_
	dc.w	$0449,$048b,$04d0,$0519,$0567,$05b9,$0610,$066c,$06ce,$0735,$07a3,$0817	; N_C2_ to N_B2_
	dc.w	$0893,$0915,$099f,$0a32,$0acd,$0b72,$0c20,$0cd8,$0d9c,$0e6b,$0f46,$102f	; N_C3_ to N_B3_
	dc.w	$1125,$122a,$133f,$1464,$159a,$16e3,$183f,$1981,$1b38,$1cd6,$1e80,$205e	; N_C4_ to N_B4_
	dc.w	$224b,$2455,$267e,$28c8,$2b34,$2dc6,$307f,$3361,$366f,$39ac,$3d1a,$40bc	; N_C5_ to N_B5_
	dc.w	$4495,$48a9,$4cfc,$518f,$5669,$5b8c,$60fe,$6602,$6cdf,$7358,$7a34,$8178	; N_C6_ to N_B6_
	dc.w	$892b,$9153,$99f7,$a31f,$acd2,$b719,$c1fc,$cd85,$d9bd,$e6b0,$f467		; N_C7_	to N_A7S