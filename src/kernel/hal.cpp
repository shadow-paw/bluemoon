// builtin drivers
#include "serial/serial.h"
//
#include "hal.h"

namespace kernel {

HAL* HAL::_inst = nullptr;
HAL* HAL::inst() {
    if (!_inst) _inst = new HAL();
    return _inst;
}

HAL::HAL() {
    // NOTE: modify kernel/Makefile to add builtin drivers
    {  // builtin: serial
        auto driver = get_driver_serial();
        driver->init(driver);
        _drivers.push_front(driver);
    }
}
HAL::~HAL() {
}
DRIVER* HAL::find_driver(uint32_t type, uint32_t id) {
    for (auto it = _drivers.begin(); it != _drivers.end(); ++it) {
        auto driver = static_cast<DRIVER*>(it);
        if (driver->type == type && driver->id == id) {
            return driver;
        }
    }
    return nullptr;
}
// -------------------------------------------------
// serial io
// -------------------------------------------------
void HAL::serial_putc(unsigned int index, int c) {
    if (index >= _serials.size()) return;
    auto& serial = _serials[index];
    serial.driver->putc(&serial, c);
}
int HAL::serial_getc(unsigned int index) {
    if (index >= _serials.size()) return 0;
    auto& serial = _serials[index];
    return serial.driver->getc(&serial);
}

}  // namespace kernel
