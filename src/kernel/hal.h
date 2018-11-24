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

    static HAL* inst();
    // NOTE: this should be implemented per platform
    bool detect();

// serial functions
 public:
    void serial_putc(unsigned int index, int c);
    int serial_getc(unsigned int index);
 private:
    list<HAL_SERIAL> _serials;

 private:
    list<DRIVER*> _drivers;
    DRIVER* find_driver(uint32_t type, uint32_t id);

 private:
    static HAL* _inst;
};
}  // namespace kernel
// -------------------------------------------------
#endif  // __cplusplus

#endif  // KERNEL_HAL_H_
