; rom tables

hex_table
	dc.b	'0123456789abcdef'

;	The following table is based on a SID clock frequency of 985248Hz (PAL)
;	Calculations were made according to Codebase64 article
;	https://codebase64.org/doku.php?id=base:how_to_calculate_your_own_sid_frequency_table
;
	ALIGN	1
notes
	DC.W	$0116,$0127,$0139,$014B,$015F,$0174,$018A,$01A1,$01BA,$01D4,$01F0,$020E	; N_C0_ to N_B0_
	DC.W	$022D,$024E,$0271,$0296,$02BE,$02E7,$0314,$0342,$0374,$03A9,$03E0,$041B	; N_C1_ to N_B1_
	DC.W	$045A,$049C,$04E2,$052D,$057B,$05CF,$0627,$0685,$06E8,$0751,$07C1,$0837	; N_C2_ to N_B2_
	DC.W	$08B4,$0938,$09C4,$0A59,$0AF7,$0B9D,$0C4E,$0D0A,$0DD0,$0EA2,$0F81,$106D	; N_C3_ to N_B3_
	DC.W	$1167,$1270,$1389,$14B2,$15ED,$173B,$189C,$1A13,$1BA0,$1D45,$1F02,$20DA	; N_C4_ to N_B4_
	DC.W	$22CE,$24E0,$2711,$2964,$2BDA,$2E76,$3139,$3426,$3740,$3A89,$3E04,$41B4	; N_C5_ to N_B5_
	DC.W	$459C,$49C0,$4E23,$52C8,$57B4,$5CEB,$6272,$684C,$6E80,$7512,$7C08,$8368	; N_C6_ to N_B6_
	DC.W	$8B39,$9380,$9C45,$A590,$AF68,$B9D6,$C4E3,$D099,$DD00,$EA24,$F810	; N_C7_	to N_A7S

;notes_old
;	dc.w	$0112,$0123,$0134,$0146,$015a,$016e,$0184,$018b,$01b3,$01cd,$01e9,$0206	; N_C0_ to N_B0_
;	dc.w	$0225,$0245,$0268,$028c,$02b3,$02dc,$0308,$0336,$0367,$039b,$03d2,$040c	; N_C1_ to N_B1_
;	dc.w	$0449,$048b,$04d0,$0519,$0567,$05b9,$0610,$066c,$06ce,$0735,$07a3,$0817	; N_C2_ to N_B2_
;	dc.w	$0893,$0915,$099f,$0a32,$0acd,$0b72,$0c20,$0cd8,$0d9c,$0e6b,$0f46,$102f	; N_C3_ to N_B3_
;	dc.w	$1125,$122a,$133f,$1464,$159a,$16e3,$183f,$1981,$1b38,$1cd6,$1e80,$205e	; N_C4_ to N_B4_
;	dc.w	$224b,$2455,$267e,$28c8,$2b34,$2dc6,$307f,$3361,$366f,$39ac,$3d1a,$40bc	; N_C5_ to N_B5_
;	dc.w	$4495,$48a9,$4cfc,$518f,$5669,$5b8c,$60fe,$6602,$6cdf,$7358,$7a34,$8178	; N_C6_ to N_B6_
;	dc.w	$892b,$9153,$99f7,$a31f,$acd2,$b719,$c1fc,$cd85,$d9bd,$e6b0,$f467	; N_C7_	to N_A7S


	ALIGN	1
color_table
	DC.W	C64_BLACK
	DC.W	C64_WHITE
	DC.W	C64_RED
	DC.W	C64_CYAN
	DC.W	C64_PURPLE
	DC.W	C64_GREEN
	DC.W	C64_BLUE
	DC.W	C64_YELLOW
	DC.W	C64_ORANGE
	DC.W	C64_BROWN
	DC.W	C64_LIGHTRED
	DC.W	C64_DARKGREY
	DC.W	C64_GREY
	DC.W	C64_LIGHTGREEN
	DC.W	C64_LIGHTBLUE
	DC.W	C64_LIGHTGREY


	ALIGN	1
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