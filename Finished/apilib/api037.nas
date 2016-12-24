[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api037.nas"]

		GLOBAL	_api_var_create

[SECTION .text]

_api_var_create:		; int api_var_create(char *name,int length);
		PUSH	EBX
		MOV		EDX,37
		MOV		ECX,[ESP+12]		; length
		MOV		EBX,[ESP+8]			; name
		INT		0x40
		POP		EBX
		RET
