#ifndef KERNEL_DDK_DISK_H_
#define KERNEL_DDK_DISK_H_

#include <stdint.h>
#include "driver.h"

struct DRIVER_DISK_;
struct DRIVER_FILESYSTEM_;
typedef struct {
    struct DRIVER_DISK_* driver;
    const char*  resource;
    uint32_t     flag;
    uint32_t     option;
    void*        opaque;   // private data used by disk driver, per each disk
    uint64_t     block_count, block_size;
    unsigned int partition_count;
    struct {
        uint32_t type;
        uint64_t start;
        uint64_t block_count;
    } partition[16];
} HAL_DISK;
typedef struct {
    struct DRIVER_FILESYSTEM_* driver;
    HAL_DISK* disk;
    void*     opaque;       // private data used by fs driver, per each volume
    uint64_t  start;
    uint64_t  block_count, block_size;
} HAL_VOLUME;

typedef struct DRIVER_DISK_ {
    DRIVER base;
    int      (*open)(struct DRIVER_DISK_* driver, HAL_DISK* disk, const char* resource, int option);
    int      (*close)(struct DRIVER_DISK_* driver, HAL_DISK* disk);
    int      (*ioctl)(HAL_DISK* disk, int req, ...);
    uint64_t (*read)(HAL_DISK* disk, void* buf, uint64_t block, uint64_t nblock);
    uint64_t (*write)(HAL_DISK* disk, void* buf, uint64_t block, uint64_t nblock);
} DRIVER_DISK;

#endif  // KERNEL_DDK_DISK_H_
