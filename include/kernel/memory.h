#ifndef MEMORY_H
#define MEMORY_H

#define ALIGN(num, bound) ((num + bound-1) & ~(bound-1))
#define ALIGN_DOWN(num, bound) (num / bound * bound)

#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/alloc.h>

void memset(void* dst, int8_t val, size_t count);
void* memcpy(void* dst, void* src, size_t count);
#endif