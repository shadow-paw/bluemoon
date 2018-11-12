#ifndef	__BLUEMOON_KERNEL_H__
#define	__BLUEMOON_KERNEL_H__

#include <stdint.h>
#include <stddef.h>
#include "kaddr.h"
#include "inline_asm.h"
#include "kdebug.h"

#ifdef __cplusplus
extern "C" {
#endif
// -------------------------------------------------
    void IDT_init();
// -------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif // __BLUEMOON_KERNEL_H__
