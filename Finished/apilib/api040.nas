[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api040.nas"]

		GLOBAL	_api_var_free

[SECTION .text]

_api_var_free:		; int api_var_free(char *name)
		PUSH	EBX
		MOV		EDX,40
		MOV		EBX,[ESP+8]			; name
		INT		0x40
		POP		EBX
		RET
