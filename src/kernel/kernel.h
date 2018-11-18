#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <stdint.h>
#include <stddef.h>
#include "kaddr.h"
#include "inlineasm.h"
#include "kdebug.h"

#ifdef __cplusplus
extern "C" {
#endif
// -------------------------------------------------
    void idt_init();
// -------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_KERNEL_H_
