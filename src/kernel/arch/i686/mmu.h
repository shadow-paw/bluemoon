#ifndef KERNEL_ARCH_I686_MMU_H_
#define KERNEL_ARCH_I686_MMU_H_

#include <stddef.h>
#include <stdint.h>
#include "kaddr.h"
#include "../../bootdata.h"

// CPU DEFINED
#define MMU_PROT_PRESENT  (0x0001)
#define MMU_PROT_RO       (0x0000)
#define MMU_PROT_RW       (0x0002)
#define MMU_PROT_USER     (0x0004)
#define MMU_PROT_MASK     (MMU_PROT_RO|MMU_PROT_RW|MMU_PROT_USER)
// PAGE TABLE FLAG
#define MMU_PAGE_ONDEMAND (0x0100)
#define MMU_PAGE_MAPPHY   (0x0200)

#define MMU_MUNMAP_RELEASE   (0)
#define MMU_MUNMAP_NORELEASE (1)

typedef uint32_t MMU_PHYADDR;

#ifdef __cplusplus
extern "C" {
#endif
// -------------------------------------------------
    int         mmu_init(const BOOTDATA* boot);
    MMU_PHYADDR mmu_alloc(void);
    void        mmu_free(MMU_PHYADDR addr);
    int         mmu_mmap(const void* mem, MMU_PHYADDR paddr, size_t size, unsigned int flag);
    int         mmu_munmap(const void* mem, size_t size, unsigned int flag);
// -------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_ARCH_I686_MMU_H_
