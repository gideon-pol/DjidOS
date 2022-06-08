#include <interface.h>
#include <kernel/interrupt.h>
#include <kernel/io.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <kernel/multiboot2.h>
#include <kernel/memory.h>

extern uint64_t _KernelStart;
/* IMPORTANT:
    the multiboot information structure technically still comes
    after the kernel elf sections. It can be safely discarded and
    overwritten after booting however.
*/
extern uint64_t _KernelEnd;

struct KernelInfo {
    multiboot_memory_map_t* largest_available_memory_section;
    uint64_t framebuffer;
    uint16_t frame_width;
    uint16_t frame_height;
};

void enableFPU();
void kernel_main(KernelInfo);

extern "C"
void prekernel(uint64_t multiboot_info_addr){
    UI::Old::Clear();

    KernelInfo info;
    
    unsigned size = *(unsigned *) multiboot_info_addr;
    struct multiboot_tag* tag = (struct multiboot_tag*) (multiboot_info_addr + 8); 

    for (tag = (struct multiboot_tag *) (multiboot_info_addr + 8);
       tag->type != 0;
       tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~7)))
    {
        switch (tag->type)
        {
            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_memory_map_t *mmap;
                struct multiboot_tag_mmap *mmapTag = (struct multiboot_tag_mmap*) tag;
                multiboot_memory_map_t* largestArea = nullptr;

                int i = 0;
                for (mmap = mmapTag->entries;
                      (uint8_t*)mmap < (uint8_t*)tag + tag->size;
                      mmap = (multiboot_memory_map_t *)((uint64_t)mmap + mmapTag->entry_size))
                {
                    i += 3;
                    
                    switch(mmap->type){
                        case MULTIBOOT_MEMORY_AVAILABLE: {
                            if(largestArea == nullptr || mmap->len >= largestArea->len) largestArea = mmap;
                        }
                    }
                }

                info.largest_available_memory_section = largestArea;
            }
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
                info.framebuffer = tagfb->common.framebuffer_addr;
                info.frame_width = tagfb->common.framebuffer_width;
                info.frame_height = tagfb->common.framebuffer_height;
                
                switch(tagfb->common.framebuffer_type){
                    case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
                        //*(uint32_t*)(0x1000) = 0;
                        break;
                    }
                    case MULTIBOOT_FRAMEBUFFER_TYPE_RGB: {
                        //*(uint32_t*)(0x1000) = 1;
                        break;
                    }
                    case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT: {
                        //*(uint32_t*)(0x1000) = 2;
                        break;
                    }
                    default: {
                        //*(uint32_t*)(0x1000) = 3;
                        break;
                    }
                }
            }
        }
    }



    PMM::Initialize(info.largest_available_memory_section);
    PMM::SetupPaging(PMM::bitmap.Map + PMM::bitmap.Size, 1);

    kernel_main(info);
}

void kernel_main(KernelInfo info){
    enableFPU();

    InterruptManager::Initialize();
    //keyboardDriver.Initialize();
    //mouseDriver.Initialize();
    InterruptManager::RemapPIC();
    
    UI::Graphics::Setup(info.frame_width, info.frame_height, info.framebuffer);

    //DrawBox(100, 100, 100, 100, Color::Cyan);
    DrawString("wat de neuk");
    //crash();
    
/*
    UI::Old::Clear();

    UI::Old::FillRow(0, '#');
    UI::Old::FillRow(24, '#');

    UI::Old::Print("DjidOS - beste OS", 1);
    UI::Old::Print("this is a test", 2);*/

    while(true);
}

void handleMultibootTags(uint64_t multiboot_info_addr){
    
}

void enableFPU(){
    size_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x200;
    asm volatile("mov %0, %%cr4" :: "r"(cr4));
    const uint16_t controlWord = 0x37f;
    asm volatile("fldcw %0" :: "m"(controlWord));
}
