#ifndef PMM_H
#define PMM_H

#include <common/common.h>
#include <common/bitmap.h>
#include <kernel/memory.h>
#include <kernel/multiboot2.h>
#include <interface/graphics/interface_graphics.h>

#define PAGE_SIZE 0x200000
#define PAGE_TABLE_SIZE 4096

namespace PMM{
    extern Bitmap bitmap;
    extern void* freeRegion;

    void Initialize(multiboot_memory_map_t* area);

    void LockPages(uintptr_t addr, uint64_t count = 1);
    void FreePages(uintptr_t addr, uint64_t count = 1);

    void* AllocatePage();
}

#endif