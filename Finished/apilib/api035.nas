[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api035.nas"]

		GLOBAL	_api_entrance

[SECTION .text]

_api_entrance:		; void api_entrance(int x)
		MOV		EDX,35
		MOV		EAX,[ESP+8]
		INT		0x40
		RET
