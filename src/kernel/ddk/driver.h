#ifndef KERNEL_DDK_DRIVER_H_
#define KERNEL_DDK_DRIVER_H_

#include <stdint.h>

enum DriverType {
    DriverTypeNull    = 'null',
    DriverTypeBus     = 'bus',
    DriverTypeSerial  = 'com ',
    DriverTypeDisk    = 'disk',
    DriverTypeFS      = 'fs  ',
    DriverTypeDisplay = 'disp',
    DriverTypeNIC     = 'nic '
};

typedef struct DRIVER_ {
    uint32_t type;
    uint32_t id;
    uint32_t ref;
} DRIVER;

#endif  // KERNEL_DDK_DRIVER_H_
