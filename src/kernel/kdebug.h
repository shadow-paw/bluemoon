#ifndef KERNEL_KDEBUG_H_
#define KERNEL_KDEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif
    // -------------------------------------------------
    void kputc(int c);
    void kprintf(const char* fmt, ...);
    // -------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_KDEBUG_H_
