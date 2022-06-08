#include <kernel/memory/pmm.h>
#include <interface.h>

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void memset(void* mem, uint8_t val, size_t s){
    for(uint64_t i = 0; i < s; i++){
        *(uint8_t*)mem = val;
    }
}


namespace PMM{
    Bitmap bitmap;

    namespace{
        uint64_t usedMemory = 0;
        uint64_t freeMemory = 0;
        void* freeRegion = nullptr;
        uint64_t* pageTable = nullptr;
    }

    uint64_t GetPhysAddr(uint64_t vaddr){
        return vaddr - KERNEL_OFFSET;

        if(pageTable == nullptr){
            return vaddr - KERNEL_OFFSET;
        } else {
            //TODO: READ PAGE MAPPING AND RETURN CORRECT ADDRESS
            /*
            uint64_t* p3Table = (uint64_t*)GetVirtualAddr(pageTable[P4INDEX((uint64_t)vaddr)] & ~0b11);
            if(p3Table == nullptr) return 0;
            uint64_t* p2Table = (uint64_t*)GetVirtualAddr(p3Table[P3INDEX((uint64_t)vaddr)] & ~0b11);
            if(p2Table == nullptr) return 0;
            uint64_t baseAddr = p2Table[P2INDEX((uint64_t)vaddr)];
            if(baseAddr == 0) return 0;
            return baseAddr + P1INDEX((uint64_t)vaddr);*/
        } 
    }

    uint64_t GetVirtualAddr(uint64_t phaddr){
        return phaddr + KERNEL_OFFSET;

        if(pageTable == nullptr){

        } else {
            return 0;
        }
    }

    void Initialize(multiboot_memory_map_t* area){
        freeRegion = (void*)area->addr + KERNEL_OFFSET;
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
        uint64_t* p3Table = p4Table + 512;
        uint64_t* p2Table = p3Table + 512;

        memset(p4Table, 0, 4096);
        memset(p3Table, 0, 4096);
        memset(p2Table, 0, 4096);
        
        p4Table[P4INDEX(KERNEL_OFFSET)] = ((uint64_t)p3Table - KERNEL_OFFSET) | 0b11;
        p3Table[P3INDEX(KERNEL_OFFSET)] = ((uint64_t)p2Table - KERNEL_OFFSET) | 0b11;

        uint64_t phaddr = 0x0;
        uint64_t p2Offset = P2INDEX(KERNEL_OFFSET);
        for(uint64_t i = 0; i < s; i++){
            p2Table[p2Offset + i] = phaddr | 0b10000011;
            phaddr += PAGE_SIZE;
        }

        if(IDENTITY_MAP_KERNEL){
            p4Table[0] = p4Table[P4INDEX(KERNEL_OFFSET)];
        }

        uint64_t* p4TableLow = (uint64_t*)((uint64_t)p4Table - KERNEL_OFFSET); 
        /*
        UI::Clear();
        uint64_t* check4 = (uint64_t*)(p4TableLow[P4INDEX(KERNEL_OFFSET)] & ~0b11);
        UI::PrintHex(check4[0], 3);
        uint64_t* check3 = (uint64_t*)(check4[P3INDEX(KERNEL_OFFSET)] & ~0b11);
        UI::PrintHex(check3[0], 4);
        uint64_t* check2 = (uint64_t*)(check3[P2INDEX(KERNEL_OFFSET)] & ~0b11);
        UI::PrintHex(check2[305], 5);*/

        asm volatile("mov %0, %%cr3" :: "r"(p4TableLow));
        pageTable = p4Table;
    }

    void MapRegion(uint64_t phaddr, uint64_t vaddr){
        uint64_t* p3Table = (uint64_t*)((pageTable[P4INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
        uint64_t* p2Table = (uint64_t*)((p3Table[P3INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
        p2Table[P2INDEX(vaddr)] = phaddr | 0b10000011;
        asm volatile("mov %0, %%cr3" :: "r"((uint64_t)pageTable - KERNEL_OFFSET));
    }

    void* AllocatePage(){
        for(int i = 0; i < bitmap.Count; i++){
            if(!bitmap[i]){
                uint64_t allocatedAddress = (uint64_t)freeRegion + i * PAGE_SIZE;
                LockPages((void*)allocatedAddress);
                return (void*)allocatedAddress;
            }
        }
    }

    // vaddr is relative to start of the free region
    void LockPages(void* vaddr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = (uint64_t)(vaddr - (uint64_t)freeRegion) / PAGE_SIZE + i;
            if(bitmap[index]) return;
            bitmap.Set(index, true);
            usedMemory += PAGE_SIZE; 
            freeMemory -= PAGE_SIZE;
        }
    }

    void FreePages(void* vaddr, uint64_t count){
        for(int i = 0; i < count; i++){
            uint64_t index = (uint64_t)(vaddr - (uint64_t)freeRegion) / PAGE_SIZE + i;
            if(!bitmap[index]) return;
            bitmap.Set(index, false);
            freeMemory += PAGE_SIZE;
            usedMemory -= PAGE_SIZE;
        }
    }
}