cpu x86-64
bits 64


%include "kernel.inc"


global bootstrap, k_PML4T, k_PDPT, k_PDT
extern idt_init, kmain
extern sbss, ebss, _kernel_end
extern ctor_start, ctor_end, dtor_start, dtor_end

%define IST_SIZE    (4096)
%define KSTACK_SIZE (8192)

section .bss
; ----------------------------------------------
align 4096
k_PML4T     resb    4096
k_PDPT      resb    4096
k_PDT       resb    4096
k_PT        resb    4096
kstack      resb    KSTACK_SIZE
ist         resb    IST_SIZE*7
tss         resb 104

section .data
; ----------------------------------------------
align 16
gdtr    dw  8 *8 -1
        dq  gdt
        dw  0
align 16
gdt     dd  0, 0
        dd  0x0000FFFF, 0x00AF9A00              ; 0x08 CODE64 DPL0
        dd  0x0000FFFF, 0x008F9200              ; 0x10 DATA64 DPL0
        dd  0x0000FFFF, 0x00CFFA00              ; 0x18 CODE32 DPL3
        dd  0x0000FFFF, 0x008FF200              ; 0x20 DATA64 DPL3
        dd  0x0000FFFF, 0x00AFFA00              ; 0x28 CODE64 DPL3
        dd  0, 0, 0, 0                          ; 0x30 TSS

section .text
; ----------------------------------------------
bootstrap:
    ; Clear BSS
    mov     rdi, KADDR_PMA(sbss)
    mov     rcx, KADDR_PMA(ebss)
    sub     rcx, rdi
    shr     rcx, 3
    xor     eax, eax
    rep     stosq
    ; Setup page tables
    mov     rsi, KADDR_PMA(k_PML4T)
    mov     rdi, KADDR_PMA(k_PDPT)
    mov     rdx, KADDR_PMA(k_PDT)
    mov     rcx, KADDR_PMA(k_PT)
    ; map page
    mov     qword [rsi], KADDR_PMA(k_PDPT) +3                                       ; First 512 GiB
    mov     qword [rsi + ((KADDR_KERNEL_VMA>>39)&511)*8], KADDR_PMA(k_PDPT) +3      ; Kernel/Last 512 GiB
    mov     qword [rdi], KADDR_PMA(k_PDT) +3                                        ; 0~1 GiB
    mov     qword [rdi + ((KADDR_KERNEL_VMA>>30)&511)*8], KADDR_PMA(k_PDT) +3       ; Kernel's 1 GiB
    mov     qword [rdx], KADDR_PMA(k_PT) +3                                         ; 0~2 MiB
    mov     rbx, KADDR_PMA(_kernel_end)
    mov     rax, 3
.1:
    mov     [rcx], rax
    add     rax, 4096
    add     rcx, 8
    cmp     rax, rbx
    jbe     .1
    mov     cr3, rsi
    ; eip -> higher end of address space
    mov     rax, qword .HigherHalf
    jmp     rax
.HigherHalf:
    mov     rdi, qword k_PML4T
    mov     qword [rdi], 0                                                  ; Remove First 512 GiB
    mov     rdi, qword k_PDPT
    mov     qword [rdi], 0                                                  ; Remove 0~1 GiB
    mov     cr3, rsi
    ; Load GDT & Reload selectors
    mov     rdi, qword gdtr
    lgdt    [rdi]
    mov     eax, SEG_DATA64_0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    mov     rsp, qword kstack + KSTACK_SIZE
    mov     rcx, qword .reloadcs
    mov     rsi, rsp
    push    rax             ; new SS
    push    rsi             ; new RSP
    push    2               ; new FLAGS
    push    SEG_CODE64_0    ; new CS
    push    rcx             ; new RIP
    iretq
.reloadcs:
    ; TSS
    mov     rdi, qword gdt
    mov     rsi, qword tss
    mov     rdx, qword ist + IST_SIZE
    mov     [rsi + tss64_ist1], rdx
    add     rdx, IST_SIZE
    mov     [rsi + tss64_ist2], rdx
    add     rdx, IST_SIZE
    mov     [rsi + tss64_ist3], rdx
    add     rdx, IST_SIZE
    mov     [rsi + tss64_ist4], rdx
    add     rdx, IST_SIZE
    mov     [rsi + tss64_ist5], rdx
    add     rdx, IST_SIZE
    mov     [rsi + tss64_ist6], rdx
    add     rdx, IST_SIZE
    mov     [rsi + tss64_ist7], rdx
    mov     edx, esi
    mov     eax, esi
    mov     ebx, esi
    shl     eax, 16
    or      eax, 103                            ; eax = Base[15..00] Limit[15..00]
    shr     edx, 16
    and     ebx, 0xFF000000
    and     edx, 0x000000FF
    or      edx, ebx
    or      edx, 0x00008900                     ; [G=0][AVL=0][P][DPL=0][TYPE=1001][00]
    shr     rsi, 32
    mov     [rdi+SEG_TSS], eax
    mov     [rdi+SEG_TSS+4], edx
    mov     [rdi+SEG_TSS+8], esi
    mov     dword [rdi+SEG_TSS+12], 0
    mov     eax, SEG_TSS
    ltr     ax
    ; Setup IDT
    call    idt_init
    ; Setup minimal C environment
    xor     ebp, ebp
    ; Constructors
    mov     rbx, ctor_start
    jmp     .ctors_until_end
.call_constructor:
    call    [rbx]
    add     rbx,8
.ctors_until_end:
    mov     rdi, ctor_end
    cmp     rbx, rdi
    jb      .call_constructor
    ; invoke C main
    call    kmain
    ; Destructors
    mov     rbx, dtor_end
    jmp     .dtors_until_end
.call_destructor:
    sub     rbx, 8
    call    [rbx]
.dtors_until_end:
    mov     rdi, dtor_start
    cmp     rbx, rdi
    ja      .call_destructor
    ; should not return from main, but just in case we halt here
.halt:
    cli
    hlt
    jmp     .halt
