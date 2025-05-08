	.arch msp430g2553
	.p2align 1,0
	.text

	.global ledControl
	.extern P1OUT

ledControl:
	cmp #0, r12
	jz nored
	bis #1, &P1OUT 		;P1.0 is red LED
nored:	
	cmp #0, r13
	jz nogreen
	bis #64, &P1OUT 	;P1.6 is green LED
nogreen:
	pop r0
	
