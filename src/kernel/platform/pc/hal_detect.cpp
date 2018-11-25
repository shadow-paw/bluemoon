// builtin drivers
#include "serial/serial.h"
#include "pci/pci.h"
//
#include "kdebug.h"
#include "hal.h"

namespace kernel {
void HAL::load_builtin_drivers() {
    // NOTE: modify kernel/Makefile to add builtin drivers
    _drivers.push_front(driver_serial());   // serial io
    _drivers.push_front(driver_pci());      // pci bus
}
bool HAL::detect() {
    if (auto driver = reinterpret_cast<DRIVER_SERIAL*>(find_driver(DriverTypeSerial, 0)); driver != nullptr) {
        _serials.push_front(HAL_SERIAL());
        auto& serial = _serials.front();
        driver->open(driver, &serial, 0);
    }
    if (auto driver = reinterpret_cast<DRIVER_BUS*>(find_driver(DriverTypeBus, 'pci ')); driver != nullptr) {
        _buses.push_front(HAL_BUS());
        auto& bus = _buses.front();
        driver->open(driver, &bus);
        // scan pci
        HAL_BUS_ADDR addr;
        HAL_BUS_INFO info;
        for (unsigned int bus_id = 0; bus_id < 256; bus_id++) {
            for (unsigned int slot_id = 0; slot_id < 32; slot_id++) {
                for (unsigned int func_id = 0; func_id < 8; func_id++) {
                    addr.bus = &bus;
                    addr.bus_id  = bus_id;
                    addr.slot_id = slot_id;
                    addr.func_id = func_id;
                    if (driver->get_info(&bus, &addr, &info) < 0) break;
                    kdebug("PCI : bus:%d slot:%d func:%d  device:%X vendor:%X class:%d:%d\n",
                           addr.bus_id, addr.slot_id, addr.func_id,
                           info.device_id, info.vendor_id, info.class_code, info.subclass_code);
                }
            }
        }
    }
    return true;
}

}  // namespace kernel
