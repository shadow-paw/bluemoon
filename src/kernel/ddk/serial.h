#ifndef KERNEL_DDK_SERIAL_H_
#define KERNEL_DDK_SERIAL_H_

#include <stdint.h>
#include <stddef.h>
#include "driver.h"

struct DRIVER_SERIAL_;
typedef struct {
    struct DRIVER_SERIAL_* driver;
    uint32_t index;
    uint16_t ioport;
} HAL_SERIAL;

typedef struct DRIVER_SERIAL_ {
    DRIVER base;
    int  (*open)(struct DRIVER_SERIAL_* driver, HAL_SERIAL* serial, uint32_t index);
    int  (*close)(struct DRIVER_SERIAL_* driver, HAL_SERIAL* serial);
    void (*putc)(HAL_SERIAL* serial, int c);
    int  (*getc)(HAL_SERIAL* serial);
} DRIVER_SERIAL;

#endif  // KERNEL_DDK_SERIAL_H_
