/**
 * Debug facility for early development stage
 * Should remove this after we have proper serial driver and virtual terminal
 */

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "hal.h"
#include "kdebug.h"

namespace kernel {
void kputc(int c) {
    HAL::inst()->serial_putc(0, c);
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
extern "C" void kdebug(const char* fmt, ...) {
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
}  // namespace kernel
