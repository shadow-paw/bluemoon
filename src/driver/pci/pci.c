#include "ddk/ddk.h"
#include "pci.h"

#define PCI_CONFIG_ADDRESS (0x0CF8)
#define PCI_CONFIG_DATA    (0x0CFC)

uint32_t pci_address(const HAL_BUS_ADDR* addr) {
    if (!addr || addr->bus_id >= 256 || addr->slot_id >= 32 || addr->func_id >= 8) return 0;
    return (unsigned int)(0x80000000 | (addr->bus_id << 16) | (addr->slot_id << 11) | (addr->func_id << 8));
}
int driver_pci_open(DRIVER_BUS* driver, HAL_BUS* bus) {
    if (!driver || !bus) return -1;
    bus->driver = driver;
    return 0;
}
int driver_pci_close(DRIVER_BUS* driver, HAL_BUS* bus) {
    if (!driver || !bus) return -1;
    bus->driver = 0;
    return 0;
}
int driver_pci_get_info(HAL_BUS* bus, const HAL_BUS_ADDR* addr, HAL_BUS_INFO* info) {
    uint32_t base, data;
    if (!bus || !addr || !info) return -1;
    if ((base = pci_address(addr)) == 0 ) return -1;
    outl(PCI_CONFIG_ADDRESS, base +0);
    if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return -1;
    info->device_id = (unsigned int)(data >> 16);
    info->vendor_id = (unsigned int)(data & 0xFFFF);
    outl(PCI_CONFIG_ADDRESS, base +8);
    if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return -1;
    info->class_code    = (unsigned int)(data >> 24);
    info->subclass_code = (unsigned int)((data >> 16) & 0xFF);
    info->revision_id   = (unsigned int)(data & 0xFF);
    return 0;
}
int driver_pci_get_bar(HAL_BUS* bus, const HAL_BUS_ADDR* addr, unsigned int index, uint32_t* bar, uint32_t* size) {
    uint32_t base, data;
    if (!bus || !addr || !bar) return -1;
    if ((base = pci_address(addr)) == 0 ) return -1;
    outl(PCI_CONFIG_ADDRESS, base + 16 + index * 4);
    if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return -1;
    *bar = data;
    if (size) {
        outl(PCI_CONFIG_ADDRESS, base + 16 + index * 4);
        outl(PCI_CONFIG_DATA, 0xFFFFFFFF);
        if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return -1;
        outl(PCI_CONFIG_ADDRESS, base +16 + index*4);
        outl(PCI_CONFIG_DATA, *bar);
        *size = (~data) +1;
    }
    return 0;
}
int driver_pci_get_irq(HAL_BUS* bus, const HAL_BUS_ADDR* addr, uint32_t* irq) {
    uint32_t base, data;
    if (!bus || !addr || !irq) return -1;
    if ((base = pci_address(addr)) == 0 ) return -1;
    outl(PCI_CONFIG_ADDRESS, base +0x3C);
    if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return -1;
    *irq = data & 0xFF;
    return 0;
}
int driver_pci_command(HAL_BUS* bus, const HAL_BUS_ADDR* addr, uint16_t cmd) {
    uint32_t base, data;
    if (!bus || !addr) return -1;
    if ((base = pci_address(addr)) == 0 ) return -1;
    outl(PCI_CONFIG_ADDRESS, base +6);
    outl(PCI_CONFIG_DATA, cmd);
    return 0;
}

DRIVER_BUS g_driver_pci = {
    .base = {
        .type = DriverTypeBus,
        .id = 'pci ',
        .ref = 0
    },
    .open = driver_pci_open,
    .close = driver_pci_close,
    .get_info = driver_pci_get_info,
    .get_bar = driver_pci_get_bar,
    .get_irq = driver_pci_get_irq,
    .command = driver_pci_command
};

DRIVER* driver_pci() {
    return &g_driver_pci.base;
}
