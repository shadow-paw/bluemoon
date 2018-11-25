#include "inlineasm.h"
#include "kdebug.h"
#include "bootdata.h"
#include "mmu.h"
#include "memory.h"
#include "pic.h"
#include "hal.h"

void test_kmalloc() {
    char * foo = new char[8192];
    for (int i=0; i < 8192; i++) {
        foo[i] = static_cast<char>(0xff & i);
    }
    kdebug("foo -> %p\n", foo);
    kdebug("foo[1234] = %d, check = %d\n", 0xff & foo[1234], 0xff & 1234);
    delete foo;
}
void main2() {
    kernel::HAL::inst()->load_builtin_drivers();
    kernel::HAL::inst()->detect();
    _STI();
    kdebug("Hello kernel!\n");
    test_kmalloc();
    kdebug("Goodbye kernel!\n");
}
extern "C" void kmain(void) {
    mmu_init((const BOOTDATA*)KADDR_BOOTDATA);
    pic_init();
    main2();
}
