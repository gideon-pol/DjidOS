#include <kernel/memory/pmm.h>
#include <interface.h>

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

namespace PMM{
    Bitmap bitmap;

    namespace{
        uint64_t usedMemory = 0;
        uint64_t freeMemory = 0;
        void* freeRegion = nullptr;
        void* pageTable = nullptr;
    }

    void Initialize(multiboot_memory_map_t* area){
        freeRegion = (void*)area->addr;
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
        uint64_t sizeToLock = (uint64_t)&_KernelEnd - KERNEL_OFFSET;
        uint64_t pagesToLock = sizeToLock / PAGE_SIZE;
        if(sizeToLock % PAGE_SIZE != 0) pagesToLock++;

        LockPages(0, pagesToLock);
    }

    // Sets up a new page table at vaddress addr and maps the first s pages at KERNEL_OFFSET for the kernel
    void SetupPaging(void* addr, size_t s){
        uint64_t* p4Table = (uint64_t*)ALIGN((uint64_t)addr, 4096);
        uint64_t* p3Table = p4Table + 4096;
        uint64_t* p2Table = p3Table + 4096;
        uint64_t* p1Table = p2Table + 4096;
        
        p4Table[P4INDEX(KERNEL_OFFSET)] = (uint64_t)(p3Table - KERNEL_OFFSET) | 0b11;
        p3Table[P3INDEX(KERNEL_OFFSET)] = (uint64_t)(p2Table - KERNEL_OFFSET) | 0b11;
        p2Table[P2INDEX(KERNEL_OFFSET)] = (uint64_t)(p1Table - KERNEL_OFFSET) | 0b11;
        uint64_t phaddr = 0x0;
        for(uint64_t i = 0; i < s; i++){
            p1Table[P1INDEX(KERNEL_OFFSET) + i] = phaddr | 0b10000011;
            phaddr += PAGE_SIZE;
        }

        asm volatile("mov %%cr3, %0" :: "r"(p4Table - KERNEL_OFFSET));
        pageTable = p4Table;
    }

    void* AllocatePage(){
        for(int i = 0; i < bitmap.Count; i++){
            if(!bitmap[i]){
                uint64_t allocatedAddress = (uint64_t)freeRegion + i * PAGE_SIZE;
                Interface::Print(allocatedAddress, 24);

                LockPages((void*)allocatedAddress);
                return (void*)allocatedAddress;
            }
        }
    }

    // Addr is relative to start of the free region
    void LockPages(void* addr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = (uint64_t)addr / PAGE_SIZE + i;
            if(bitmap[index]) return;
            bitmap.Set(index, true);
            usedMemory += PAGE_SIZE; 
            freeMemory -= PAGE_SIZE;
        }
    }

    void FreePages(void* addr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = (uint64_t)addr / PAGE_SIZE + i;
            if(!bitmap[index]) return;
            bitmap.Set(index, false);
            freeMemory += PAGE_SIZE;
            usedMemory -= PAGE_SIZE;
        }
    }
}