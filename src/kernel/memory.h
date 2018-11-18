#ifndef KERNEL_MEMORY_H_
#define KERNEL_MEMORY_H_

#include <stddef.h>
#include <stdbool.h>

#define HEAP_ADDRESSONLY    0x00010000

// -------------------------------------------------
typedef struct {
    size_t       start;
    size_t       ptr;
    size_t       size;
    unsigned int flag;
} HEAP;

#ifdef __cplusplus
extern "C" {
#endif
// -------------------------------------------------
    bool  heap_create(HEAP* heap, void* start, size_t size, unsigned int flag);
    void* heap_alloc(HEAP* heap, size_t size);
    void* kmalloc(size_t size);
    void  kfree(void* ptr);
// -------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_MEMORY_H_
