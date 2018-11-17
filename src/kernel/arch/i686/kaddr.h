#ifndef KERNEL_ARCH_I686_KADDR_H_
#define KERNEL_ARCH_I686_KADDR_H_

// Physical Address of kernel (1MB + 4096 ELF Header)
// ----------------------------------------------
#define KADDR_KERNEL_PMA        (0x00100000 + 4096)

// MMU Page Allocator (-8MB ~ -4MB)
// for 4GiB requires 4 MiB
// ----------------------------------------------
#define KADDR_MMU_STACK         (0xFF800000)

// Kernel Zone (-128MB ~ -TOP)
// ----------------------------------------------
#define KADDR_ZERO_VMA          (0xE0000000)
#define KADDR_SYNCPD            (0xEFFFF000)
#define KADDR_KERNEL_VMA        (KADDR_ZERO_VMA + KADDR_KERNEL_PMA)
#define KADDR_BOOTDATA          (KADDR_ZERO_VMA + 0x0600)
#define KADDR_PMA(x)            (((uintptr_t)(x)) - KADDR_ZERO_VMA)

// Global Resource (-256MB ~ -128MB)
// Frame buffer, DMA Buffers, MMIO
// ----------------------------------------------
#define KADDR_GLOBAL_RESOURCE   (0xC0000000)

// Kernel Modules (-384MB ~ -256MB)
// ----------------------------------------------
#define KADDR_DRIVER            (0xD0000000)

// MMU recursive mapping (-4MB ~ TOP)
// ----------------------------------------------
#define MMU_RECURSIVE_SLOT      (1023UL)

// User-space address layout (0MB ~ 256MB)
// ----------------------------------------------
#define APPADDR_PROCESS_STACK   (0x7FC00000)
#define APPADDR_PROCESS_HEADER  (APPADDR_PROCESS_STACK  + 4096)
// For spawning process
#define KADDR_CLONEPD           (APPADDR_PROCESS_HEADER + 4096)
#define KADDR_NEWPROCESSINFO    (KADDR_CLONEPD          + 4096)

// Misc
// ----------------------------------------------
#define KADDR_INITRD            (0xC0000000)
#define KADDR_DISPLAY           (0xFE000000)

// Architecture specific type
// ----------------------------------------------
typedef uint32_t    MMU_PADDR;

#endif  // KERNEL_ARCH_I686_KADDR_H_
