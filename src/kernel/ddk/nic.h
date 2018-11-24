#ifndef KERNEL_DDK_NIC_H_
#define KERNEL_DDK_NIC_H_

#include <stdint.h>
#include <stddef.h>
#include "driver.h"
#include "bus.h"

struct HAL_NIC_;
// -------------------------------------------------
typedef struct DRIVER_NIC_ {
    DRIVER base;
    int (*open)(struct DRIVER_NIC_* driver, struct HAL_NIC_* nic);
    int (*close)(struct DRIVER_NIC_* driver, struct HAL_NIC_* nic);
    int (*control)(struct HAL_NIC_* nic, int code, void* data, size_t datalen);
} DRIVER_NIC;
typedef struct HAL_NIC_ {
    DRIVER_NIC*  driver;
    HAL_BUS_ADDR bus_addr;
    HAL_BUS_INFO bus_info;
    unsigned int io_addr;
    unsigned int irq;
} HAL_NIC;

#endif  // KERNEL_DDK_NIC_H_
