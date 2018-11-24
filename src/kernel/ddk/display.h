#ifndef KERNEL_DDK_DISPLAY_H_
#define KERNEL_DDK_DISPLAY_H_


#include <stdint.h>
#include <stddef.h>
#include "driver.h"
#include "bus.h"

struct HAL_DISPLAY_;
typedef struct {
    unsigned int    mode;
    int             width, height, bpp;
    int             pitch;
} HAL_DISPLAY_MODEINFO;
typedef struct DRIVER_DISPLAY_ {
    DRIVER base;
    int (*create)(struct DRIVER_DISPLAY_* driver, struct HAL_DISPLAY_* display);
    int (*destroy)(struct DRIVER_DISPLAY_* driver, struct HAL_DISPLAY_* display);
    int (*get_modeinfo)(struct HAL_DISPLAY_* display, unsigned int mode, HAL_DISPLAY_MODEINFO* info);
    int (*set_mode)(struct HAL_DISPLAY_* display, unsigned int mode);
    int (*clear_screen)(struct HAL_DISPLAY_* display);
} DRIVER_DISPLAY;
typedef struct HAL_DISPLAY_ {
    DRIVER_DISPLAY*      driver;
    HAL_BUS_ADDR         bus_addr;
    HAL_BUS_INFO         bus_info;
    HAL_DISPLAY_MODEINFO mode_info;
    void*                frame_buffer;
    uintptr_t            frame_buffer_addr;
    size_t               frame_buffer_size;
} HAL_DISPLAY;

#endif  // KERNEL_DDK_DISPLAY_H_
