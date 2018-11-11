;
; Boot Loader for Bluemoon OS
;
; Useful Offset:
;   0000:7C00  VBR
;   0000:7E00  This Stage
;   0000:0600  Boot Data Block
;
; This loader does:
; 1. Enable A20
; 2. Get BIOS memory map and store at Boot Data Block
; 3. Detect CPU
; 4. Load system files and machine initialization
;    32 BIT CPU:
;        - Load kernel32.bin and initrd32.bin
;        - Setup Protected mode, no paging
;    64 BIT CPU:
;        - Load kernel64.bin and initrd64.bin
;        - Setup Long Mode, identity mapped PM4L
;    * Loaded offset and size is record in Boot Data Block
; 5. Parse ELF32/ELF64 executable and JMP to it.
; PS. BSS is not cleared. Kernel need to clear it after setup logical address.
;
; Assemble with:
; nasm -f bin -o boot.bin boot.asm
;

cpu     x86-64
org     0x7E00

; ----------------------------------------------
struc BPB                 ; BIOS Paramater Block (At Boot Record)
    resb  3               ; JMP
    BPB_name    resb  8   ; Volume OEM Name eg. "mkdosfs"
    BPB_bps     resw  1   ; Bytes per Sector
    BPB_spc     resb  1   ; Sectors per Cluster
    BPB_res     resw  1   ; Reserved sector before 1st FAT
    BPB_nfat    resb  1   ; number of FATs
    BPB_maxroot resw  1   ; max number of root entry
    BPB_total1  resw  1   ; total sectors, if 0 use total2
    BPB_media   resb  1   ; media descriptor, F8 for HDD
    BPB_spf     resw  1   ; sectors per FAT
    BPB_spt     resw  1   ; sectors per track (63)
    BPB_head    resw  1   ; num of head
    BPB_lba     resd  1   ; starting sector of this vol
    BPB_total2  resd  1   ; total sectors if total1 == 0
endstruc
struc ELF32_Ehdr
    elf32_e_ident       resb    16
    elf32_e_type        resw    1
    elf32_e_machine     resw    1
    elf32_e_version     resd    1
    elf32_e_entry       resd    1
    elf32_e_phoff       resd    1
    elf32_e_shoff       resd    1
    elf32_e_flags       resd    1
    elf32_e_ehsize      resw    1
    elf32_e_phentsize   resw    1
    elf32_e_phnum       resw    1
    elf32_e_shentsize   resw    1
    elf32_e_shnum       resw    1
    elf32_e_shstrndx    resw    1
endstruc
struc ELF32_Phdr
    elf32_p_type        resd    1
    elf32_p_offset      resd    1
    elf32_p_vaddr       resd    1
    elf32_p_paddr       resd    1
    elf32_p_filesz      resd    1
    elf32_p_memsz       resd    1
    elf32_p_flags       resd    1
    elf32_p_align       resd    1
endstruc
struc ELF32_Shdr
    elf32_sh_name       resd    1
    elf32_sh_type       resd    1
    elf32_sh_flags      resd    1
    elf32_sh_addr       resd    1
    elf32_sh_offset     resd    1
    elf32_sh_size       resd    1
    elf32_sh_link       resd    1
    elf32_sh_info       resd    1
    elf32_sh_addralign  resd    1
    elf32_sh_entsize    resd    1
endstruc
struc ELF64_Ehdr
    elf64_e_ident       resb    16
    elf64_e_type        resw    1
    elf64_e_machine     resw    1
    elf64_e_version     resd    1
    elf64_e_entry       resq    1
    elf64_e_phoff       resq    1
    elf64_e_shoff       resq    1
    elf64_e_flags       resd    1
    elf64_e_ehsize      resw    1
    elf64_e_phentsize   resw    1
    elf64_e_phnum       resw    1
    elf64_e_shentsize   resw    1
    elf64_e_shnum       resw    1
    elf64_e_shstrndx    resw    1
endstruc
struc ELF64_Phdr
    elf64_p_type        resd    1
    elf64_p_flags       resd    1
    elf64_p_offset      resq    1
    elf64_p_vaddr       resq    1
    elf64_p_paddr       resq    1
    elf64_p_filesz      resq    1
    elf64_p_memsz       resq    1
    elf64_p_align       resq    1
endstruc
struc ELF64_Shdr
    elf64_sh_name       resd    1
    elf64_sh_type       resd    1
    elf64_sh_flags      resq    1
    elf64_sh_addr       resq    1
    elf64_sh_offset     resq    1
    elf64_sh_size       resq    1
    elf64_sh_link       resd    1
    elf64_sh_info       resd    1
    elf64_sh_addralign  resq    1
    elf64_sh_entsize    resq    1
endstruc

SHT_NOBITS      equ     8


; Defines
; ----------------------------------------------
VBR             equ     0x7C00
OpenFile        equ     (VBR + 512 - 2 - 4 )
ReadFile        equ     (VBR + 512 - 2 - 2)
BOOTDATA        equ     0x0600
FATBUFFER       equ     BOOTDATA  +512
DISKBUFFER      equ     FATBUFFER +512
KERNEL_LOADAT   equ     0x100000
INITRD_LOADAT   equ     0x400000


section .text
bits 16
; ----------------------------------------------
; Entry Point
; Note: This stage is loaded by VBR at 0000:7C00
;       This stage itself is located at 0000:7E00
; ----------------------------------------------
start:
    ; Machine Initialization
    cli
    xor     ax, ax
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, 0x7C00
    jmp     0:.relo
.relo:
    push    word 2
    popf
    mov     [drive], dl
    ; clear screen
    mov     ax, 0xB800
    mov     es, ax
    xor     ax, ax
    xor     di, di
    mov     cx, 80*25*2
    rep     stosw
    mov     es, ax
    ; Enable A20 (macbook will hang with keyb controller IO)
    mov     ax, 0x2401
    int     0x15
    ; Gather boot information
    call    GetMemoryMap
    ; Detect CPU Mode
    call    DetectCPU
    or      ax, ax
    jnz     .boot64
; Boot 32-bit
; ----------------------------------------------
.boot32:
    mov     si, MsgBoot32
    call    print
    ; Load kernel & Initrd
    mov     dl, [drive]
    lea     si, [kernel32]
    mov     ebp, KERNEL_LOADAT
    mov     [BOOTDATA], ebp
    call    LoadFile
    sub     ebp, KERNEL_LOADAT - 4095
    and     ebp, 0xFFFFFF00
    mov     [BOOTDATA+4], ebp
    lea     si, [initrd32]
    mov     ebp, INITRD_LOADAT
    mov     [BOOTDATA+8], ebp
    call    LoadFile
    sub     ebp, INITRD_LOADAT - 4095
    and     ebp, 0xFFFFFF00
    mov     [BOOTDATA+12], ebp
    ; Enter protected mode
    cli
    lgdt    [cs:gdtr]
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax
    jmp     0x08:start32
; Boot 64-bit
; ----------------------------------------------
.boot64:
    mov     si, MsgBoot64
    call    print
    ; Load kernel & Initrd
    mov     dl, [drive]
    lea     si, [kernel64]
    mov     ebp, KERNEL_LOADAT
    mov     [BOOTDATA], ebp
    call    LoadFile
    sub     ebp, KERNEL_LOADAT - 4095
    and     ebp, 0xFFFFF000
    mov     [BOOTDATA+4], ebp
    lea     si, [initrd64]
    mov     ebp, INITRD_LOADAT
    mov     [BOOTDATA+8], ebp
    call    LoadFile
    sub     ebp, INITRD_LOADAT - 4095
    and     ebp, 0xFFFFF000
    mov     [BOOTDATA+12], ebp
    ; Disable interrupt and setup structures
    cli
    ; Setup long mode and invoke kernel
    ; Page Structure
    ; PML4T : 0x1000
    ; PDPT  : 0x2000
    ; PDT   : 0x3000
    ; PT    : 0x4000
    mov     edi, 0x1000
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 4096
    rep     stosd
    mov     edi, cr3
    mov     dword [edi], 0x2003
    add     edi, 0x1000
    mov     dword [edi], 0x3003
    add     edi, 0x1000
    mov     dword [edi], 0x4003
    add     edi, 0x1000
    mov     ebx, 3
    mov     ecx, 512
.PTEntry:
    mov     [edi], ebx
    add     ebx, 0x1000
    add     edi, 8
    loop    .PTEntry
    ; Enable PAE
    mov     eax, cr4
    or      eax, 1<<5
    mov     cr4, eax
    ; Enable Long Mode
    mov     ecx, 0xC0000080
    rdmsr
    or      eax, 1<<8
    wrmsr
    ; Enter compatibility mode & invoke kernel
    lgdt    [cs:gdtr]
    mov     eax, cr0
    or      eax, (1<<31) | (1<<0)
    mov     cr0, eax
    jmp     0x18:start64

; ----------------------------------------------
; Detect Long-mode Support
; ----------------------------------------------
DetectCPU:
    ; First, check if CUPID is supported
    pushfd
    pop     eax
    mov     ecx, eax
    xor     eax, 1 << 21    ; Flip the ID-bit, which is bit 21.
    push    eax
    popfd
    pushfd
    pop     eax
    push    ecx
    popfd
    xor     eax, ecx
    jz      .NoCPUID
    ; Check if extended CPUID supported
    mov     eax, 0x80000000
    cpuid
    cmp     eax, 0x80000001
    jb      .NoCPUID
    mov     eax, 0x80000001
    cpuid
    test    edx, 1<<29      ; Test if the LM-bit is set, which is bit 29
    jz      .NoLongMode
    mov     ax, 1
    ret
.NoLongMode:
    xor     ax, ax
    ret
.NoCPUID:
    mov     si, MsgCPUNotSupported
    call    print
    jmp     halt

; ----------------------------------------------
; BIOS memory map
; ----------------------------------------------
GetMemoryMap:
    mov     [BOOTDATA+16], dword 0
    mov     di, BOOTDATA+16+4
    xor     si, si
    xor     ebx, ebx
.getnextblock:
    mov     eax, 0xE820
    mov     ecx, 24
    mov     edx, 0x534D4150
    mov     [di+20], byte 0         ; in returned from BIOS, if bit0 = 0 the ACPI entry should be ignored
    int     0x15
    jc      .done
    cmp     eax, 0x534D4150
    jne     .done
    or      ebx, ebx
    jz      .done
    add     di, byte 24
    inc     si
    jmp     .getnextblock
.done:
    mov     [BOOTDATA+16], si
    ret

; Load file from disk using BIOS
; SI = filename
; EBP = destination
; ----------------------------------------------
LoadFile:
    call    word [OpenFile]
    or      eax, eax
    jz      .diskerror
.1:
    ; It take time to load large file, animate at cursor
    call    animate
    mov     di, DISKBUFFER
    call    word [ReadFile]
    mov     esi, DISKBUFFER
    mov     edi, ebp
    add     ebp, ecx
    shr     ecx, 2
    call    MoveHigh
    or      eax, eax
    jnz     .1
.done:
    ret
.diskerror:
    mov     si, MsgFileNotFound
    call    print
    jmp     halt

; ----------------------------------------------
; Move memory to high
; ----------------------------------------------
MoveHigh:
    push    eax
    push    ebx
    lgdt    [cs:gdtr]
    mov     eax, cr0
    inc     ax
    mov     cr0, eax
    mov     bx, 0x10
    mov     ds, bx
    mov     es, bx
    a32 rep movsd                   ; copy
    dec     ax
    mov     cr0, eax
    xor     bx, bx
    mov     ds, bx
    mov     es, bx
    pop     ebx
    pop     eax
    ret

; ----------------------------------------------
; Misc Functions
; ----------------------------------------------
print:
    push    es
    push    di
    mov     ax, 0xB800
    mov     es, ax
    mov     di, [screen_pos]
    mov     ah, 7
.1:
    lodsb
    or      al, al
    jz      .2
    cmp     al, 0x0D
    je      .nextline
    stosw
    jmp     .1
.nextline:
    push    cx
    push    ax
    lea     ax, [di +160]
    mov     cl, 160
    div     cl
    mul     cl
    mov     di, ax
    pop     ax
    pop     cx
    jmp     .1
.2:
    mov     [screen_pos], di
    pop     di
    pop     es
    ret

animate:
    push    es
    push    di
    push    ax
    push    bx
    mov     ax, 0xB800
    mov     es, ax
    mov     di, [screen_pos]
    mov     bx, [AnimatorIndex]
    inc     word [AnimatorIndex]
    and     bx, 3
    mov     ah, 7
    mov     al, [Animator +bx]
    stosw
    pop     bx
    pop     ax
    pop     di
    pop     es
    ret

; ----------------------------------------------
halt:
    sti
    hlt
    jmp     halt

; ----------------------------------------------
; DATA AREA
; ----------------------------------------------
align 16
gdtr: dw 8*5-1
      dd gdt
      dd 0
      dw 0
align 16
gdt:  dd 0, 0
      dd 0x0000FFFF, 0x00CF9A00               ; CODE32 Base:0 Limit:FFFFF CODE+READ+32BIT+4K
      dd 0x0000FFFF, 0x00CF9200               ; DATA32 Base:0 Limit:FFFFF DATA+RW+32BIT+4K
      dd 0x00000000, 0x00209A00               ; CODE64
      dd 0x00000000, 0x00009200               ; DATA64

lba_fat         dd 0
lba_data        dd 0
fat_cache       dd 0
screen_pos      dw 0
AnimatorIndex   dw 0
drive           db 0

kernel32            db 'KERNEL32BIN', 0x20          ; 20 = Archive
kernel64            db 'KERNEL64BIN', 0x20
initrd32            db 'INITRD32BIN', 0x20
initrd64            db 'INITRD64BIN', 0x20

MsgBoot32           db  'Booting 32-bit kernel...', 0x0D, 0
MsgBoot64           db  'Booting 64-bit kernel...', 0x0D, 0
MsgCPUNotSupported  db  'CPU not supported. System halt.', 0
MsgFileNotFound     db  'Essential system file not found. System halt.', 0
Animator            db  '|/-\'

entry32 dd  0
entry64 dq  0

; ----------------------------------------------
; 32bit Entry
; ----------------------------------------------
bits 32
align 16
start32:
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    mov     esp, 0x7C00
    ; Parse ELF32
    mov     ebp, KERNEL_LOADAT                      ; ELF.ehdr
    ; Resolve Entry
    mov     edi, [ebp + elf32_e_entry]              ; Entry logical address
    call    ELF32_LMA2PMA
    jmp     edi
    ;push    edi
    ; Clear BSS
    ;mov     esi, [ebp + elf32_e_shoff]              ; Section Header Offset
    ;movzx   edx, word [ebp + elf32_e_shnum]         ; Section count
    ;movzx   ebx, word [ebp + elf32_e_shentsize]     ; Size of Section Header
    ;add     esi, ebp                                ; Section Header
    ;or      edx, edx
    ;jmp     .done_bss
;.shdr_next:
    ;mov     edi, [esi + elf32_sh_type]
    ;cmp     edi, 8
    ;jne     .shdr_skip
    ;mov     edi, [esi + elf32_sh_addr]
    ;mov     ecx, [esi + elf32_sh_size]
    ;call    ELF32_LMA2PMA
    ;shr     ecx, 3
    ;xor     eax, eax
    ;rep     stosd
    ;jmp     .done_bss
;.shdr_skip:
    ;add     esi, ebx
    ;dec     edx
    ;jnz     .shdr_next
;.done_bss:
    ;pop     eax
    ;jmp     eax

ELF32_LMA2PMA:
    push    ebx
    push    ecx
    push    edx
    push    esi
    mov     esi, [ebp + elf32_e_phoff]              ; Program Header Offset
    movzx   edx, word [ebp + elf32_e_phnum]         ; Program Header count
    movzx   ebx, word [ebp + elf32_e_phentsize]     ; Size of Program Header
    add     esi, ebp                                ; Program Header
    or      edx, edx
    jz      .phdr_fail
.phdr_next:
    mov     eax, [esi + elf32_p_vaddr]              ; Logical address of program section
    mov     ecx, [esi + elf32_p_memsz]
    add     ecx, eax                                ; End of section
    cmp     edi, eax
    jb      .phdr_skip
    cmp     edi, ecx
    jbe     .phdr_found
.phdr_skip:
    add     esi, ebx
    dec     edx
    jnz     .phdr_next
.phdr_fail:
    mov     edi, 0xB8000
    mov     eax, 0x1F211F21                         ; Full screen of !!!!!
    mov     ecx, 80*25/2
    rep     stosd
    hlt
.phdr_found:
    sub     edi, eax                                ; entry is within this program section
    add     edi, [esi + elf32_p_offset]             ; address = entry(VMA) - section(VMA) + section(offset) + file offset
    add     edi, ebp
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    ret

; ----------------------------------------------
; 64bit Entry
; ----------------------------------------------
bits 64
align 16
start64:
    mov     ax, 0x20
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    mov     rsp, 0x7C00
    ; Parse ELF64
    mov     rbp, KERNEL_LOADAT                      ; ELF.ehdr
    ; Resolve Entry
    mov     rdi, [rbp + elf64_e_entry]              ; Entry logical address
    call    ELF64_LMA2PMA
    ;push    rdi
    jmp     rdi
    ; Clear BSS
    ; mov     rsi, [rbp + elf64_e_shoff]              ; Section Header Offset
    ; movzx   rdx, word [rbp + elf64_e_shnum]         ; Section count
    ; movzx   rbx, word [rbp + elf64_e_shentsize]     ; Size of Section Header
    ; add     rsi, rbp                                ; Section Header
    ; or      rdx, rdx
    ; jmp     .done_bss
; .shdr_next:
    ; mov     edi, [rsi + elf64_sh_type]
    ; cmp     edi, 8
    ; jne     .shdr_skip
    ; mov     rdi, [rsi + elf64_sh_addr]
    ; mov     rcx, [rsi + elf64_sh_size]
    ; call    ELF64_LMA2PMA
    ; shr     rcx, 3
    ; xor     rax, rax
    ; rep     stosq
    ; jmp     .done_bss
;.shdr_skip:
    ;add     rsi, rbx
    ;dec     rdx
    ;jnz     .shdr_next
;.done_bss:
    ;pop     rax
    ;jmp     rax

ELF64_LMA2PMA:
    push    rbx
    push    rcx
    push    rdx
    push    rsi
    mov     rsi, [rbp + elf64_e_phoff]              ; Program Header Offset
    movzx   rdx, word [rbp + elf64_e_phnum]         ; Program Header count
    movzx   rbx, word [rbp + elf64_e_phentsize]     ; Size of Program Header
    add     rsi, rbp                                ; Program Header
    or      rdx, rdx
    jz      .phdr_fail
.phdr_next:
    mov     rax, [rsi + elf64_p_vaddr]              ; Logical address of program section
    mov     rcx, [rsi + elf64_p_memsz]
    add     rcx, rax                                ; End of section
    cmp     rdi, rax
    jb      .phdr_skip
    cmp     rdi, rcx
    jbe     .phdr_found
.phdr_skip:
    add     rsi, rbx
    dec     rdx
    jnz     .phdr_next
.phdr_fail:
    mov     edi, 0xB8000
    mov     rax, 0x1F211F211F211F21                 ; Full screen of !!!!!
    mov     ecx, 80*25/4
    rep     stosq
    hlt
.phdr_found:
    sub     rdi, rax                                ; entry is within this program section
    add     rdi, [rsi + elf64_p_offset]             ; address = entry(VMA) - section(VMA) + section(offset) + file offset
    add     rdi, rbp
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rbx
    ret
