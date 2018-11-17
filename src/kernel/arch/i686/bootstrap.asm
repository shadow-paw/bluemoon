cpu P4
bits 32

%include "kernel.inc"

global bootstrap, k_PDT, k_TSS
extern kmain
extern sbss, ebss, _kernel_end
extern ctor_start, ctor_end, dtor_start, dtor_end

%define KSTACK_SIZE (81902)

section .bss
; ----------------------------------------------
align 4096
k_PDT  resb    4096
k_PT   resb    4096
kstack resb    KSTACK_SIZE
tss    resb    104


section .data
; ----------------------------------------------
align 16
gdtr    dw  6 *8 -1
        dd  gdt
        dw  0

align 16
gdt     dd  0, 0
        dd  0x0000FFFF, 0x00CF9A00              ; 0x08 CODE32 DPL0
        dd  0x0000FFFF, 0x00CF9200              ; 0x10 DATA32 DPL0
        dd  0x0000FFFF, 0x00CFFA00              ; 0x18 CODE32 DPL3
        dd  0x0000FFFF, 0x00CFF200              ; 0x20 DATA32 DPL3
        dd  0, 0                                ; TSS


section .text
; ----------------------------------------------
bootstrap:
    ; Clear BSS
    mov     edi, KADDR_PMA(sbss)
    mov     ecx, KADDR_PMA(ebss)
    sub     ecx, edi
    shr     ecx, 2
    xor     eax, eax
    rep     stosd
    ; Setup page tables
    ; Linear 1M -> Physical 1M (kernel loaded addr)
    ; Linear 3.75G -> Physical 1M (kernel loaded addr)
    mov     esi, KADDR_PMA(k_PDT)
    mov     edi, KADDR_PMA(k_PT)
    mov     dword [esi + (KADDR_KERNEL_PMA>>22)*4], KADDR_PMA(k_PT) +3
    mov     dword [esi + (KADDR_KERNEL_VMA>>22)*4], KADDR_PMA(k_PT) +3
    mov     ebx, KADDR_PMA(_kernel_end)
    mov     eax, 3
.1:
    stosd
    add     eax, 4096
    cmp     eax, ebx
    jb      .1

    mov     cr3, esi
    mov     eax, cr0
    or      eax, 0x80000000
    mov     cr0, eax
    ; eip -> higher end of address space
    mov     eax, dword .HigherHalf
    jmp     eax
.HigherHalf:
    ; unmap 1M
    mov     dword [esi + (KADDR_KERNEL_PMA>>22)*4], 0
    mov     cr3, esi
    ; Load GDT & Reload selectors
    lgdt    [gdtr]
    mov     eax, SEG_DATA32_0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    mov     esp, dword kstack + KSTACK_SIZE
    jmp     dword SEG_CODE32_0:.reload_cs
.reload_cs:
    ; TSS
    mov     dword [tss + tss32_ss0], SEG_DATA32_0
    mov     dword [tss + tss32_esp0], esp
    ; mov     dword [tss + tss32_cr3], KADDR_PMA(k_PDT)
    mov     eax, tss
    shl     eax, 16
    or      eax, 67h                ; [Base 15..00][Limit 15..00]
    mov     [gdt+SEG_TSS], eax
    mov     eax, dword tss
    mov     edx, dword tss
    shr     edx, 16
    and     eax, 0xFF000000
    and     edx, 0x000000FF
    or      eax, edx
    or      eax, 0x00008900
    mov     [gdt+SEG_TSS+4], eax
    mov     eax, SEG_TSS
    ltr     ax
    ; Setup minimal C environment
    xor     ebp, ebp
    ; Constructors
    mov     ebx, ctor_start
    jmp     .ctors_until_end
.call_constructor:
    call    [ebx]
    add     ebx, 4
.ctors_until_end:
    cmp     ebx, ctor_end
    jb      .call_constructor
    ; invoke C main
    call    kmain
    ; Destructors
    mov     ebx, dtor_end
    jmp     .dtors_until_end
.call_destructor:
    sub     ebx, 4
    call    [ebx]
.dtors_until_end:
    cmp     ebx, dtor_start
    ja      .call_destructor
    ; should not return from main, but just in case we halt here
.halt:
    cli
    hlt
    jmp     .halt
