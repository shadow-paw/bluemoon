#include "hal.h"

namespace kernel {

bool HAL::detect() {
    {  // serial
        auto driver = reinterpret_cast<DRIVER_SERIAL*>(find_driver(DriverTypeSerial, 0));
        _serials.push_front(HAL_SERIAL());
        auto& serial = _serials.front();
        driver->open(driver, &serial, 0);
    }
    return true;
}

}  // namespace kernel
