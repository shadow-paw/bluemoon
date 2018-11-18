#include "inlineasm.h"
#include "kdebug.h"

void INT_00(void) {
    kprintf("INT00 : #DE Divide Error Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_01(void) {
    kprintf("INT01 : #DB Debug Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_02(void) {
    kprintf("INT02 : NMI Interrupt. IP\n");
    __asm volatile ("cli; hlt");
}
void INT_03(void) {
    kprintf("INT03 : #BP Breakpoint Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_04(void) {
    kprintf("INT04 : #OF Overflow Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_05(void) {
    kprintf("INT05 : #BR BOUND Range Exceeded Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_06(void) {
    kprintf("INT06 : #UD Invalid Opcode Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_07(void) {
    kprintf("INT07 : #NM Device Not Available Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_08(void) {
    kprintf("INT08 : #DF Double Fault Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_0A(uint32_t code) {
    kprintf("INT0A : #TS Invalid TSS Exception. CODE:%d\n", code);
    __asm volatile ("cli; hlt");
}
void INT_0B(uint32_t code) {
    kprintf("INT0B : #NP Segment Not Present. CODE:%d\n", code);
    __asm volatile ("cli; hlt");
}
void INT_0C(uint32_t code) {
    kprintf("INT0C : #SS Stack Fault Exception. CODE:%d\n", code);
    __asm volatile ("cli; hlt");
}
void INT_0D(uint32_t code, uint32_t ip) {
    kprintf("INT0D : #GP General Protection Exception. IP: %X CODE:%d\n", ip, code);
    __asm volatile ("cli; hlt");
}
// NOTE: INT_0E implemented at mmu.c
void INT_10(void) {
    kprintf("INT10 : #MF x87 FPU Floating-Point Error.\n");
    __asm volatile ("cli; hlt");
}
void INT_11(uint32_t code) {
    kprintf("INT11 : #AC Alignment Check Exception. CODE:%d\n", code);
    __asm volatile ("cli; hlt");
}
void INT_12(void) {
    kprintf("INT12 : #MC Machine Check Exception.\n");
    __asm volatile ("cli; hlt");
}
void INT_13(void) {
    kprintf("INT13 : #XM SIMD Floating-Point Exception.\n");
    __asm volatile ("cli; hlt");
}
