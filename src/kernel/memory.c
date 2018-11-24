#include "inlineasm.h"
#include "mmu.h"
#include "memory.h"

extern char _kernel_end;

// -------------------------------------------------
// heap
// -------------------------------------------------
bool heap_create(HEAP* heap, void* start, size_t size, unsigned int flag) {
    heap->start = (size_t)start;
    heap->ptr = 0;
    heap->size = size;
    heap->flag = flag;
    return true;
}
void* heap_alloc(HEAP* heap, size_t size) {
    size_t from, to;
    void* mem;
    if (heap->ptr + size >= heap->size) return NULL;
    if ((heap->flag & HEAP_ADDRESSONLY) == 0) {
        from = ((heap->start + heap->ptr) >> 12) << 12;
        to = ((heap->start + heap->ptr + size + 4095) >> 12) << 12;
        if (to > from) {
            mmu_mmap((void*)from, 0, (to-from), heap->flag & 0xFFFF);
        }
    }
    mem = (void*) (heap->start + heap->ptr);
    heap->ptr += size;
    return mem;
}
// -------------------------------------------------
// kmalloc
// -------------------------------------------------
typedef struct KMALLOC_FREENODE_S {
    struct KMALLOC_FREENODE_S *next;
    size_t  size;
} KMALLOC_FREENODE;
typedef struct {
    _SPINLOCK           lock;
    KMALLOC_FREENODE    node;
} KMALLOC_FREELIST;
// -------------------------------------------------
_SPINLOCK __kernel_heap_lock;
HEAP __kernel_heap = {
    .start = (size_t)((char*)&_kernel_end) + 4096,
    .ptr = 0,
    .size = 1024*1024*192, // max heap size 192MB
    .flag = 0
};
// -------------------------------------------------
KMALLOC_FREELIST __kmalloc_64    = { .lock = 0, .node = { .next = 0, .size = 0} };
KMALLOC_FREELIST __kmalloc_512   = { .lock = 0, .node = { .next = 0, .size = 0} };
KMALLOC_FREELIST __kmalloc_1024  = { .lock = 0, .node = { .next = 0, .size = 0} };
KMALLOC_FREELIST __kmalloc_large = { .lock = 0, .node = { .next = 0, .size = 0} };
// -------------------------------------------------
void* kmalloc(size_t size) {
    KMALLOC_FREELIST *list;
    KMALLOC_FREENODE *node = 0;
    KMALLOC_FREENODE *parent;
    size = (((size + 15 + 16) >> 4) << 4);
    if (size <= 64) {
        list = &__kmalloc_64;
        size = 64;
    } else if (size <= 512) {
        list = &__kmalloc_512;
        size = 512;
    } else if (size <= 1024) {
        list = &__kmalloc_1024;
        size = 1024;
    } else {
        list = &__kmalloc_large;
    }

    _INT_DISABLE();
    _SPIN_LOCK(&list->lock);
    if (size <= 1024) {
        if ((node=list->node.next) != 0) {
            list->node.next = node->next;
        }
     } else {
        for (parent=&list->node; ; parent=node) {
            if ((node=parent->next) == 0) break;
            if (node->size >= size) {
                parent->next = node->next;
                break;
            }
        }
    }
    _SPIN_UNLOCK(&list->lock);
    _INT_RESTORE();

    if (node == 0) {
        _INT_DISABLE();
        _SPIN_LOCK(&__kernel_heap_lock);
        node = (KMALLOC_FREENODE*) heap_alloc(&__kernel_heap, size);
        _SPIN_UNLOCK(&__kernel_heap_lock);
        _INT_RESTORE();
        if (node == 0) return NULL;
        node->size = size;
    }
    // kprintf ("kmalloc : return %X, %d bytes\n", node, size );
    return (void*) ((char*)node +16);  // sizeof(KMALLOC_FREENODE);
}
void kfree(void* ptr) {
    KMALLOC_FREELIST* list;
    KMALLOC_FREENODE* node;
    if (ptr == NULL) return;
    node = (KMALLOC_FREENODE*) ((size_t)ptr - 16);  // sizeof(KMALLOC_FREENODE));
    if (node->size <= 64) {
        list = &__kmalloc_64;
    } else if (node->size <= 512) {
        list = &__kmalloc_512;
    } else if (node->size <= 1024) {
        list = &__kmalloc_1024;
    } else {
        list = &__kmalloc_large;
    }

    _INT_DISABLE();
    _SPIN_LOCK(&list->lock);
    node->next = list->node.next;
    list->node.next = node;
    _SPIN_UNLOCK(&list->lock);
    _INT_RESTORE();
}
