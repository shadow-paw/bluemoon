; nasm -f bin -o boot0.bin boot0.asm
; dd if=boot0.bin of=bluemoon.img bs=1 skip=62 seek=1048638 count=450
; where 1048638 = 2048(start sector) x 512 + 62

cpu 386
bits 16
org 0x7C00

; ----------------------------------------------
struc BPB                 ; BIOS Paramater Block (At Boot Record)
                resb  3   ; JMP
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

diskcache   equ     0x1000
stage2      equ     0x7E00

section .text
; ----------------------------------------------
vbr:
    jmp short start
    nop
    TIMES   0x3B  db  0     ; BIOS PARA BLOCK
start:
    jmp   0:.fix_csip
.fix_csip:
    xor     ebx, ebx
    mov     ds, bx
    mov     es, bx
    mov     ss, bx
    mov     sp, 0x7C00        ; stack @ 0000:7C00
    push    word 2
    popf
    ; clear screen
    mov     ax, 0xB800
    mov     es, ax
    xor     ax, ax
    xor     di, di
    mov     cx, 80*25*2
    rep     stosw
    mov     es, ax
    ; --------------------------------------
    ; Calculate LBA offset
    ; --------------------------------------
    mov     eax, [vbr + BPB_lba]            ; eax = LBA of volume
    movzx   ebx, word [vbr + BPB_res]       ; bx = sectors for [reserved]
    add     eax, ebx                        ; eax = LBA of FAT
    mov     [lba_fat], eax

    movzx   ebx, word [vbr + BPB_spf]       ; sectors per FAT
    movzx   cx,  byte [vbr + BPB_nfat]      ; number of FAT
    imul    ebx, ecx                        ; ebx = total size of all FAT
    add     eax, ebx                        ; eax = LBA of root directory entry
    mov     [lba_rootdir], eax

    movzx   ecx, word [vbr + BPB_maxroot]   ; calculate size of root directory, in sector count
    shr     cx, 4                           ; cx = sector count of RootDir, cx=cx*32/512, assume 512 bytes per sector
    add     eax, ecx                        ; ebp = LBA of data area
    mov     [lba_data], eax

    mov     si, filename
    call    OpenFile
    or      eax, eax
    jz      .diskerror

    mov     di, stage2
.1:
    call    ReadFile
    or      eax, eax
    jnz     .1
    jmp     stage2

.diskerror:
    int     0x18


; ----------------------------------------------
; HANDLE OpenFile(const char* filename);
; INPUT
;    si -> filename
; OUTPUT
;    eax = HANLDE of file, 0 if not found
; CLOBBER
;    bx, cx, di
; ----------------------------------------------
OpenFile:
    mov     eax, [lba_rootdir]
    mov     cx, word [vbr + BPB_maxroot]    ; calculate size of root directory, in sector count
    push    cx
    shr     cx, 4
    mov     di, diskcache
    call    CacheReadSectors                ; read cx sectors into di
    ; --------------------------------------
    ; Find file
    ; --------------------------------------
    pop     bx                              ; number of root entry, each 32 bytes
.findfile:
    mov     cx, 12
    push    si
    push    di
    repe cmpsb
    pop     di
    pop     si
    je      OpenFile_found
    add     di, 32
    dec     bx
    jnz     .findfile
ReturnFail:
    xor     eax, eax
    ret
OpenFile_found:
    movzx   eax, word [di+26]               ; first cluster number
    ret

; ----------------------------------------------
; HANDLE ReadFile(HANDLE handle);
; INPUT
;    eax = HANLDE of file
;    di -> Data Buffer
; OUTPUT
;    eax = HANLDE of file (Updated), 0 if end of file
;    ecx = bytes read
; CLOBBER
;    si, di
; ----------------------------------------------
ReadFile:
    cmp     ax, 2
    jb      ReturnFail
    cmp     ax, 0xFFF8
    jae     ReturnFail
    push    bx
    ; Load a cluster
    push    ax
    dec     ax
    dec     ax                              ; ax = cluster to load
    movzx   ecx, byte [vbr + BPB_spc]       ; sector per cluster
    imul    eax, ecx                        ;
    add     eax, [lba_data]                 ; eax = DATA + (cluster-2)*spc
    call    ReadSectors
    shl     cx, 9                           ; bytes loaded
    add     di, cx                          ; advance destination buffer pointer
    ; Load next entry from FAT
    pop     bx
    push    cx
    push    di
    movzx   eax, bh
    add     eax, [lba_fat]
    mov     cx, 1
    mov     di, diskcache
    call    CacheReadSectors
    pop     di
    pop     cx
    movzx   si, bl
    add     si, si                      ; 16bit entry
    movzx   eax, word [si+diskcache]
    pop     bx
    ret

; ----------------------------------------------
; Function: ReadSectors
;
;   Use BIOS INT13 Extended Read to read disk sectors
;
; Input:
;   EAX = LBA of sector
;   CX  = Number to sector to read
;   DL  = Drive (eg. 0x80)
;   ES:DI = Buffer
; Affected Registers:
;   CF  = Set on error, Clear if no error
; ----------------------------------------------
CacheReadSectors:
    cmp     eax, [lba_cache]
    je      cached
    mov     [lba_cache], eax
ReadSectors:
    push    ax
    push    si
    push    dword 0
    push    eax
    push    es
    push    di
    push    cx
    push    word 0x0010
    mov     si, sp
    mov     ah, 0x42
    int     0x13
    add     sp, 16
    pop     si
    pop     ax
cached:
    ret

; ----------------------------------------------
lba_fat         dd  0
lba_rootdir     dd  0
lba_data        dd  0
lba_cache       dd  0
filename        db  'BOOT    BIN', 0x20 ; 20 = Archive
; ----------------------------------------------
    TIMES   510-($-$$)-4    db 0x90
    dw  OpenFile
    dw  ReadFile
    dw 0xAA55
