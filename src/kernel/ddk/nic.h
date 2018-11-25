#ifndef KERNEL_DDK_NIC_H_
#define KERNEL_DDK_NIC_H_

#include <stdint.h>
#include <stddef.h>
#include "driver.h"
#include "bus.h"

struct HAL_NIC_;
struct DRIVER_NIC_;
typedef struct {
    struct DRIVER_NIC_* driver;
    HAL_BUS_ADDR bus_addr;
    HAL_BUS_INFO bus_info;
    unsigned int io_addr;
    unsigned int irq;
} HAL_NIC;

typedef struct DRIVER_NIC_ {
    DRIVER base;
    int (*open)(struct DRIVER_NIC_* driver, HAL_NIC* nic);
    int (*close)(struct DRIVER_NIC_* driver, HAL_NIC* nic);
    int (*control)(HAL_NIC* nic, int code, void* data, size_t datalen);
} DRIVER_NIC;

#endif  // KERNEL_DDK_NIC_H_
