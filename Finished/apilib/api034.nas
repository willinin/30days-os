[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api034.nas"]

		GLOBAL	_api_produce

[SECTION .text]

_api_produce:	; void api_produce();
		MOV		EDX,34
		INT		0x40
		RET
