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
    /// create heap in the address range
    /// @param heap struct to hold the heap info, filled by function upon return.
    /// @param start starting address
    /// @param size max size of heap to grow
    /// @param flag protection flags, see mmap()
    /// @return true if success
    bool  heap_create(HEAP* heap, void* start, size_t size, unsigned int flag);
    /// allocate memory from heap
    /// @param size size to allocate, in bytes
    /// @return pointer to allocated memory, nullptr if fail
    void* heap_alloc(HEAP* heap, size_t size);
    /// kernel malloc
    /// @param size size to allocate, in bytes
    /// @return pointer to allocated memory, nullptr if fail
    void* kmalloc(size_t size);
    /// kernel free
    /// @param pointer to memory
    void  kfree(void* ptr);
// -------------------------------------------------
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_MEMORY_H_
