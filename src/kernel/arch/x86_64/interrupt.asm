cpu x86-64
bits 64

%include "kernel.inc"

global idt_init, idt_set
extern INT_00, INT_01, INT_02, INT_03, INT_04, INT_05, INT_06, INT_07
extern INT_08,         INT_0A, INT_0B, INT_0C, INT_0D, INT_0E, INT_0F
extern INT_10, INT_11, INT_12, INT_13

section .bss
; ----------------------------------------------
align 4096
idt resq 256         ; 256 interrupts

section .data
; ----------------------------------------------
align 16
idtr    dw  256*16-1
        dq  idt
        dw  0

section .text
; ----------------------------------------------
idt_init:
    call    idt_set, 0x00, qword _INT_00, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x01, qword _INT_01, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x02, qword _INT_02, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x03, qword _INT_03, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x04, qword _INT_04, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x05, qword _INT_05, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x06, qword _INT_06, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x07, qword _INT_07, 1000111000000001b    ; P DPL=0 TYPE=1110 IST=1
    call    idt_set, 0x08, qword _INT_08, 1000111000000001b    ; P DPL=0 TYPE=1110 IST=1
    call    idt_set, 0x0A, qword _INT_0A, 1000111000000001b    ; P DPL=0 TYPE=1110 IST=1
    call    idt_set, 0x0B, qword _INT_0B, 1000111000000001b    ; P DPL=0 TYPE=1110 IST=1
    call    idt_set, 0x0C, qword _INT_0C, 1000111000000001b    ; P DPL=0 TYPE=1110 IST=1
    call    idt_set, 0x0D, qword _INT_0D, 1000111000000001b    ; P DPL=0 TYPE=1110 IST=1
    call    idt_set, 0x0E, qword _INT_0E, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x10, qword _INT_10, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x11, qword _INT_11, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    call    idt_set, 0x12, qword _INT_12, 1000111000000010b    ; P DPL=0 TYPE=1110 IST=2
    call    idt_set, 0x13, qword _INT_13, 1000111000000000b    ; P DPL=0 TYPE=1110 IST=0
    mov     rdi, qword idtr
    lidt    [rdi]
    ret

; ----------------------------------------------
; idt_set(num, function, access (P:1 DPL:2 0:1 TYPE:4 0:5 IST:3)
idt_set:
    mov     r11, qword idt
    mov     eax, esi
    and     eax, 0xFFFF
    or      eax, SEG_CODE64_0 << 16
    shr     rsi, 16
    shl     rsi, 16
    or      rsi, rdx
    shl     rdi, 4
    mov     dword [r11 + rdi], eax
    mov     qword [r11 + rdi +4], rsi
    mov     dword [r11 + rdi +12], 0
    ret

; INT00
; ----------------------------------------------
align 16
_INT_00:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_00
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT01
; ----------------------------------------------
align 16
_INT_01:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_01
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT02
; ----------------------------------------------
align 16
_INT_02:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_02
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT03
; ----------------------------------------------
align 16
_INT_03:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_03
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT04
; ----------------------------------------------
align 16
_INT_04:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_04
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT05
; ----------------------------------------------
align 16
_INT_05:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_05
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT06
; ----------------------------------------------
align 16
_INT_06:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_06
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT07
; ----------------------------------------------
align 16
_INT_07:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_07
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT08
; ----------------------------------------------
align 16
_INT_08:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_08
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT0A
; ----------------------------------------------
align 16
_INT_0A:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    mov     rdi, [rsp+9*8]
    call    INT_0A
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    add     rsp, 8
    iretq

; INT0B
; ----------------------------------------------
align 16
_INT_0B:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    mov     rdi, [rsp+9*8]
    call    INT_0B
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    add     rsp, 8
    iretq

; INT0C
; ----------------------------------------------
align 16
_INT_0C:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    mov     rdi, [rsp+9*8]
    call    INT_0C
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    add     rsp, 8
    iretq

; INT0D
; ----------------------------------------------
align 16
_INT_0D:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    mov     rdi, [rsp+9*8]
    mov     rsi, [rsp+10*8]
    call    INT_0D
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    add     rsp, 8
    iretq

; INT0E
; ----------------------------------------------
align 16
_INT_0E:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    mov     rdi, [rsp+9*8]
    mov     rsi, cr2
    mov     rdx, [rsp+9*8+8]
    call    INT_0E
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    add     rsp, 8
    iretq

; INT10
; ----------------------------------------------
align 16
_INT_10:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_10
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT11
; ----------------------------------------------
align 16
_INT_11:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    mov     rdi, [rsp+9*8]
    call    INT_11
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    add     rsp, 8
    iretq

; INT12
; ----------------------------------------------
align 16
_INT_12:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; INT13
; ----------------------------------------------
align 16
_INT_13:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    call    INT_13
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax
    iretq

; ----------------------------------------------
