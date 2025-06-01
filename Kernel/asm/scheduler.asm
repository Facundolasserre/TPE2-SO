GLOBAL scheduling_handler

GLOBAL halt_asm

EXTERN schedule 

section .text

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