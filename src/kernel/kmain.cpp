#include "inlineasm.h"
#include "kdebug.h"
#include "bootdata.h"
#include "mmu.h"
#include "memory.h"

void test_kmalloc() {
    char * foo = new char[8192];
    for (int i=0; i < 8192; i++) {
        foo[i] = static_cast<char>(0xff & i);
    }
    kprintf("foo -> %p\n", foo);
    kprintf("foo[1234] = %d, check = %d\n", 0xff & foo[1234], 0xff & 1234);
    delete foo;
}

extern "C" void kmain(void) {
    kprintf("Hello kernel!\n");
    mmu_init((const BOOTDATA*)KADDR_BOOTDATA);
    test_kmalloc();
    kprintf("Goodbye kernel!\n");
}
