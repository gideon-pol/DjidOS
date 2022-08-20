#ifndef MEMORY_H
#define MEMORY_H

#define ALIGN(num, bound) ((num + bound-1) & ~(bound-1))
#define ALIGN_DOWN(num, bound) (num / bound * bound)

#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/alloc.h>

// TODO:
// implementation for basic operations like memset, memcpy, memmove etc
#endif