#ifndef VMM_H
#define VMM_H

#include <common/common.h>

#define P4INDEX(vaddr) ( (uint64_t)vaddr >> 39 & 511 )
#define P3INDEX(vaddr) ( (uint64_t)vaddr >> 30 & 511 )
#define P2INDEX(vaddr) ( (uint64_t)vaddr >> 21 & 511 )
#define P1INDEX(vaddr) ( (uint64_t)vaddr & 0x1FFFFF )

#define KERNEL_OFFSET 0xFFFFFF8000000000

#define PAGE_PRESENT ( 0b1 )
#define PAGE_RW ( 0b10 )
#define IDENTITY_MAP_KERNEL false

namespace VMM {
    extern uint64_t check;
    void SetupPaging(void* addr, size_t s);
    void MapPage(void* phaddr, void* vaddr);
    void UnmapPage(void* vaddr);
    void* AllocatePageTable();
    void* FreePageTable(void* vaddr);
    void* GetPhysAddr(void* vaddr);
    void* SBRK(int64_t s);
    uint64_t GetHeapSize();

    static const void* KERNEL_HEAP_START = (void*)0xFFFFFF9000000000;
    static void* KernelHeapEnd;
}

#endif