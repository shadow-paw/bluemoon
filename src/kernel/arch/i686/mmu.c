#include "inlineasm.h"
#include "kdebug.h"
#include "mmu.h"

// // TODO: Versioning on pagedir, and sync when context switch
#define MMU_PD_INDEX(addr) ((((uintptr_t)(addr))>>22) & 1023)
#define MMU_PT_INDEX(addr) ((((uintptr_t)(addr))>>12) & 1023)
#define KADDR_MMU_PT       (MMU_RECURSIVE_SLOT << 22)
#define KADDR_MMU_PD       (KADDR_MMU_PT + (MMU_RECURSIVE_SLOT << 12))
#define MMU_PD(addr)       ((uint32_t*)(KADDR_MMU_PD))
#define MMU_PT(addr)       ((uint32_t*)((uintptr_t)KADDR_MMU_PT + ((((uintptr_t)(addr))>>10)&0x3FF000)))

extern uint32_t k_PDT[];
extern char _kernel_start, _kernel_end;

// -------------------------------------------------
uint32_t MMU_PHYPOOL_PT[1024] __attribute__((aligned(4096)));
uint32_t MMU_PHYPOOL_FIRSTPAGE[1024] __attribute__((aligned(4096)));
// we just store list of free page in an array
_SPINLOCK             MMU_lock = 0;
volatile MMU_PHYADDR* MMU_phypool = (MMU_PHYADDR*)KADDR_MMU_PHYPOOL;
volatile unsigned int MMU_phypool_index = 0;  // index for next allocation

// NOTE: implemented below
bool mmu_mark(const void* addr, MMU_PHYADDR paddr, uint32_t flag);
// -------------------------------------------------
bool mmu_init(const BOOTDATA* boot) {
    const BIOSMEMORYMAP* map = boot->biosmm;
    uint64_t kend = (KADDR_PMA((uintptr_t)&_kernel_end)) + 4096;
    uint64_t initrd_start = (uint64_t) boot->initrd_addr;
    uint64_t initrd_end = (uint64_t) (initrd_start + boot->initrd_size + 4096);
    uint64_t start, end;
    uint32_t pt_index = 0;
    // Install recursive page directory
    k_PDT[MMU_PD_INDEX(KADDR_MMU_PD)] = KADDR_PMA(k_PDT) +3;
    // entry for MMU_PHYPOOL
    k_PDT[MMU_PD_INDEX(KADDR_MMU_PHYPOOL)] = KADDR_PMA(MMU_PHYPOOL_PT) +3;
    MMU_PHYPOOL_PT[0] = KADDR_PMA(MMU_PHYPOOL_FIRSTPAGE) +3;
    for (int i = 1; i < 1024; i++) {
        MMU_PHYPOOL_PT[i] = MMU_PAGE_ONDEMAND;
    }
    _MOVCR3(KADDR_PMA(k_PDT));
    // Build stack of available physical page
    MMU_phypool_index = 0;
    for (uint32_t i = 0; i < boot->biosmm_count; i++) {
        if (map[i].type != 1) continue;
        start = ((map[i].base + 4095) >>12) <<12;            // align to 4K
        end = ((map[i].base + map[i].size) >>12) <<12;
        for (; start < end; start += 4096) {
            // skip memory below kernel
            if (start < kend) continue;
            // skip memory > 4GiB
            if (start >= 0x100000000) continue;
            // skip initrd
            if (start >= initrd_start && start <= initrd_end) continue;
            // upon setting MMU_phypool[512] a page fault occur and demand allocated
            if (pt_index != (MMU_phypool_index >> 9)) {
                pt_index = (MMU_phypool_index >> 9);
                mmu_mark((const void*)&MMU_phypool[MMU_phypool_index], 0, MMU_PAGE_ONDEMAND);
            }
            MMU_phypool[MMU_phypool_index] = (uint32_t) start;
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
bool mmu_mark(const void* addr, MMU_PHYADDR paddr, uint32_t flag) {
    uint32_t pd_index = MMU_PD_INDEX(addr);
    uint32_t pt_index = MMU_PT_INDEX(addr);
    uint32_t* pd = MMU_PD(addr);
    uint32_t* pt = MMU_PT(addr);
    if ((pd[pd_index] & (MMU_PROT_PRESENT|MMU_PAGE_ONDEMAND)) == 0) {
        pd[pd_index] = MMU_PAGE_ONDEMAND|MMU_PROT_RW|MMU_PROT_USER;
    }
    if ((pt[pt_index] & MMU_PROT_PRESENT) == 0) {
        if ((flag & MMU_PAGE_MAPPHY) == 0) {
            pt[pt_index] = (uint32_t) (MMU_PAGE_ONDEMAND | (flag & MMU_PROT_MASK));
        } else {
            pt[pt_index] = (uint32_t) (paddr | (flag & MMU_PROT_MASK) | MMU_PROT_PRESENT);
            _INVLPG(addr);
        }
    } else {
        if ((flag & MMU_PAGE_MAPPHY) == 0) {
            pt[pt_index] = (pt[pt_index] & (~(uint32_t)MMU_PROT_MASK)) | ((uint32_t)flag & MMU_PROT_MASK);
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
        const void* addr = (const uint8_t*)mem + off;
        uint32_t* pt = MMU_PT(addr);
        uint32_t entry = pt[MMU_PT_INDEX(addr)];
        pt[MMU_PT_INDEX(addr)] = 0;
        if ((entry & MMU_PROT_PRESENT) != 0) {
            if ((entry & MMU_PROT_PRESENT) != 0 && (flag & MMU_MUNMAP_NORELEASE) == 0) {
                entry &= 0xFFFFF000;
                if (entry) {
                    mmu_free(entry);
                    _INVLPG(addr);
                }
            }
        }
    }
    return true;
}
void INT_0E(uint32_t code, uint32_t addr, uint32_t ip) {
    uint32_t  page, prot, pd_index;
    uint32_t* pt;
    // kprintf("  INT0E : #PF Page Fault Exception. IP:%X CODE:%d ADDR:%X\n"
    //         "        : PD[%d] PT[%d]\n", ip, code, addr, MMU_PD_INDEX(addr), MMU_PT_INDEX(addr));
    pt = MMU_PT(addr);
    if ((code & 1) == 0) {  // Page Not Present
        if ((pt[MMU_PT_INDEX(addr)] & MMU_PAGE_ONDEMAND) == 0) {
            kprintf("  INT0E : #PF Page Fault Exception. IP:%X CODE:%d ADDR:%X\n"
                    "        : PD[%d] PT[%d]\n", ip, code, addr, MMU_PD_INDEX(addr), MMU_PT_INDEX(addr));
            kprintf("    #PF : Access to unallocated memory.\n");
            kprintf("        : ADDR: %X PT[%d]: %X\n", addr, MMU_PT_INDEX(addr), pt[MMU_PT_INDEX(addr)]);
            __asm volatile ("cli; hlt");
        }
        page = mmu_alloc();
        // kprintf ("PTE: %X, %d, MMU_phypool_index=%d, page=%X\n", pt[MMU_PT_INDEX(addr)], (addr>>12)&511, MMU_phypool_index, page);
        prot = pt[MMU_PT_INDEX(addr)] & MMU_PROT_MASK;
        pt[MMU_PT_INDEX(addr)] = page | prot | MMU_PROT_PRESENT;
        _INVLPG((const void*)addr);
        // TODO: clear allocated memory
        // memset((void*)((addr >> 12) << 12), 0, 4096);
        if ((pd_index=MMU_PD_INDEX(addr)) >= 512) {
            uint32_t* pd = MMU_PD(addr);
            k_PDT[pd_index] = pd[pd_index];
        }
    } else {
        kprintf("  INT0E : #PF Page Fault Exception. IP:%X CODE:%d ADDR:%X\n"
                "        : PD[%d] PT[%d]\n", ip, code, addr, MMU_PD_INDEX(addr), MMU_PT_INDEX(addr));
        kprintf("      #PF : Access to protected memory.\n");
        kprintf("          : ADDR: %X PTE[%d]: %X\n", addr, MMU_PT_INDEX(addr), pt[MMU_PT_INDEX(addr)]);
        __asm volatile ("cli; hlt");
    }
    // kprintf ("INT0E : EXIT. CODE:%d ADDR:%X\n", code, addr);
}
