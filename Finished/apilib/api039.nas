[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api039.nas"]

		GLOBAL	_api_var_wrt

[SECTION .text]

_api_var_wrt:		; int api_var_wrt(char *name,int n,int value)
		PUSH	EBX
		MOV		EDX,39
    MOV		EAX,[ESP+16]		; value
		MOV		ECX,[ESP+12]		; n
		MOV		EBX,[ESP+8]			; name
		INT		0x40
		POP		EBX
		RET
