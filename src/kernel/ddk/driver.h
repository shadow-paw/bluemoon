#ifndef KERNEL_DDK_DRIVER_H_
#define KERNEL_DDK_DRIVER_H_

#include <stdint.h>

enum {
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
    int (*init)(struct DRIVER_* driver);
    int (*fini)(struct DRIVER_* driver);
} DRIVER;

#endif  // KERNEL_DDK_DRIVER_H_
