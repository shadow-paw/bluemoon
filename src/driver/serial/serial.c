#include "ddk/ddk.h"
#include "serial.h"

int driver_open(struct DRIVER_SERIAL_* driver, struct HAL_SERIAL_* serial, unsigned int index) {
    uint16_t ioport[] = { 0x03F8, 0x02F8, 0x03E8, 0x02E8 };
    if (index >= sizeof(ioport) / sizeof(ioport[0])) return -1;
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
int driver_close(struct DRIVER_SERIAL_* driver, struct HAL_SERIAL_* serial) {
    return 0;
}
void driver_putc(struct HAL_SERIAL_* serial, int c) {
    // wait for transmit empty
    while ((inb((uint16_t)(serial->ioport + 5)) & 0x20) == 0) {}
    outb(serial->ioport, (uint8_t)c);
}
int driver_getc(struct HAL_SERIAL_* serial) {
    // wait for input
    while ((inb((uint16_t)(serial->ioport + 5)) & 1) == 0) {}
    return (int)inb(serial->ioport);
}
int driver_init(DRIVER* driver) {
    DRIVER_SERIAL* d = (DRIVER_SERIAL*)driver;
    d->open = driver_open;
    d->close = driver_close;
    d->putc = driver_putc;
    d->getc = driver_getc;
    return 0;
}
int driver_fini(DRIVER* driver) {
    return 0;
}

DRIVER_SERIAL serial_driver = {
    .base = {
        .type = DriverTypeSerial,
        .id = 0,
        .init = driver_init,
        .fini = driver_fini
    },
};

DRIVER* get_driver_serial() {
    return &serial_driver.base;
}
