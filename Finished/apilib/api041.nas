[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api041.nas"]

		GLOBAL	_api_avoid_sleep

[SECTION .text]

_api_avoid_sleep:		; void api_avoid_sleep();
		MOV		EDX,41
		INT		0x40
		RET
