[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api038.nas"]

		GLOBAL	_api_var_read

[SECTION .text]

_api_var_read:		; char* api_var_read(char *name,int n);
		PUSH	EBX
		MOV		EDX,38
		MOV		ECX,[ESP+12]		; n
		MOV		EBX,[ESP+8]			; name
		INT		0x40
		POP		EBX
		RET
