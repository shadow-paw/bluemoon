#ifndef KERNEL_DDK_DISPLAY_H_
#define KERNEL_DDK_DISPLAY_H_


#include <stdint.h>
#include <stddef.h>
#include "driver.h"
#include "bus.h"

struct DRIVER_DISPLAY_;
typedef struct {
    unsigned int mode;
    int          width, height, bpp;
    int          pitch;
} HAL_DISPLAY_MODEINFO;
typedef struct {
    struct DRIVER_DISPLAY_* driver;
    HAL_BUS_ADDR         bus_addr;
    HAL_BUS_INFO         bus_info;
    HAL_DISPLAY_MODEINFO mode_info;
    void*                frame_buffer;
    uintptr_t            frame_buffer_addr;
    size_t               frame_buffer_size;
} HAL_DISPLAY;

typedef struct DRIVER_DISPLAY_ {
    DRIVER base;
    int (*open)(struct DRIVER_DISPLAY_* driver, HAL_DISPLAY* display, const HAL_BUS_ADDR* addr, const HAL_BUS_INFO* info);
    int (*close)(struct DRIVER_DISPLAY_* driver, HAL_DISPLAY* display);
    int (*get_modes)(HAL_DISPLAY* display, int mode, HAL_DISPLAY_MODEINFO* info);
    int (*set_mode)(HAL_DISPLAY* display, int mode);
    int (*clear_screen)(HAL_DISPLAY* display);
} DRIVER_DISPLAY;

#endif  // KERNEL_DDK_DISPLAY_H_
