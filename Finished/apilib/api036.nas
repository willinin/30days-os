[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api036.nas"]

		GLOBAL	_api_exit

[SECTION .text]

_api_exit:		; void api_exit(int x)
		MOV		EDX,36
		MOV		EAX,[ESP+8]
		INT		0x40
		RET
