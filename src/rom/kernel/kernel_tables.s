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
	dc.b	$80		; 0x0A - LF	non printable character
	dc.b	$80		; 0x0B - VT	non printable character
	dc.b	$80		; 0x0C - FF	non printable character
	dc.b	$80		; 0x0D - CR	non printable character
	dc.b	$80		; 0x0E - SO	non printable character
	dc.b	$80		; 0x0F - SI	non printable character
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
	dc.b	$80		; 0x1A - SUB	non printable character
	dc.b	$80		; 0x1B - ESC	non printable character
	dc.b	$80		; 0x1C - FS	non printable character
	dc.b	$80		; 0x1D - GS	non printable character
	dc.b	$80		; 0x1E - RS	non printable character
	dc.b	$80		; 0x1F - US	non printable character
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
	dc.b	$2A		; 0x2A - *
	dc.b	$2B		; 0x2B - +
	dc.b	$2C		; 0x2C - ,
	dc.b	$2D		; 0x2D - -
	dc.b	$2E		; 0x2E - .
	dc.b	$2F		; 0x2F - /
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
	dc.b	$3A		; 0x3A - :
	dc.b	$3B		; 0x3B - ;
	dc.b	$3C		; 0x3C - <
	dc.b	$3D		; 0x3D - =
	dc.b	$3E		; 0x3E - >
	dc.b	$3F		; 0x3F - ?
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
	dc.b	$4A		; 0x4A - J
	dc.b	$4B		; 0x4B - K
	dc.b	$4C		; 0x4C - L
	dc.b	$4D		; 0x4D - M
	dc.b	$4E		; 0x4E - N
	dc.b	$4F		; 0x4F - O
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
	dc.b	$5A		; 0x5A - Z
	dc.b	$1B		; 0x5B - [
	dc.b	$5C		; 0x5C - \	patched
	dc.b	$1D		; 0x5D - ]
	dc.b	$5E		; 0x5E - ^	patched
	dc.b	$5F		; 0x5F - _	patched
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
	dc.b	$0A		; 0x6A - j
	dc.b	$0B		; 0x6B - k
	dc.b	$0C		; 0x6C - l
	dc.b	$0D		; 0x6D - m
	dc.b	$0E		; 0x6E - n
	dc.b	$0F		; 0x6F - o
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
	dc.b	$1A		; 0x7A - z
	dc.b	$64		; 0x7B - {	patched
	dc.b	$65		; 0x7C - |	patched
	dc.b	$66		; 0x7D - }	patched
	dc.b	$67		; 0x7E - ~	patched
	dc.b	$80		; 0x7F - DEL	non printable character

hex_table
	dc.b	'0123456789abcdef'

;	The following table is based on a SID clock frequency of 985248Hz (PAL)
;	Calculations were made according to Codebase64 article
;	https://codebase64.org/doku.php?id=base:how_to_calculate_your_own_sid_frequency_table
;
notes
	dc.w	$0116,$0127,$0139,$014B,$015F,$0174,$018A,$01A1,$01BA,$01D4,$01F0,$020E	; N_C0_ to N_B0_
	dc.w	$022D,$024E,$0271,$0296,$02BE,$02E7,$0314,$0342,$0374,$03A9,$03E0,$041B	; N_C1_ to N_B1_
	dc.w	$045A,$049C,$04E2,$052D,$057B,$05CF,$0627,$0685,$06E8,$0751,$07C1,$0837	; N_C2_ to N_B2_
	dc.w	$08B4,$0938,$09C4,$0A59,$0AF7,$0B9D,$0C4E,$0D0A,$0DD0,$0EA2,$0F81,$106D	; N_C3_ to N_B3_
	dc.w	$1167,$1270,$1389,$14B2,$15ED,$173B,$189C,$1A13,$1BA0,$1D45,$1F02,$20DA	; N_C4_ to N_B4_
	dc.w	$22CE,$24E0,$2711,$2964,$2BDA,$2E76,$3139,$3426,$3740,$3A89,$3E04,$41B4	; N_C5_ to N_B5_
	dc.w	$459C,$49C0,$4E23,$52C8,$57B4,$5CEB,$6272,$684C,$6E80,$7512,$7C08,$8368	; N_C6_ to N_B6_
	dc.w	$8B39,$9380,$9C45,$A590,$AF68,$B9D6,$C4E3,$D099,$DD00,$EA24,$F810	; N_C7_	to N_A7S

;notes_old
;	dc.w	$0112,$0123,$0134,$0146,$015a,$016e,$0184,$018b,$01b3,$01cd,$01e9,$0206	; N_C0_ to N_B0_
;	dc.w	$0225,$0245,$0268,$028c,$02b3,$02dc,$0308,$0336,$0367,$039b,$03d2,$040c	; N_C1_ to N_B1_
;	dc.w	$0449,$048b,$04d0,$0519,$0567,$05b9,$0610,$066c,$06ce,$0735,$07a3,$0817	; N_C2_ to N_B2_
;	dc.w	$0893,$0915,$099f,$0a32,$0acd,$0b72,$0c20,$0cd8,$0d9c,$0e6b,$0f46,$102f	; N_C3_ to N_B3_
;	dc.w	$1125,$122a,$133f,$1464,$159a,$16e3,$183f,$1981,$1b38,$1cd6,$1e80,$205e	; N_C4_ to N_B4_
;	dc.w	$224b,$2455,$267e,$28c8,$2b34,$2dc6,$307f,$3361,$366f,$39ac,$3d1a,$40bc	; N_C5_ to N_B5_
;	dc.w	$4495,$48a9,$4cfc,$518f,$5669,$5b8c,$60fe,$6602,$6cdf,$7358,$7a34,$8178	; N_C6_ to N_B6_
;	dc.w	$892b,$9153,$99f7,$a31f,$acd2,$b719,$c1fc,$cd85,$d9bd,$e6b0,$f467	; N_C7_	to N_A7S

	align	1
test_table
	DC.B	$00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0A,$0B,$0C,$0D,$0E,$0F
	DC.B	$10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$1A,$1B,$1C,$1D,$1E,$1F
	DC.B	$20,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2A,$2B,$2C,$2D,$2E,$2F
	DC.B	$30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3A,$3B,$3C,$3D,$3E,$3F
	DC.B	$40,$41,$42,$43,$44,$45,$46,$47,$48,$49,$4A,$4B,$4C,$4D,$4E,$4F
	DC.B	$50,$51,$52,$53,$54,$55,$56,$57,$58,$59,$5A,$5B,$5C,$5D,$5E,$5F
	DC.B	$60,$61,$62,$63,$64,$65,$66,$67,$68,$69,$6A,$6B,$6C,$6D,$6E,$6F
	DC.B	$70,$71,$72,$73,$74,$75,$76,$77,$78,$79,$7A,$7B,$7C,$7D,$7E,$7F
	DC.B	$80,$81,$82,$83,$84,$85,$86,$87,$88,$89,$8A,$8B,$8C,$8D,$8E,$8F
	DC.B	$90,$91,$92,$93,$94,$95,$96,$97,$98,$99,$9A,$9B,$9C,$9D,$9E,$9F
	DC.B	$A0,$A1,$A2,$A3,$A4,$A5,$A6,$A7,$A8,$A9,$AA,$AB,$AC,$AD,$AE,$AF
	DC.B	$B0,$B1,$B2,$B3,$B4,$B5,$B6,$B7,$B8,$B9,$BA,$BB,$BC,$BD,$BE,$BF
	DC.B	$C0,$C1,$C2,$C3,$C4,$C5,$C6,$C7,$C8,$C9,$CA,$CB,$CC,$CD,$CE,$CF
	DC.B	$D0,$D1,$D2,$D3,$D4,$D5,$D6,$D7,$D8,$D9,$DA,$DB,$DC,$DD,$DE,$DF
	DC.B	$E0,$E1,$E2,$E3,$E4,$E5,$E6,$E7,$E8,$E9,$EA,$EB,$EC,$ED,$EE,$EF
	DC.B	$F0,$F1,$F2,$F3,$F4,$F5,$F6,$F7,$F8,$F9,$FA,$FB,$FC,$FD,$FE,$FF