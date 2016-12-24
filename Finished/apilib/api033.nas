[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api033.nas"]

		GLOBAL	_api_consume

[SECTION .text]

_api_consume:	; void api_consume();
		MOV		EDX,33
		INT		0x40
		RET
