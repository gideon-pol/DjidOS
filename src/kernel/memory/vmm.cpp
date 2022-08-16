#include <kernel/memory/vmm.h>
#include <interface.h>

namespace VMM{
    namespace{
        void memset(void* mem, uint8_t val, size_t s){
            for(uint64_t i = 0; i < s; i++){
                *(uint8_t*)mem = val;
            }
        }

        void* freeRegion = nullptr;
        uint64_t* pageTable = nullptr;
        Bitmap bitmap;
    }

    void* GetPhysAddr(void* vaddr){
        // If paging has not been set up properly simply retract the kernel's offset, otherwise translate through the page table
        if(pageTable == nullptr){
            return (void*)((uintptr_t)vaddr - KERNEL_OFFSET);
        } else {
            if(pageTable[P4INDEX(vaddr)] & PAGE_PRESENT){
                uint64_t* p3Table = (uint64_t*)((pageTable[P4INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
                if(p3Table[P3INDEX(vaddr)] & PAGE_PRESENT){
                    uint64_t* p2Table = (uint64_t*)((p3Table[P3INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
                    if(p2Table[P2INDEX(vaddr)] & PAGE_PRESENT){
                        return (void*)(P1INDEX(vaddr) + (p2Table[P2INDEX(vaddr)] & ~0xFFF));
                    }
                }
            }
        } 

        return (void*)-1;
    }

    // Sets up a new page table at vaddress addr and maps the first s pages at KERNEL_OFFSET for the kernel
    void SetupPaging(void* addr, size_t s){
        uint64_t* p4Table = (uint64_t*)ALIGN((uint64_t)addr, PAGE_TABLE_SIZE);
        uint64_t* p3Table = p4Table + PAGE_TABLE_SIZE/sizeof(uint64_t*);
        uint64_t* p2Table = p3Table + PAGE_TABLE_SIZE/sizeof(uint64_t*);

        memset(p4Table, 0, 4096);
        memset(p3Table, 0, 4096);
        memset(p2Table, 0, 4096);
        
        p4Table[P4INDEX(KERNEL_OFFSET)] = ((uint64_t)p3Table - KERNEL_OFFSET) | PAGE_RW | PAGE_PRESENT;
        p3Table[P3INDEX(KERNEL_OFFSET)] = ((uint64_t)p2Table - KERNEL_OFFSET) | PAGE_RW | PAGE_PRESENT;

        uint64_t phaddr = 0x0;
        uint64_t p2Offset = P2INDEX(KERNEL_OFFSET);
        for(uint64_t i = 0; i < s; i++){
            p2Table[p2Offset + i] = phaddr | 0b10000000 | PAGE_RW | PAGE_PRESENT;
            phaddr += PAGE_SIZE;
        }

        if(IDENTITY_MAP_KERNEL){
            p4Table[0] = p4Table[P4INDEX(KERNEL_OFFSET)];
        }

        uint64_t* p4TableLow = (uint64_t*)((uint64_t)p4Table - KERNEL_OFFSET);

        asm volatile("mov %0, %%cr3" :: "r"(p4TableLow));
        pageTable = p4Table;

        uintptr_t leftoverMemInPage = (uintptr_t)p2Table + PAGE_TABLE_SIZE;
        size_t leftoverMem = s * PAGE_SIZE - (leftoverMemInPage - KERNEL_OFFSET);

        uint64_t bitmapCount = leftoverMem / PAGE_TABLE_SIZE;
        if(leftoverMemInPage % PAGE_TABLE_SIZE != 0) bitmapCount++;
        uint64_t bitmapSize = bitmapCount / 8 + 1;

        bitmap = Bitmap((uint8_t*)leftoverMemInPage, bitmapCount, bitmapSize);

        memset((void*)leftoverMemInPage, 0, bitmapSize); 
        for(int i = 0; i < bitmapSize/PAGE_TABLE_SIZE + 1; i++){
            bitmap.Set(i, true);
        }

        KernelHeapEnd = (void*)KERNEL_HEAP_START;
    }

    void* AllocatePageTable(){
        for(int i = 0; i < bitmap.Count; i++){  
            if(!bitmap[i]){   
                uintptr_t allocatedAddress = (uintptr_t)bitmap.Map + i * PAGE_TABLE_SIZE;
                bitmap.Set(i, true);

                memset((void*)allocatedAddress, 0, PAGE_TABLE_SIZE);
                return (void*)allocatedAddress;
            }
        }

        return nullptr;
    }

    void* FreePageTable(void* vaddr){
        uint64_t index = ((uint64_t)vaddr - (uint64_t)bitmap.Map) / PAGE_TABLE_SIZE;
        bitmap.Set(index, false);
    }

    void MapPage(void* phaddr, void* vaddr){
        if(pageTable[P4INDEX(vaddr)] & PAGE_PRESENT){
            uint64_t* p3Table = (uint64_t*)((pageTable[P4INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
            if(p3Table[P3INDEX(vaddr)] & PAGE_PRESENT){
                uint64_t* p2Table = (uint64_t*)((p3Table[P3INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
                p2Table[P2INDEX(vaddr)] = (uint64_t)phaddr | 0b10000000 | PAGE_RW | PAGE_PRESENT;
            } else {
                
                uint64_t* newP2Table = (uint64_t*)AllocatePageTable();
                p3Table[P3INDEX(vaddr)] = (uint64_t)GetPhysAddr(newP2Table) | PAGE_RW | PAGE_PRESENT;
                newP2Table[P2INDEX(vaddr)] = (uint64_t)phaddr | 0b10000000 | PAGE_RW | PAGE_PRESENT;
            }
        } else{
            uint64_t* newP3Table = (uint64_t*)AllocatePageTable();
            pageTable[P4INDEX(vaddr)] = (uint64_t)GetPhysAddr(newP3Table) | PAGE_RW | PAGE_PRESENT;
            uint64_t* newP2Table = (uint64_t*)AllocatePageTable();
            newP3Table[P3INDEX(vaddr)] = (uint64_t)GetPhysAddr(newP2Table) | PAGE_RW | PAGE_PRESENT;
            newP2Table[P2INDEX(vaddr)] = (uint64_t)phaddr | 0b10000000 | PAGE_RW | PAGE_PRESENT;
        }

        asm volatile("mov %0, %%cr3" :: "r"(GetPhysAddr(pageTable)));
    }

    void UnmapPage(void* vaddr){
        if(pageTable[P4INDEX(vaddr)] & PAGE_PRESENT){
            uint64_t* p3Table = (uint64_t*)((pageTable[P4INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
            if(p3Table[P3INDEX(vaddr)] & PAGE_PRESENT){
                uint64_t* p2Table = (uint64_t*)((p3Table[P3INDEX(vaddr)] & ~0xFFF) + KERNEL_OFFSET);
                p2Table[P2INDEX(vaddr)] &= ~PAGE_PRESENT;
            }
        }

        asm volatile("mov %0, %%cr3" :: "r"(GetPhysAddr(pageTable)));
    }

    void* SBRK(int64_t s){
        void* originalEnd = KernelHeapEnd;

        if(s < 0){
            size_t pagesToMove = -s / PAGE_SIZE;
            //Terminal::Println("Moving down by %ld bytes, %ld pages, total heap page count: %ld, %ld", -s, pagesToMove, ((uintptr_t)KernelHeapEnd - (uintptr_t)KERNEL_HEAP_START)/PAGE_SIZE, s % PAGE_SIZE);

            for(int i = 0; i < pagesToMove && KernelHeapEnd != KERNEL_HEAP_START; i++){
                KernelHeapEnd = (uint8_t*)KernelHeapEnd - PAGE_SIZE;
                uintptr_t phaddr = (uintptr_t)GetPhysAddr(KernelHeapEnd);
                UnmapPage(KernelHeapEnd);
                //Terminal::Println("Unmapped page");

                PMM::FreePages(phaddr);
                //Terminal::Println("Freed page");
            }

            //Terminal::Println("Heap pages left: %ld", ((uintptr_t)KernelHeapEnd - (uintptr_t)KERNEL_HEAP_START)/PAGE_SIZE);
            //Terminal::Println("Kernel start mapping: %lx", VMM::GetPhysAddr((void*)KERNEL_HEAP_START));

            //Terminal::Println("Done moving down program heap");
        } else if(s > 0) {
            size_t pagesToMove = ALIGN(s, PAGE_SIZE) / PAGE_SIZE;

            for(int i = 0; i < pagesToMove; i++){
                //Terminal::Println("Allocating new page");
                void* page = PMM::AllocatePage();
                if(page == (void*)-1){
                    return (void*)-1;
                }
                //Terminal::Println("VMM Allocated page: %lx", page);
                MapPage(page, KernelHeapEnd);

                KernelHeapEnd = (void*)((uintptr_t)KernelHeapEnd + PAGE_SIZE);
            }
        }

        return originalEnd;
    }

    uint64_t GetHeapSize(){
        return (uint64_t)KernelHeapEnd - (uint64_t)KERNEL_HEAP_START;
    }
}