#ifndef KERNEL_DDK_SERIAL_H_
#define KERNEL_DDK_SERIAL_H_

#include <stdint.h>
#include <stddef.h>
#include "driver.h"

struct HAL_SERIAL_;
// -------------------------------------------------
typedef struct DRIVER_SERIAL_ {
    DRIVER base;
    int  (*open)(struct DRIVER_SERIAL_* driver, struct HAL_SERIAL_* serial, unsigned int index);
    int  (*close)(struct DRIVER_SERIAL_* driver, struct HAL_SERIAL_* serial);
    void (*putc)(struct HAL_SERIAL_* serial, int c);
    int  (*getc)(struct HAL_SERIAL_* serial);
} DRIVER_SERIAL;
typedef struct HAL_SERIAL_ {
    DRIVER_SERIAL* driver;
    unsigned int index;
    uint16_t ioport;
} HAL_SERIAL;

#endif  // KERNEL_DDK_SERIAL_H_
