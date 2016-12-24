[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api028.nas"]

		GLOBAL	_api_addr

[SECTION .text]

_api_addr:	; void api_addr(int addr);
		MOV		EDX,28
		MOV		EAX,[ESP+4]		; c
		INT		0x40
		RET
