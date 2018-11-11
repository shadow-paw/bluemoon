#ifndef __MKOSIMAGE_H__
#define __MKOSIMAGE_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string>

class ImageMaker {
public:
    /**
     * Create disk image with one FAT partition
     * @param filename output filename
     * @param size File size, in bytes
     * @return true if success
     */
    static bool create(const std::string& filename, size_t size);
private:
    /**
     * Resize file
     * @param fp File handle to disk image
     * @param size File size, in bytes
     * @return true if success
     */
    static bool resize_file(FILE* fp, size_t size);
    /**
     * Initialize MBR with no partition and no boot code.
     * @param fp File handle to disk image
     * @return true if success
     */
    static bool mbr_init(FILE * fp);
    /**
     * Set partition table entry
     * @param fp File handle to disk image
     * @param index Index of partition, 0~3
     * @param start Start sector, e.g. 2048
     * @param count Number of sector for this volume
     * @return true if success
     */
    static bool mbr_partition(FILE* fp, unsigned int index, uint32_t start, uint32_t count);
    /**
     * Initialize FAT volume with no boot code.
     * @param fp File handle to disk image
     * @param start Start sector, e.g. 2048
     * @param count Number of sector for this volume
     * @return true if success
     */
    static bool vbr_init(FILE* fp, uint32_t start, uint32_t count);
};

#endif
