%ifndef __BLUEMOON_KERNEL__
%define __BLUEMOON_KERNEL__



; Address Translation used at early kernel stage
; ----------------------------------------------
%define KADDR_ZERO_VMA      (0xFFFFFFFF80000000)
%define KADDR_KERNEL_PMA    (0x00100000 + 4096)
%define KADDR_KERNEL_VMA    (KADDR_ZERO_VMA + KADDR_KERNEL_PMA)
%define KADDR_BOOTDATA      (KADDR_ZERO_VMA + 0x0600)
%define KADDR_PMA(x)        ((x) - KADDR_ZERO_VMA)

%define APPADDR_PROCESS_STACK   (0x7FC00000)


; Selector ID
; ----------------------------------------------
%define SEG_CODE64_0    (0x08)
%define SEG_DATA64_0    (0x10)
%define SEG_CODE32_3    (0x18)
%define SEG_DATA64_3    (0x20)
%define SEG_CODE64_3    (0x28)
%define SEG_TSS         (0x30)


; Interrupt Numbering
; ----------------------------------------------
%define IRQ_BASE_INTNUM (0x20)


; TSS64
; ----------------------------------------------
struc TSS64
                    resd    1
    tss64_rsp0      resq    1
    tss64_rsp1      resq    1
    tss64_rsp2      resq    1
                    resd    2
    tss64_ist1      resq    1
    tss64_ist2      resq    1
    tss64_ist3      resq    1
    tss64_ist4      resq    1
    tss64_ist5      resq    1
    tss64_ist6      resq    1
    tss64_ist7      resq    1
                    resd    2
    tss64_iobase    resd    1
endstruc



; --------------------------------------
; Call Macro (AMD x86-64 ABI)
; --------------------------------------
%macro call 1
    call    %1
%endmacro

%macro call 2
    mov     rdi, %2
    call    %1
%endmacro

%macro call 3
    mov     rdi, %2
    mov     rsi, %3
    call    %1
%endmacro

%macro call 4
    mov     rdi, %2
    mov     rsi, %3
    mov     rdx, %4
    call    %1
%endmacro

%macro call 5
    mov     rdi, %2
    mov     rsi, %3
    mov     rdx, %4
    mov     rcx, %5
    call    %1
%endmacro



%endif

