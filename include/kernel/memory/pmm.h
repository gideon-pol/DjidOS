#ifndef PMM_H
#define PMM_H

#include <common/common.h>
#include <common/bitmap.h>
#include <kernel/memory.h>
#include <kernel/multiboot2.h>

#define PAGE_SIZE 0x200000
#define IDENTITY_MAP_KERNEL true

namespace PMM{
    extern Bitmap bitmap;

    void Initialize(multiboot_memory_map_t* area);
    void SetupPaging(void* addr, size_t s);

    void MapRegion(uint64_t phaddr, uint64_t vaddr);

    void LockPages(void* addr, uint64_t count = 1);
    void FreePages(void* addr, uint64_t count = 1);

    void* AllocatePage();
}

#endif