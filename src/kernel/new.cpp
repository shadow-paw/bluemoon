#include <stddef.h>

/// Implement c++ new with kernel malloc
void* operator new(size_t s) throw() {
    return nullptr;
}
void* operator new(size_t, void* p) throw() {
    return p;
}
void* operator new[](size_t, void* p) throw() {
    return p;
}
void operator delete(void* p) throw() {
}
void operator delete(void*, size_t s) throw() {
}
void operator delete[] (void*) throw() {
}
void operator delete[] (void*, size_t s) throw() {
}
