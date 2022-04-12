#pragma once

#include <cstddef>
#include <cstdint>

uint32_t strlen(char* str);
extern "C" void __cxa_pure_virtual();

inline void* operator new(size_t s) throw() { return nullptr; }
inline void operator delete(void* p, unsigned long s) throw() {}
