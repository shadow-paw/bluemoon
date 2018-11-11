#include <string.h>
#include "image_maker.h"

bool ImageMaker::create(const std::string& filename, size_t size) {
    const uint32_t volume_start = 2048;
    const uint32_t volume_sectors = size / 512 - volume_start;
    FILE * fp;
    if ((fp = fopen(filename.c_str(), "wb")) == nullptr) return false;
    if (!resize_file(fp, size)) return false;
    if (!mbr_init(fp)) return false;
    if (!mbr_partition(fp, 0, volume_start, volume_sectors)) return false;
    if (!vbr_init(fp, volume_start, volume_sectors)) return false;
    fclose(fp);
    return true;
}
bool ImageMaker::resize_file(FILE* fp, size_t size) {
    uint8_t buffer[512];
    size_t count = size / sizeof(buffer);
    memset(buffer, 0, sizeof(buffer));
    fseek(fp, 0, SEEK_SET);
    for (size_t i = 0; i < count; i++) {
        if (fwrite(buffer, sizeof(buffer), 1, fp) != 1) return false;
    }
    return true;
}
bool ImageMaker::mbr_init(FILE * fp) {
    uint8_t buffer[512];
    memset(buffer, 0, sizeof(buffer));
    buffer[510] = 0x55;
    buffer[511] = 0xAA;
    fseek(fp, 0, SEEK_SET);
    return fwrite(buffer, sizeof(buffer), 1, fp) == 1;
}
bool ImageMaker::mbr_partition(FILE* fp, unsigned int index, uint32_t start, uint32_t count) {
    uint32_t entry[4];
    entry[0] = 0x80 | (0xFE<<8) | (0xFF<<16) | (0xFF<<24);   // status, head=FE, sector=FF, cylinder=FF
    entry[1] = 0x06 | (0xFE<<8) | (0xFF<<16) | (0xFF<<24);   // type, head=FE, sector=FF, cylinder=FF
    entry[2] = start;
    entry[3] = count;
    if (index >= 4) return false;
    fseek(fp, 446 + (index * 16), SEEK_SET);
    return fwrite(entry, sizeof(entry), 1, fp) == 1;
}
bool ImageMaker::vbr_init(FILE * fp, uint32_t start, uint32_t count) {
    const uint32_t sectors_per_cluster = 2;
    const uint32_t volume_sectors = count;
    const uint32_t root_sectors = (512 * 32 + 511) / 512;
    const uint32_t data_sectors = (volume_sectors - 1 - root_sectors - 4) / 512 * 512;
    const uint32_t data_clusters = data_sectors / sectors_per_cluster;
    const uint32_t fat_sectors = (data_clusters * sectors_per_cluster + 511) / 512 + 1;
    uint8_t buffer[512];
    uint16_t v16;
    uint32_t v32;
    buffer[0] = 0xEB;  // JMP
    buffer[1] = 0x3C;
    buffer[2] = 0x90;
    memcpy(buffer+3, "mkdosfs ", 8);
    // BIOS PARAMETER BLOCK
    v16 = 512; memcpy(&buffer[0x0B], &v16, sizeof(v16));  // 0x0B:  2: sector size
    buffer[0x0D] = (uint8_t)sectors_per_cluster;          // 0x0D:  1: sectors per cluster
    v16 = 1; memcpy(&buffer[0x0E], &v16, sizeof(v16));    // 0x0E:  2: reserved sectors count
    buffer[0x10] = 2;                                     // 0x10:  1: number of allocation tables
    v16 = 512; memcpy(&buffer[0x11], &v16, sizeof(v16));  // 0x11:  2: number of root entries
    v16 = volume_sectors < 65536 ? static_cast<uint16_t>(volume_sectors) : 0;
    v16 = 1; memcpy(&buffer[0x13], &v16, sizeof(v16));    // 0x13:  2: total number of sectors in volume
    buffer[0x15] = 0xF8;                                  // 0x15:  1: media type, F8 for HDD
    v16 = static_cast<uint16_t>(fat_sectors);
    memcpy(&buffer[0x16], &v16, sizeof(v16));             // 0x16:  2: number of sectors per FAT
    v16 = 32; memcpy(&buffer[0x18], &v16, sizeof(v16));   // 0x18:  2: sectors per track
    v16 = 16; memcpy(&buffer[0x1A], &v16, sizeof(v16));   // 0x1A:  2: number of heads
    memcpy(&buffer[0x1C], &start, sizeof(start));         // 0x1C:  4: start sector from disk
    v32 = volume_sectors < 65536 ? 0 : volume_sectors;
    memcpy(&buffer[0x20], &v32, sizeof(v32));             // 0x20:  4: (large) number of sectors, see 0x13
    // EXTENDED BOOT RECORD
    buffer[0x24] = 0x80;                                  // 0x24:  1: physical disk number
    buffer[0x25] = 0;                                     // 0x25:  1: current head (unused)
    buffer[0x26] = 0x29;                                  // 0x26:  1: signature
    v32 = 0x0BADC0DE;
    memcpy(&buffer[0x27], &v32, sizeof(v32));             // 0x27:  4: volume serial number
    memcpy(&buffer[0x2B], "BLUEMOON   ", 11);             // 0x2B: 11: volume label
    memcpy(&buffer[0x36], "FAT16   ", 8);                 // 0x36:  8: system id
    buffer[510] = 0x55;
    buffer[511] = 0xAA;
    fseek(fp, start*512, SEEK_SET);
    return fwrite(buffer, sizeof(buffer), 1, fp) == 1;
}
