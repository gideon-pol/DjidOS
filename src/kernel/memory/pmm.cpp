#include <kernel/memory/pmm.h>
#include <interface.h>

extern uintptr_t _KernelStart;
extern uintptr_t _KernelEnd;

namespace PMM{
    Bitmap bitmap;
    void* freeRegion = nullptr;

    namespace{
        uint64_t usedMemory = 0;
        uint64_t freeMemory = 0;
    }

    void Initialize(multiboot_memory_map_t* area){
        freeRegion = (void*)area->addr;
        freeMemory = area->len;

        uint64_t bitmapCount = area->len / PAGE_SIZE;
        uint64_t bitmapSize = bitmapCount / 8 + 1;

        uint8_t* bitmapBuffer = (uint8_t*)&_KernelEnd;
        bitmap = Bitmap(bitmapBuffer, bitmapCount, bitmapSize);

        memset(bitmapBuffer, 0, bitmapSize);

        // Lock both the kernel region and boot sector
        size_t pagesToLock = ALIGN((uint64_t)&_KernelEnd - KERNEL_OFFSET, PAGE_SIZE) / PAGE_SIZE;
        LockPages((uintptr_t)freeRegion, pagesToLock);
    }

    Spinlock pageAllocaterLock;
    void* AllocatePage(){
        pageAllocaterLock.Acquire();
        //Terminal::Println("Page allocation requested");
        for(int i = 0; i < bitmap.Count; i++){
            if(!bitmap[i]){
                uintptr_t allocatedAddress = i * PAGE_SIZE;
                LockPages(allocatedAddress);
                pageAllocaterLock.Unlock();

                return (void*)allocatedAddress;
            }
        }

        //Terminal::Println("%crPage allocation failed, out of memory%cw");
        pageAllocaterLock.Unlock();
        return (void*)-1;
    }

    // phaddr is relative to start of the free region
    void LockPages(uintptr_t phaddr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = phaddr / PAGE_SIZE + i;
            if(bitmap[index]) break;
            bitmap.Set(index, true);
            usedMemory += PAGE_SIZE; 
            freeMemory -= PAGE_SIZE;
        }
    }

    void FreePages(uintptr_t phaddr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = phaddr / PAGE_SIZE + i;
            if(!bitmap[index]) break;
            bitmap.Set(index, false);
            freeMemory += PAGE_SIZE;
            usedMemory -= PAGE_SIZE;
        }
    }
}