#ifndef KERNEL_ARCH_I686_MMU_H_
#define KERNEL_ARCH_I686_MMU_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "kaddr.h"
#include "../../bootdata.h"

// CPU DEFINED
#define MMU_PROT_PRESENT     (0x0001)
#define MMU_PROT_RO          (0x0000)
#define MMU_PROT_RW          (0x0002)
#define MMU_PROT_USER        (0x0004)
#define MMU_PROT_MASK        (MMU_PROT_RO|MMU_PROT_RW|MMU_PROT_USER)

#define MMU_MUNMAP_RELEASE   (0)
#define MMU_MUNMAP_NORELEASE (1)

// KERNEL SPECIFIC
#define MMU_FLAG_ONDEMAND    (0x0100)
#define MMU_MMAP_MAPPHY      (0x0200)

typedef uint32_t MMU_PHYADDR;

#ifdef __cplusplus
extern "C" {
#endif
// -------------------------------------------------
    bool         mmu_init(const BOOTDATA* boot);
    MMU_PHYADDR  mmu_alloc(void);
    void         mmu_free(MMU_PHYADDR addr);
    bool         mmu_mmap(const void* mem, MMU_PHYADDR paddr, size_t size, unsigned int flag);
    bool         mmu_munmap(const void* mem, size_t size, unsigned int flag);
    MMU_PHYADDR  mmu_clonepagedir(void);
    void         mmu_setdir(uint32_t cr3, uint32_t *pver);
// -------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_ARCH_I686_MMU_H_
