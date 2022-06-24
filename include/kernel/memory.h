#ifndef MEMORY_H
#define MEMORY_H

#include <common/common.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/liballoc/liballoc.h>

#define KERNEL_OFFSET 0xFFFFFF8000000000

#define P4INDEX(vaddr) ( (uint64_t)vaddr >> 39 & 511 )
#define P3INDEX(vaddr) ( (uint64_t)vaddr >> 30 & 511 )
#define P2INDEX(vaddr) ( (uint64_t)vaddr >> 21 & 511 )
#define P1INDEX(vaddr) ( (uint64_t)vaddr & 0x1FFFFF )

#define ALIGN(num, bound) ((num + bound-1) & ~(bound-1))
#define ALIGN_DOWN(num, bound) (num - num % bound)

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

#endif
