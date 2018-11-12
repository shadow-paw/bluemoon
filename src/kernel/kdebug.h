#ifndef __BLUEMOON_KDEBUG_H__
#define __BLUEMOON_KDEBUG_H__

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

#endif // __BLUEMOON_KDEBUG_H__
