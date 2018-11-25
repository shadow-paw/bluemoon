#include <string.h>
#include "ddk/ddk.h"
#include "bga.h"

#define VBE_DISPI_IOPORT_INDEX  0x01CE
#define VBE_DISPI_IOPORT_DATA   0x01CF

#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)

const HAL_DISPLAY_MODEINFO g_bga_modes[] = {
    { 0, 640,  480, 32, 640*4 },
    { 1, 800,  600, 32, 800*4 },
    { 2, 1024, 768, 32, 1024*4 }
};

int driver_bga_open(DRIVER_DISPLAY* driver, HAL_DISPLAY* display, const HAL_BUS_ADDR* addr, const HAL_BUS_INFO* info) {
    if (!driver || !display || !addr || !info) return -1;
    display->driver = driver;
    if (info->class_code != 3 || info->subclass_code != 0 ||
        info->device_id != 0x1111 || info->vendor_id != 0x1234) return -1;
    display->bus_addr = *addr;
    display->bus_info = *info;
    memset(&display->mode_info, 0, sizeof(display->mode_info));
    display->frame_buffer = 0;
    display->frame_buffer_addr = 0;
    display->frame_buffer_size = 0;
    // kdebug("BGA : Detected device @ %$ | bus:%d slot:%d func:%d\n",
    //        driver->base.id,
    //        display->bus_addr.bus_id,
    //        display->bus_addr.slot_id,
    //        display->bus_addr.func_id);
    return 0;
}
int driver_bga_close(DRIVER_DISPLAY* driver, HAL_DISPLAY* display) {
    if (!driver || !display) return -1;
    display->driver = 0;
    memset(&display->bus_addr, 0, sizeof(&display->bus_addr));
    memset(&display->bus_info, 0, sizeof(display->bus_info));
    memset(&display->mode_info, 0, sizeof(display->mode_info));
    display->frame_buffer = 0;
    display->frame_buffer_addr = 0;
    display->frame_buffer_size = 0;
    return 0;
}
int driver_bga_get_modes(HAL_DISPLAY* display, int mode, HAL_DISPLAY_MODEINFO* info) {
    const int maxmode = sizeof(g_bga_modes) / sizeof(g_bga_modes[0]) -1;
    if (!display || mode > maxmode) return -1;
    if (!info) return maxmode;
    memcpy(info, &g_bga_modes[mode], sizeof(HAL_DISPLAY_MODEINFO));
    // kdebug("BGA : Get video mode info: mode: %d = %dx%dx%d\n",
    //        mode, info->width, info->height, info->bpp);
    return 0;
}
int driver_bga_set_mode(HAL_DISPLAY* display, int mode) {
    HAL_BUS* bus;
    uint32_t bar_addr, bar_size;
    const int maxmode = sizeof(g_bga_modes) / sizeof(g_bga_modes[0]) -1;
    if (!display || mode > maxmode) return -1;
    if ((bus = display->bus_addr.bus) == 0) return -1;
    memcpy(&display->mode_info, &g_bga_modes[mode], sizeof(HAL_DISPLAY_MODEINFO));
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
    outw(VBE_DISPI_IOPORT_DATA, 0);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)display->mode_info.width);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)display->mode_info.height);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)display->mode_info.bpp);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
    outw(VBE_DISPI_IOPORT_DATA, 0x41);
    if (bus->driver->get_bar(bus, &display->bus_addr, 0, &bar_addr, &bar_size) < 0) return -1;
    if ( display->frame_buffer_size != 0 ) {
        // TODO: expose mmap
        // mmu_munmap(display->frame_buffer, display->frame_buffer_size, MMU_MUNMAP_NORELEASE);
        display->frame_buffer_size = 0;
    }
    bar_addr = bar_addr & 0xFFFFFFF0;
    if (bar_size > (uint32_t)(display->mode_info.pitch * display->mode_info.height * 4)) {
        bar_size = (uint32_t)(display->mode_info.pitch * display->mode_info.height * 4);
    }
    // TODO: expose mmap
    // if (mmu_mmap(display->frame_buffer, bar, size, MMU_MMAP_MAPPHY) < 0) return -1;
    display->frame_buffer_addr = (size_t)bar_addr;
    display->frame_buffer_size = (size_t)bar_size;
    // kdebug("BGA : Change video mode: %d = %dx%dx%d, BAR:%X SIZE:%X\n",
    //        mode,
    //        display->mode_info.width, display->mode_info.height, display->mode_info.bpp,
    //        display->frame_buffer_addr, display->frame_buffer_size);
    return 0;
}
int driver_bga_clear_screen(HAL_DISPLAY* display) {
    if (!display || !display->frame_buffer || display->frame_buffer_size == 0) return -1;
    memset(display->frame_buffer, 0, display->frame_buffer_size);
    return 0;
}

DRIVER_DISPLAY g_driver_bga = {
    .base = {
        .type = DriverTypeDisplay,
        .id = 'bga ',
        .ref = 0
    },
    .open = driver_bga_open,
    .close = driver_bga_close,
    .get_modes = driver_bga_get_modes,
    .set_mode = driver_bga_set_mode,
    .clear_screen = driver_bga_clear_screen
};

DRIVER* driver_bga() {
    return &g_driver_bga.base;
}
