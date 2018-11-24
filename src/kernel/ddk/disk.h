#ifndef KERNEL_DDK_DISK_H_
#define KERNEL_DDK_DISK_H_

#include <stdint.h>
#include "driver.h"
#include "filesystem.h"

struct HAL_DISK_;
typedef struct DRIVER_DISK_ {
    DRIVER base;
    int      (*open)(struct DRIVER_DISK_* driver, struct HAL_DISK_* disk, const char* resource, int option);
    int      (*close)(struct DRIVER_DISK_* driver, struct HAL_DISK_* disk);
    int      (*ioctl)(struct HAL_DISK_* disk, int req, ...);
    uint64_t (*read)(struct HAL_DISK_* disk, void* buf, uint64_t block, uint64_t nblock);
    uint64_t (*write)(struct HAL_DISK_* disk, void* buf, uint64_t block, uint64_t nblock);
} DRIVER_DISK;
typedef struct HAL_DISK_ {
    DRIVER_DISK*      disk_driver;
    unsigned int      flag;
    const char*       resource;
    int               option;
    void*             opaque;   // private data used by disk driver, per each disk
    uint64_t          block_count, block_size;
    unsigned int      partition_count;
    struct {
        uint32_t type;
        uint64_t start;
        uint64_t block_count;
    } partition[16];
} HAL_DISK;
typedef struct HAL_VOLUME_ {
    DRIVER_FILESYSTEM* driver;
    HAL_DISK*          disk;
    void*              opaque;       // private data used by fs driver, per each volume
    uint64_t           start;
    uint64_t           block_count, block_size;
} HAL_VOLUME;

#endif  // KERNEL_DDK_DISK_H_
