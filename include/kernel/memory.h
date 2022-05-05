#ifndef MEMORY_H
#define MEMORY_H

#include <common.h>
#include <kernel/memory/pmm.h>

#define KERNEL_OFFSET 0xFFFFFF8000000000

#define P4INDEX(vaddr) vaddr >> 39 & 511
#define P3INDEX(vaddr) vaddr >> 30 & 511
#define P2INDEX(vaddr) vaddr >> 21 & 511
#define P1INDEX(vaddr) vaddr & 0x1FFFFF

#define ALIGN(num, bound) ((num + bound-1) & ~(bound-1))

namespace MemAlloc {
    namespace Kernel {

    }

    void* DumbAlloc(uint64_t size, uint64_t alignment);
}

#endif
