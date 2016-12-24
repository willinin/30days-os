[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api043.nas"]

		GLOBAL	_api_unlock

[SECTION .text]

_api_unlock:		; void api_unlock();
		MOV		EDX,43
		INT		0x40
		RET
