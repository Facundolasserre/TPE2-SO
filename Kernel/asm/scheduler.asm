GLOBAL scheduling_handler ; tick handler
GLOBAL halt_asm
GLOBAL cli_asm
GLOBAL sti_asm
GLOBAL fill_stack
EXTERN schedule 

section .text:

%macro pushState 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popState 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

;visto en clase 5 de mayo
scheduling_handler: ; tick handler
    pushState
    mov rdi, rsp
    call schedule
    mov rsp, rax

    mov al, 20h
    out 20h, al
   
    popState
    ;pop rax debug a ver si esta el RIP de initProcessWrapper
    iretq

fill_stack:
    push rbp
    mov rbp, rsp 
    mov rsp, rdi 

    push 0x0
    push rdi        ;   SP
    push 0x202      ;   RFLAGS
    push 0x8        ;   CS
                    ;   > Preparo argumentos para initProcessWrapper 
    push rsi        ;   RIP = initProcessWrapper
    mov rdi, rdx    ;   program
    mov rsi, rcx    ;   argc
    mov rdx, r8     ;   argv
    pushState       ;   Cargo algun estado de registros

    mov rax, rsp 
    
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