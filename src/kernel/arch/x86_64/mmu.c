#include <string.h>
#include "inlineasm.h"
#include "kdebug.h"
#include "mmu.h"

#define KADDR_MMU_PT         (0xFFFF000000000000UL + (MMU_RECURSIVE_SLOT << 39))
#define KADDR_MMU_PD         (KADDR_MMU_PT         + (MMU_RECURSIVE_SLOT << 30))
#define KADDR_MMU_PDPT       (KADDR_MMU_PD         + (MMU_RECURSIVE_SLOT << 21))
#define KADDR_MMU_PML4       (KADDR_MMU_PDPT       + (MMU_RECURSIVE_SLOT << 12))
#define MMU_PML4_INDEX(addr) ((((uintptr_t)(addr))>>39) & 511)
#define MMU_PDPT_INDEX(addr) ((((uintptr_t)(addr))>>30) & 511)
#define MMU_PD_INDEX(addr)   ((((uintptr_t)(addr))>>21) & 511)
#define MMU_PT_INDEX(addr)   ((((uintptr_t)(addr))>>12) & 511)
#define MMU_PML4(addr)       ((uint64_t*) KADDR_MMU_PML4 )
#define MMU_PDPT(addr)       ((uint64_t*)(KADDR_MMU_PDPT + ((((uintptr_t)(addr))>>27) & 0x00001FF000)))
#define MMU_PD(addr)         ((uint64_t*)(KADDR_MMU_PD   + ((((uintptr_t)(addr))>>18) & 0x003FFFF000)))
#define MMU_PT(addr)         ((uint64_t*)(KADDR_MMU_PT   + ((((uintptr_t)(addr))>>9)  & 0x7FFFFFF000)))

// PAGE TABLE FLAG
#define MMU_PAGE_ONDEMAND    (0x0100)
#define MMU_PAGE_MAPPHY      (0x0200)

extern uint64_t k_PML4T[], k_PDPT[], k_PDT[];
extern char _kernel_start, _kernel_end;

// -------------------------------------------------
uint64_t MMU_PHYPOOL_PD[512] __attribute__((aligned(4096)));
uint64_t MMU_PHYPOOL_PT[512] __attribute__((aligned(4096)));
uint64_t MMU_PHYPOOL_FIRSTPAGE[512] __attribute__((aligned(4096)));
// we just store list of free page in an array
_SPINLOCK             MMU_lock = 0;
volatile MMU_PHYADDR* MMU_phypool = (MMU_PHYADDR*)KADDR_MMU_PHYPOOL;
volatile unsigned int MMU_phypool_index = 0;

// NOTE: implemented below
bool mmu_mark(const void* addr, MMU_PHYADDR paddr, uint64_t flag);
// -------------------------------------------------
bool mmu_init(const BOOTDATA* boot) {
    const BIOSMEMORYMAP* map = boot->biosmm;
    uint64_t kend         = (KADDR_PMA((uintptr_t)&_kernel_end)) + 4096;
    uint64_t initrd_start = (uint64_t) boot->initrd_addr;
    uint64_t initrd_end   = (uint64_t) (initrd_start + boot->initrd_size + 4096);
    uint64_t start, end;
    uint32_t pt_index = 0;
    // Install recursive page directory
    k_PML4T[MMU_PML4_INDEX(KADDR_MMU_PML4)] = KADDR_PMA(k_PML4T) +3;
    // entry for MMU_STACK
    k_PDPT[MMU_PDPT_INDEX(KADDR_MMU_PHYPOOL)] = KADDR_PMA(MMU_PHYPOOL_PD) +3;
    MMU_PHYPOOL_PD[0] = KADDR_PMA(MMU_PHYPOOL_PT) +3;
    MMU_PHYPOOL_PT[0] = KADDR_PMA(MMU_PHYPOOL_FIRSTPAGE) +3;
    _MOVCR3(KADDR_PMA(k_PML4T));
    // Build stack of available physical page
    MMU_phypool_index = 0;
    for (uint32_t i=0; i < boot->biosmm_count; i++) {
        if ( map[i].type != 1 ) continue;
        start = ((map[i].base + 4095) >>12) <<12;            // align to 4K
        end   = ((map[i].base + map[i].size) >>12) <<12;
        for (; start < end; start += 4096) {
            // skip memory below kernel
            if (start < kend) continue;
            // skip initrd
            if (start >= initrd_start && start <= initrd_end) continue;
            if (pt_index != (MMU_phypool_index >> 9)) {
                pt_index = (MMU_phypool_index >> 9);
                mmu_mark((const void*)&MMU_phypool[MMU_phypool_index], 0, MMU_PAGE_ONDEMAND);
            }
            // upon setting MMU_phypool[512] a page fault occur and demand allocated
            MMU_phypool[MMU_phypool_index] = (uint64_t)start;
            MMU_phypool_index++;
        }
    }
    return true;
}
MMU_PHYADDR mmu_alloc(void) {
    MMU_PHYADDR addr = 0;
    _INT_DISABLE();
    _SPIN_LOCK(&MMU_lock);
    if (MMU_phypool_index == 0) goto fail;
    MMU_phypool_index--;
    addr = MMU_phypool[MMU_phypool_index];
fail:
    _SPIN_UNLOCK(&MMU_lock);
    _INT_RESTORE();
    return addr;
}
void mmu_free(MMU_PHYADDR addr) {
    _INT_DISABLE();
    _SPIN_LOCK(&MMU_lock);
    MMU_phypool[MMU_phypool_index] = addr;
    MMU_phypool_index++;
    _SPIN_UNLOCK(&MMU_lock);
    _INT_RESTORE();
}
bool mmu_mark(const void* addr, MMU_PHYADDR paddr, uint64_t flag) {
    uint64_t pml4_index = MMU_PML4_INDEX(addr);
    uint64_t pdpt_index = MMU_PDPT_INDEX(addr);
    uint64_t pd_index   = MMU_PD_INDEX(addr);
    uint64_t pt_index   = MMU_PT_INDEX(addr);
    uint64_t* pml4 = MMU_PML4(addr);
    uint64_t* pdpt = MMU_PDPT(addr);
    uint64_t* pd   = MMU_PD(addr);
    uint64_t* pt   = MMU_PT(addr);
    if ((pml4[pml4_index] & (MMU_PROT_PRESENT|MMU_PAGE_ONDEMAND)) == 0) {
        pml4[pml4_index] = MMU_PAGE_ONDEMAND|MMU_PROT_RW|MMU_PROT_USER;
    }
    if ((pdpt[pdpt_index] & (MMU_PROT_PRESENT|MMU_PAGE_ONDEMAND) ) == 0) {
        pdpt[pdpt_index] = MMU_PAGE_ONDEMAND|MMU_PROT_RW|MMU_PROT_USER;
    }
    if ((pd[pd_index] & (MMU_PROT_PRESENT|MMU_PAGE_ONDEMAND) ) == 0) {
        pd[pd_index] = MMU_PAGE_ONDEMAND|MMU_PROT_RW|MMU_PROT_USER;
    }
    if ((pt[pt_index] & MMU_PROT_PRESENT) == 0) {
        if ((flag & MMU_PAGE_MAPPHY) == 0) {
            pt[pt_index] = (uint64_t) (MMU_PAGE_ONDEMAND | (flag & MMU_PROT_MASK));
            // kprintf ("Mark addr:%X PTE[%d] = %X\n", addr, pt_index, pt[pt_index]);
        } else {
            pt[pt_index] = (uint64_t) (paddr | (flag & MMU_PROT_MASK) | MMU_PROT_PRESENT);
            _INVLPG(addr);
        }
    } else {
        if ((flag & MMU_PAGE_MAPPHY) == 0) {
            pt[pt_index] = (pt[pt_index] & (~(uint64_t)MMU_PROT_MASK)) | ((uint64_t)flag & MMU_PROT_MASK) | MMU_PROT_PRESENT;
        } else {
            kprintf("    MMU : map fail, addr:%X paddr:%X flag=%d entry:%X\n", addr, paddr, flag, pt[pt_index]);
            return false;
        }
    } return true;
}
bool mmu_mmap(const void* mem, MMU_PHYADDR paddr, size_t size, unsigned int flag) {
    for (size_t off = 0; off < size; off += 4096) {
        if (!mmu_mark((const uint8_t*)mem + off, paddr+off, flag)) return false;
    } return true;
}
bool mmu_munmap(const void* mem, size_t size, unsigned int flag) {
    for (size_t off = 0; off < size; off += 4096) {
        const void* addr  = (const uint8_t*)mem + off;
        uint64_t* pt  = MMU_PT(addr);
        uint64_t entry = pt[MMU_PT_INDEX(addr)];
        pt[MMU_PT_INDEX(addr)] = 0;
        if ((entry & MMU_PROT_PRESENT) != 0) {
            if ((entry & MMU_PROT_PRESENT) != 0 && (flag & MMU_MUNMAP_NORELEASE) == 0) {
                entry &= 0xFFFFFFFFFFFFF000;
                if (entry) {
                    mmu_free(entry);
                    _INVLPG(addr);
                }
            }
        }
    }
    return true;
}
void INT_0E(uint64_t code, uint64_t addr, uint64_t ip) {
    uint64_t  page, prot;
    uint64_t* pt;
    //    kprintf ("  INT0E : #PF Page Fault Exception. IP:%X CODE:%d ADDR:%X\n"
    //         "        : PML4[%d] PDPT[%d] PD[%d] PT[%d]\n", ip, code, addr,
    //         MMU_PML4_INDEX(addr), MMU_PDPT_INDEX(addr), MMU_PD_INDEX(addr), MMU_PT_INDEX(addr) );
    pt = MMU_PT(addr);
    if ((code & 1) == 0) {  // Page Not Present
        if ((pt[MMU_PT_INDEX(addr)] & MMU_PAGE_ONDEMAND) == 0) {
            kprintf("  INT0E : #PF Page Fault Exception. IP:%X CODE:%d ADDR:%X\n"
                    "        : PML4[%d] PDPT[%d] PD[%d] PT[%d]\n", ip, code, addr,
                    MMU_PML4_INDEX(addr), MMU_PDPT_INDEX(addr), MMU_PD_INDEX(addr), MMU_PT_INDEX(addr) );
            kprintf("    #PF : Access to unallocated memory. CODE: %d\n", code);
            kprintf("        : ADDR: %X PT[%d]: %X\n", addr, MMU_PT_INDEX(addr), pt[MMU_PT_INDEX(addr)]);
            __asm volatile ("cli; hlt");
        }
        page = mmu_alloc();
        // kprintf ("PTE: %X, %d, MMU_sp=%d, page=%X\n", pte, (addr>>12)&511, MMU_sp, page);
        prot = pt[MMU_PT_INDEX(addr)] & MMU_PROT_MASK;
        pt[MMU_PT_INDEX(addr)] = page | prot | MMU_PROT_PRESENT;
        _INVLPG((const void*)addr);
        memset((void*)((addr >> 12) << 12), 0, 4096);
    } else {
        kprintf("  INT0E : #PF Page Fault Exception. IP:%X CODE:%d ADDR:%X\n"
                "        : PML4[%d] PDPT[%d] PD[%d] PT[%d]\n", ip, code, addr,
                MMU_PML4_INDEX(addr), MMU_PDPT_INDEX(addr), MMU_PD_INDEX(addr), MMU_PT_INDEX(addr) );
        kprintf("      #PF : Access to protected memory. CODE: %d\n", code);
        kprintf("          : ADDR: %X PTE[%d]: %X\n", addr, MMU_PT_INDEX(addr), pt[MMU_PT_INDEX(addr)]);
        __asm volatile ("cli; hlt");
    }
    // kprintf ("INT0E : EXIT. CODE:%d ADDR:%X\n", code, addr);
}
