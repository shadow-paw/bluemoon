/**
 * Debug facility for early development stage
 * Should remove this after we have proper serial driver and virtual terminal
 */

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "kaddr.h"
#include "ioport.h"
#include "kdebug.h"

/**
 * Serial output
 */
#define PORT 0x3f8   /* COM1 */
void serial_init() {
    static bool serial_initialized = false;
    if (serial_initialized) return;
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    serial_initialized = true;
}
int serial_is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}
void serial_putc(int c) {
    serial_init();
    while (serial_is_transmit_empty() == 0) {}
    outb(PORT, (uint8_t)c);
}

/**
 * VGA text output
 */
int cur_x = 0, cur_y = 0;
void vga_putc(int c) {
    volatile unsigned char* video = (volatile unsigned char*) (KADDR_ZERO_VMA + 0xB8000);
    // scroll screen
    if (cur_y >= 25) {
        for (int i=0; i < 24*80*2; i++) {
            video[i] = video[i+160];
        }
        for (int i=0; i < 160; i++) {
            video[24*80*2 + i] = 0;
        }
        cur_y = 24;
    }
    if (c == '\r' || c == '\n') {
        cur_x = 80;
    } else {
        video[cur_y*160+cur_x*2  ] = (unsigned char) c;
        video[cur_y*160+cur_x*2+1] = 0x07;
    }
    if ((++cur_x) >= 80) {
        cur_x = 0;
        cur_y++;
    }
}
// -------------------------------------------------
// High level print functions
// -------------------------------------------------
void kputc(int c) {
    serial_putc(c);
    vga_putc(c);
}
void kputs(const char * s) {
    for (; *s; s++) kputc(*s);
}
void hex2asc(uint64_t num, char* des) {
    bool leadingZero = true;
    for (int i=0; i < 16; i++) {
        size_t digit = (size_t)(num >> 60);
        num = (num << 4);
        if (i == 8) {
            if (leadingZero) {
                des -= 8;
            } else {
                *des++ = ':';
            }
        }
        if (digit > 0) leadingZero = false;
        *des++ = (char) ((digit < 10) ? (digit + '0') : (digit - 10 + 'A'));
    } *des = 0;
}
void dec2asc(uint64_t num, char* des) {
    char buf[32];
    uint64_t digit;
    int count = 0;
    for (;;) {
        digit = (num % 10);
        num = (num / 10);
        buf[count] = (char)(digit + '0');
        count++;
        if (num == 0) break;
    }
    for (count--; count >= 0; count--) {
        *des = buf[count];
        des++;
    }
    *des = 0;
}
void kprintf(const char* fmt, ...) {
    char buf[32];
    const char* p;
    char c;
    uint32_t u32;
    uint64_t u64;
    char *s;
    va_list va;
    int bLong;

    va_start(va, fmt);
    for (p=fmt; *p; p++) {
        c = *p;
        if (c != '%') {
            kputc(c);
        } else {
            ++p;
            c = *p;
            if ( c == 'l' ) {
                bLong = 1;
                c = *(++p);
            } else {
                bLong = 0;
            }
            switch (c) {
                case '$':
                    u32 = va_arg(va, uint32_t);
                    buf[0] = (char)((u32>>24) & 0xFF);
                    buf[1] = (char)((u32>>16) & 0xFF);
                    buf[2] = (char)((u32>>8 ) & 0xFF);
                    buf[3] = (char)((u32    ) & 0xFF);
                    buf[4] = 0;
                    kputs(buf);
                    break;
                case 'd':
                    u64 = (uint64_t)va_arg(va, size_t);
                    dec2asc(u64, buf);
                    kputs(buf);
                    break;
                case 'p':
                case 'x':
                case 'X':
                    u64 = (uint64_t)va_arg(va, size_t);
                    hex2asc(u64, buf);
                    kputs(buf);
                    break;
                case 's':
                    s = va_arg(va, char*);
                    kputs(s);
                    break;
                case '%':
                    kputc(c);
                    break;
                default:
                    kputc('%');
                    kputc(c);
                    break;
            }
        }
    }
    va_end(va);
}
