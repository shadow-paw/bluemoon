#ifndef KERNEL_HAL_H_
#define KERNEL_HAL_H_

#include <stdbool.h>
#include <stdint.h>
#include "ddk/ddk.h"
#include "std/list.h"

#ifdef __cplusplus
// -------------------------------------------------
namespace kernel {
class HAL {
 public:
    HAL();
    ~HAL();

    // NOTE: this should be implemented per platform
    void load_builtin_drivers();
    bool detect();

// -------------------------------------------------
// singleton
// -------------------------------------------------
 public:
    static HAL* inst();

 private:
    static HAL* _inst;
// -------------------------------------------------
// serial functions
// -------------------------------------------------
 public:
    /// write character to serial port
    /// @param index port index, 0: COM1, 1: COM2, 2: COM3, 3: COM4
    /// @param c character to output
    void serial_putc(unsigned int index, int c);
    /// read character from serial port
    /// @param index port index, 0: COM1, 1: COM2, 2: COM3, 3: COM4
    /// @return character read from input, 0 if failed.
    int  serial_getc(unsigned int index);

 private:
    // devices
    list<HAL_SERIAL> _serials;
    list<HAL_BUS>    _buses;

// -------------------------------------------------
// loaded drivers
// -------------------------------------------------
 private:
    list<DRIVER*> _drivers;
    DRIVER* find_driver(uint32_t type, uint32_t id);
};
}  // namespace kernel
// -------------------------------------------------
#endif  // __cplusplus

#endif  // KERNEL_HAL_H_
