#include <kernel/memory/pmm.h>
#include <interface.h>

extern uintptr_t _KernelStart;
extern uintptr_t _KernelEnd;

void memset(void* mem, uint8_t val, size_t s){
    for(uint64_t i = 0; i < s; i++){
        *(uint8_t*)mem = val;
    }
}

namespace PMM{
    Bitmap bitmap;
    void* freeRegion = nullptr;

    namespace{
        uint64_t usedMemory = 0;
        uint64_t freeMemory = 0;
    }

    void Initialize(multiboot_memory_map_t* area){
        freeRegion = (void*)area->addr;// + KERNEL_OFFSET;
        freeMemory = area->len;

        uint64_t bitmapCount = area->len / PAGE_SIZE;
        if(area->len % PAGE_SIZE != 0) bitmapCount++;
        uint64_t bitmapSize = bitmapCount / 8 + 1;
        
        uint8_t* bitmapBuffer = (uint8_t*)&_KernelEnd;
        bitmap = Bitmap(bitmapBuffer, bitmapCount, bitmapSize);
        for(int i = 0; i < bitmapSize; i++){
            *(bitmapBuffer + i) = 0;
        }

        // Lock both the kernel region and boot sector
        uint64_t pagesToLock = ((uint64_t)&_KernelEnd - KERNEL_OFFSET)/PAGE_SIZE + 1;

        LockPages((uintptr_t)freeRegion, pagesToLock);
    }

    void* AllocatePage(){
        Terminal::Println("Page allocation requested");
        for(int i = 0; i < bitmap.Count; i++){
            if(!bitmap[i]){
                uintptr_t allocatedAddress = i * PAGE_SIZE;
                LockPages(allocatedAddress);
                return (void*)allocatedAddress;
            }
        }

        Terminal::Println("%crPage allocation failed, out of memory%cw");
        while(true);

        return (void*)-1;
    }

    // phaddr is relative to start of the free region
    void LockPages(uintptr_t phaddr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = phaddr / PAGE_SIZE + i;
            if(bitmap[index]) return;
            bitmap.Set(index, true);
            usedMemory += PAGE_SIZE; 
            freeMemory -= PAGE_SIZE;
        }
    }

    void FreePages(uintptr_t phaddr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = phaddr / PAGE_SIZE + i;
            if(!bitmap[index]) return;
            bitmap.Set(index, false);
            freeMemory += PAGE_SIZE;
            usedMemory -= PAGE_SIZE;
        }
    }
}