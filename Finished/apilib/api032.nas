[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api032.nas"]

		GLOBAL	_api_shareadd

[SECTION .text]

_api_shareadd:	; void api_shareadd();
		MOV		EDX,32
		INT		0x40
		RET
