GLOBAL loader
GLOBAL reset

EXTERN main
EXTERN initializeKernelBinary
EXTERN getStackBase

loader:
	call initializeKernelBinary	; Set up the kernel binary, and get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call main
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang

reset:
	call getStackBase
	mov rsp, rax
	call main
