#ifndef KERNEL_DDK_FILESYSTEM_H_
#define KERNEL_DDK_FILESYSTEM_H_

#include <stdint.h>
#include "driver.h"
#include "disk.h"

typedef struct {
    struct HAL_VOLUME_* vol;
    uint64_t inode;
} FS_DIR;
typedef struct {
    struct HAL_VOLUME_* vol;
    uint64_t inode;
    uint32_t attr;
    uint64_t size;
    uint64_t data;
    uint64_t block_size, nblock;
} FS_FILE;
typedef struct {
    char     name[32];
    uint32_t attr;
    uint64_t size;
} FS_DIRENT;
typedef struct DRIVER_FILESYSTEM_ {
    DRIVER base;
    int      (*open)(struct DRIVER_FILESYSTEM_* driver, struct HAL_VOLUME_* volume);
    int      (*close)(struct DRIVER_FILESYSTEM_* driver, struct HAL_VOLUME_* volume);
    // int    (*format)  (struct HAL_VOLUME_* volume);
    int      (*file_open)(struct HAL_VOLUME_* volume, FS_FILE* file, const char* path);
    int      (*file_close)(struct HAL_VOLUME_* volume, FS_FILE* file);
    uint64_t (*file_read)(struct HAL_VOLUME_* volume, FS_FILE* file, void* buf, uint64_t block, uint64_t nblock);
    uint64_t (*file_write)(struct HAL_VOLUME_* volume, FS_FILE* file, const void* buf, uint64_t block, uint64_t nblock);
    int      (*dir_open)(struct HAL_VOLUME_* volume, FS_DIR *dir, const char* path);
    int      (*dir_close)(struct HAL_VOLUME_* volume, FS_DIR *dir);
    int      (*dir_read)(struct HAL_VOLUME_* volume, FS_DIR *dir, unsigned int index, FS_DIRENT* dirent );
//    int (*dir_write) (struct STORAGE_VOLUME_S* volume, FS_DIR *dir, unsigned int index, FS_DIRENT* dirent );
    // int (*fstat) (struct STORAGE_VOLUME_S* volume, VFS_FILE* file, char* buf);
} DRIVER_FILESYSTEM;

#endif  // KERNEL_DDK_FILESYSTEM_H_
