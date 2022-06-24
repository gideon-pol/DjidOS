#ifndef VMM_H
#define VMM_H

#include <common/common.h>
#include <kernel/memory.h>

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