; nasm -f bin -o mbr.bin mbr.asm
; sudo dd if=mbr.bin of=/dev/sda bs=446 count=1


cpu 386
bits 16
org 0x0600


section .text
    ; ------------------------------------------
    ; Initialize machine state
    ; ------------------------------------------
    push    word 2
    popf
    xor     ax, ax
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, 0x7C00     ; stack @ 0000:7C00
    ; ------------------------------------------
    ; Copy itself from 0000:7C00 to 0000:0600
    ; ------------------------------------------
    mov     si, sp
    mov     di, 0x0600
    mov     cx, 256
    rep movsw
    jmp     0:reloc
reloc:
    ; ------------------------------------------
    ; Search for active partition
    ; ------------------------------------------
    mov     bx, 4           ; loop 4 primary partitions
    mov     si, ptable
next_primary:
    cmp     byte [si], 0x80
    je      load_partition
    lea     si, [si+16]
    dec     bx
    jnz     next_primary
panic:
    int     0x18            ; Nothing bootable, panic
    ; ------------------------------------------
    ; Load active partition
    ; ------------------------------------------
load_partition:
    push    word 0
    push    word 0
    push    word [si+8+2]
    push    word [si+8]
    push    word 0
    push    word 0x7C00
    push    word 1
    push    word 0x0010
    mov     si, sp
    mov     ah, 0x42
    int     0x13
    jc      panic
    add     sp, 16
    ; ------------------------------------------
    ; Pass control to volume boot sector
    ; ------------------------------------------
    push    word 0
    push    word 0x7C00
    retf
; Should not change below
; ----------------------------------------------
    TIMES   440-($-$$)  db 0
disksign:
        dd 0
        dw 0
ptable:
    TIMES   510-($-$$)  db  0   ; partition table
        dw 0xAA55               ; boot record signature
