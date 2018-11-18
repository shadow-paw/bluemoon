#ifndef KERNEL_ARCH_X86_64_KADDR_H_
#define KERNEL_ARCH_X86_64_KADDR_H_

// Physical Address of kernel (1MB + 4096 ELF Header)
// -------------------------------------------------
#define KADDR_KERNEL_PMA        (0x00100000 + 4096)

// Reserved: -0.5GB ~ TOP
// -------------------------------------------------

// MMU Page Allocator (-1GB ~ -0.5GB)
// for 256GB memory there is 67108864 x 4K pages, requires 512 MiB
// -------------------------------------------------
#define KADDR_MMU_PHYPOOL       (0xFFFFFFFFC0000000)

// Kernel Zone (-2GB ~ -1GB)
// -------------------------------------------------
#define KADDR_ZERO_VMA          (0xFFFFFFFF80000000)
#define KADDR_KERNEL_VMA        (KADDR_ZERO_VMA + KADDR_KERNEL_PMA)
#define KADDR_BOOTDATA          (KADDR_ZERO_VMA + 0x0600)
#define KADDR_PMA(x)            (((uintptr_t)(x)) - KADDR_ZERO_VMA)

// Global Resource (-3GB ~ -2GB)
// Frame buffer, DMA Buffers, MMIO
// -------------------------------------------------
#define KADDR_GLOBAL_RESOURCE   (0xFFFFFFFF40000000)

// Kernel Modules (-4GB ~ -3GB)
// -------------------------------------------------
#define KADDR_DRIVER            (0xFFFFFFFF00000000)

// MMU recursive mapping (-512GB ~ -256GB)
// -------------------------------------------------
#define MMU_RECURSIVE_SLOT      (510UL)

// User-space address layout (0MB ~ 256GB)
// -------------------------------------------------
#define APPADDR_PROCESS_STACK   (0x7FC00000)    // 2GB-4MB
#define APPADDR_PROCESS_HEADER  (APPADDR_PROCESS_STACK  + 4096)
// For spawning process
#define KADDR_CLONEPD           (APPADDR_PROCESS_HEADER + 4096)
#define KADDR_NEWPROCESSINFO    (KADDR_CLONEPD          + 4096)

// Misc
// -------------------------------------------------
#define KADDR_INITRD            (0xFFFFFFFF80C00000)
#define KADDR_DISPLAY           (0xFFFFFFFF8F000000)

#endif  // KERNEL_ARCH_X86_64_KADDR_H_
