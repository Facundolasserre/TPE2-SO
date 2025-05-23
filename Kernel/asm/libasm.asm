GLOBAL cpuVendor
GLOBAL getSeconds
GLOBAL getHours
GLOBAL getMinutes
GLOBAL spkOut
GLOBAL spkIn
GLOBAL rtc_bd_to_binary

section .text
	



cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret


rtc_bd_to_binary:
	push rbp
	mov rbp, rsp

	; Obtenemos los datos del registro OBh con el fin de cambiar
	; como se muestra la hora. Hasta el momento te lo da en BD, pero
	; la quiero en binario.
	; Copiamos en al la dir OBh para ingresar al registro
	; RTC Status register B.
	mov al, 0Bh 
	; Copiamos en la direccion 70h el 0Bh para poder acceder al registro
	; RTC Status register B.
	out 70h, al  
	; Hacemos la lectura del registro y la guardamos en al.
	in al, 71h
	; Creacion de la mascara para poner en 1 el bit 2 (tercero)
	; para leer en binario.
	mov bl, 4h
	or al, bl
	; Guardamos el cambio en bl.
	mov bl, al
	; Movemos a al 0Bh para poder volver a acceder al registro
	; RTC Status register B.
	mov al, 0Bh
	; Copiamos en la direccion 70h el 0Bh para poder acceder al registro
	; RTC Status register B. 
	out 70h, al
	; Movemos a al el cambio que queremos ingresar.
	mov al, bl
	; Para poder escribir en el registro usamos out.
	out 71h, al

	mov rsp, rbp
	pop rbp
	ret


	
getSeconds:
	push rbp
	mov rbp, rsp

	call rtc_bd_to_binary


    mov al, 0x00
    out 70h, al
    in al, 71h

	mov rsp, rbp
	pop rbp
    ret

getMinutes:
	push rbp
	mov rbp, rsp

	call rtc_bd_to_binary

    mov al, 0x02
    out 70h, al
    in al, 71h

	mov rsp, rbp
	pop rbp
    ret

getHours:
	push rbp
	mov rbp, rsp

	call rtc_bd_to_binary

    mov al, 0x04
    out 70h, al
    in al, 71h

	mov rsp, rbp
	pop rbp
    ret		


spkIn:
	push rbp
	mov rbp, rsp

	mov rdx, rdi
	in al, dx

	mov rsp, rbp
	pop rbp
	ret

spkOut:
	push rbp
	mov rbp, rsp 

	mov rax, rsi 
	mov rdx, rdi 
	out dx, al 

	mov rsp, rbp
	pop rbp
	ret