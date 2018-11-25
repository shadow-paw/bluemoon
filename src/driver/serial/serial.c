#include "ddk/ddk.h"
#include "serial.h"

int driver_serial_open(DRIVER_SERIAL* driver, HAL_SERIAL* serial, uint32_t index) {
    uint16_t ioport[] = { 0x03F8, 0x02F8, 0x03E8, 0x02E8 };
    if (index >= sizeof(ioport) / sizeof(ioport[0])) return -1;
    if (!driver || !serial) return -1;
    serial->driver = driver;
    serial->index = index;
    serial->ioport = ioport[index];
    outb((uint16_t)(serial->ioport + 1), 0x00);    // Disable all interrupts
    outb((uint16_t)(serial->ioport + 3), 0x80);    // Enable DLAB (set baud rate divisor)
    outb((uint16_t)(serial->ioport + 0), 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb((uint16_t)(serial->ioport + 1), 0x00);    //                  (hi byte)
    outb((uint16_t)(serial->ioport + 3), 0x03);    // 8 bits, no parity, one stop bit
    outb((uint16_t)(serial->ioport + 2), 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb((uint16_t)(serial->ioport + 4), 0x0B);    // IRQs enabled, RTS/DSR set
    return 0;
}
int driver_serial_close(DRIVER_SERIAL* driver, HAL_SERIAL* serial) {
    if (!driver || !serial) return -1;
    serial->driver = 0;
    return 0;
}
void driver_serial_putc(HAL_SERIAL* serial, int c) {
    if (!serial) return;
    // wait for transmit empty
    while ((inb((uint16_t)(serial->ioport + 5)) & 0x20) == 0) {}
    outb(serial->ioport, (uint8_t)c);
}
int driver_serial_getc(HAL_SERIAL* serial) {
    if (!serial) return -1;
    // wait for input
    while ((inb((uint16_t)(serial->ioport + 5)) & 1) == 0) {}
    return (int)inb(serial->ioport);
}

DRIVER_SERIAL g_driver_serial = {
    .base = {
        .type = DriverTypeSerial,
        .id = 0,
        .ref = 0
    },
    .open = driver_serial_open,
    .close = driver_serial_close,
    .putc = driver_serial_putc,
    .getc = driver_serial_getc
};

DRIVER* driver_serial() {
    return &g_driver_serial.base;
}
