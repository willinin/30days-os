[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api042.nas"]

		GLOBAL	_api_lock

[SECTION .text]

_api_lock:		; void api_lock();
		MOV		EDX,42
		INT		0x40
		RET
