GLOBAL scheduling_handler

GLOBAL halt_asm
GLOBAL cli_asm
GLOBAL sti_asm
GLOBAL fill_stack
EXTERN schedule 

section .text

%macro pushState 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    pushfq        ; Guarda el registro de banderas (flags)
%endmacro

%macro popState 0
    popfq         ; Restaura el registro de banderas (flags)
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

;visto en clase 5 de mayo
scheduling_handler:
    pushState
    mov rdi, rsp
    call schedule
    mov rsp, rax

    mov al, 20h
    out 20h, al
   
    popState
    iretq

fill_stack:
    push rbp
    mov rbp, rsp 
    mov rsp, rdi 

    push 0x0
    push rsi        ;   SP
    push 0x202      ;   RFLAGS
    push 0x8        ;   CS
                    ;   > Preparo argumentos para initProcessWrapper 
    mov rdi, rdx    ;   program
    mov rsi, rcx    ;   argc
    mov rdx, r8     ;   argv
    push rsi        ;   RIP = initProcessWrapper
    pushState       ;   Cargo algun estado de registros

    mov rsp, rbp
    pop rbp

    ret

halt_asm:
    hlt
    ret

cli_asm:
    cli
    ret

sti_asm:
    sti
    ret