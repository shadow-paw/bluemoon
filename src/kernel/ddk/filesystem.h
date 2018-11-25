#ifndef KERNEL_DDK_FILESYSTEM_H_
#define KERNEL_DDK_FILESYSTEM_H_

#include <stdint.h>
#include "driver.h"
#include "disk.h"

typedef struct {
    HAL_VOLUME* vol;
    uint64_t    inode;
} FS_DIR;
typedef struct {
    HAL_VOLUME* vol;
    uint64_t    inode;
    uint32_t    attr;
    uint64_t    size;
    uint64_t    data;
    uint64_t    block_size, nblock;
} FS_FILE;
typedef struct {
    char     name[32];
    uint32_t attr;
    uint64_t size;
} FS_DIRENT;

typedef struct DRIVER_FILESYSTEM_ {
    DRIVER base;
    int      (*open)(struct DRIVER_FILESYSTEM_* driver, HAL_VOLUME* volume);
    int      (*close)(struct DRIVER_FILESYSTEM_* driver, HAL_VOLUME* volume);
    // int    (*format)  (HAL_VOLUME* volume);
    int      (*file_open)(HAL_VOLUME* volume, FS_FILE* file, const char* path);
    int      (*file_close)(HAL_VOLUME* volume, FS_FILE* file);
    uint64_t (*file_read)(HAL_VOLUME* volume, FS_FILE* file, void* buf, uint64_t block, uint64_t nblock);
    uint64_t (*file_write)(HAL_VOLUME* volume, FS_FILE* file, const void* buf, uint64_t block, uint64_t nblock);
    int      (*dir_open)(HAL_VOLUME* volume, FS_DIR *dir, const char* path);
    int      (*dir_close)(HAL_VOLUME* volume, FS_DIR *dir);
    int      (*dir_read)(HAL_VOLUME* volume, FS_DIR *dir, unsigned int index, FS_DIRENT* dirent );
//    int (*dir_write) (HAL_VOLUME* volume, FS_DIR *dir, unsigned int index, FS_DIRENT* dirent );
    // int (*fstat) (HAL_VOLUME* volume, VFS_FILE* file, char* buf);
} DRIVER_FILESYSTEM;

#endif  // KERNEL_DDK_FILESYSTEM_H_
