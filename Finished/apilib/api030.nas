[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api030.nas"]

		GLOBAL	_api_reader

[SECTION .text]

_api_reader:	; void api_reader();
		MOV		EDX,30
		INT		0x40
		RET
