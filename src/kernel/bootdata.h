#ifndef KERNEL_BOOTDATA_H_
#define KERNEL_BOOTDATA_H_

#include <stdint.h>

// -------------------------------------------------
typedef struct {
    uint64_t base;
    uint64_t size;
    uint32_t type;
    uint32_t acpi;
} __attribute__((__packed__)) BIOSMEMORYMAP;
// -------------------------------------------------
typedef struct {
    uint32_t      kernel_addr;
    uint32_t      kernel_size;
    uint32_t      initrd_addr;
    uint32_t      initrd_size;
    uint32_t      biosmm_count;
    BIOSMEMORYMAP biosmm[1];
} __attribute__((__packed__)) BOOTDATA;
// -------------------------------------------------

#endif  // KERNEL_BOOTDATA_H_
