#ifndef KERNEL_DDK_BUS_H_
#define KERNEL_DDK_BUS_H_

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"

struct DRIVER_BUS_;
struct HAL_BUS_;
typedef struct {
    struct HAL_BUS_* bus;
    unsigned int     bus_id;
    unsigned int     slot_id;
    unsigned int     func_id;
} HAL_BUS_ADDR;
typedef struct {
    unsigned int device_id;
    unsigned int vendor_id;
    unsigned int class_code;
    unsigned int subclass_code;
    unsigned int revision_id;
} HAL_BUS_INFO;
typedef struct DRIVER_BUS_ {
    DRIVER base;
    int (*open)(struct DRIVER_BUS_* driver, struct HAL_BUS_* bus);
    int (*close)(struct DRIVER_BUS_* driver, struct HAL_BUS_* bus);
    int (*get_info)(struct HAL_BUS_* bus, const HAL_BUS_ADDR* addr, HAL_BUS_INFO* info);
    int (*get_bar)(struct HAL_BUS_* bus, const HAL_BUS_ADDR* addr, unsigned int index, uint32_t* bar, uint32_t* size);
    int (*get_irq)(struct HAL_BUS_* bus, const HAL_BUS_ADDR* addr, uint32_t* irq);
    int (*command)(struct HAL_BUS_* bus, const HAL_BUS_ADDR* addr, uint16_t cmd);
} DRIVER_BUS;
typedef struct HAL_BUS_ {
    struct DRIVER_BUS_* bus_driver;
} HAL_BUS;

#endif  // KERNEL_DDK_BUS_H_
