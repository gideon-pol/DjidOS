#include <interface.h>
#include <kernel/interrupt.h>
#include <kernel/io.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <kernel/multiboot2.h>
#include <kernel/memory.h>

extern uintptr_t _KernelStart;
/* IMPORTANT:
    the multiboot information structure technically still comes
    after the kernel elf sections. It can be safely discarded and
    overwritten after booting however.
*/
extern uintptr_t _KernelEnd;

struct KernelInfo {
    multiboot_memory_map_t* largest_available_memory_section;
    uint64_t framebuffer;
    uint16_t frame_width;
    uint16_t frame_height;
};

void enableFPU();
void kernel_main(KernelInfo);

extern "C"
void prekernel(uintptr_t multiboot_info_addr){
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
                      mmap = (multiboot_memory_map_t *)((uintptr_t)mmap + mmapTag->entry_size))
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
                
                /*
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
                }*/
            }
        }
    }

    PMM::Initialize(info.largest_available_memory_section);
    VMM::SetupPaging((void*)((uintptr_t)PMM::bitmap.Map + PMM::bitmap.Size), 1);

    kernel_main(info);
}

void kernel_main(KernelInfo info){
    InterruptManager::Initialize();
    
    UI::Graphics::Setup(info.frame_width, info.frame_height, info.framebuffer);    
    Terminal::Setup();

    Terminal::Println("Framebuffer setup completed");
    Terminal::Println("Initialized terminal");

    keyboardDriver.Initialize();
    Terminal::Println("%cgSuccessfully started keyboard driver%cw");

    mouseDriver.Initialize();
    Terminal::Println("%cgSuccessfully started mouse driver%cw");

    InterruptManager::RemapPIC();
    Terminal::Println("Remapped PIC chip");

    Terminal::Println("Enabling FPU");
    enableFPU();

    // Landing screen
    {
        DrawBox(0, 0, CurrentFrame.Width, CurrentFrame.Height - Font->height, Color::White);
        int width = 55;
        int height = 9;
        int xOffset = (CurrentFrame.GetColumns() - width) / 2;
        int yOffset = (CurrentFrame.GetRows() - height * 1.5) / 2;
        
        UI::Graphics::DrawString(" _____                __          _____   _____", -1, xOffset, yOffset+0, Color::Black);
        UI::Graphics::DrawString("/\\  __`\\   __  __    /\\ \\        /\\  __`\\/\\  __`\\", -1, xOffset, yOffset+1, Color::Black);
        UI::Graphics::DrawString("\\ \\ \\/\\ \\ /\\_\\/\\_\\   \\_\\ \\    ___\\ \\ \\/\\ \\ \\ \\L\\_\\", -1, xOffset, yOffset+2, Color::Black);
        UI::Graphics::DrawString(" \\ \\ \\ \\ \\\\/\\ \\/\\ \\  /'_` \\  / __`\\ \\ \\ \\ \\ \\____ \\", -1, xOffset, yOffset+3, Color::Black);
        UI::Graphics::DrawString("  \\ \\ \\_\\ \\\\ \\ \\ \\ \\/\\ \\L\\ \\/\\ \\L\\ \\ \\ \\_\\ \\/\\ \\L\\ \\", -1, xOffset, yOffset+4, Color::Black);
        UI::Graphics::DrawString("   \\ \\____/_\\ \\ \\ \\_\\ \\___,_\\ \\____/\\ \\_____\\ \\_____\\", -1, xOffset, yOffset+5, Color::Black);
        UI::Graphics::DrawString("    \\/___//\\ \\_\\ \\/_/\\/__,_ /\\/___/  \\/_____/\\/_____/", -1, xOffset, yOffset+6, Color::Black);
        UI::Graphics::DrawString("          \\ \\____/", -1, xOffset, yOffset+7, Color::Black);
        UI::Graphics::DrawString("           \\/___/", -1, xOffset, yOffset+8, Color::Black);
        int rows = CurrentFrame.GetRows();
        UI::Graphics::DrawString("DjidOS - A mediocre OS", -1, 0, rows - 2, Color::Black);

        
        //THIS DRAWS A FUNNY RAINBOW LANDING SCREEN
        /*
        int h = 0;
        while(true){
            UI::Graphics::DrawString(" _____                __          _____   _____", -1, xOffset, yOffset+0, Color::FromHSV(h, 1, 1));
            UI::Graphics::DrawString("/\\  __`\\   __  __    /\\ \\        /\\  __`\\/\\  __`\\", -1, xOffset, yOffset+1, Color::FromHSV(h+45, 1, 1));
            UI::Graphics::DrawString("\\ \\ \\/\\ \\ /\\_\\/\\_\\   \\_\\ \\    ___\\ \\ \\/\\ \\ \\ \\L\\_\\", -1, xOffset, yOffset+2, Color::FromHSV(h+90, 1, 1));
            UI::Graphics::DrawString(" \\ \\ \\ \\ \\\\/\\ \\/\\ \\  /'_` \\  / __`\\ \\ \\ \\ \\ \\____ \\", -1, xOffset, yOffset+3, Color::FromHSV(h+135, 1, 1));
            UI::Graphics::DrawString("  \\ \\ \\_\\ \\\\ \\ \\ \\ \\/\\ \\L\\ \\/\\ \\L\\ \\ \\ \\_\\ \\/\\ \\L\\ \\", -1, xOffset, yOffset+4, Color::FromHSV(h+180, 1, 1));
            UI::Graphics::DrawString("   \\ \\____/_\\ \\ \\ \\_\\ \\___,_\\ \\____/\\ \\_____\\ \\_____\\", -1, xOffset, yOffset+5, Color::FromHSV(h+225, 1, 1));
            UI::Graphics::DrawString("    \\/___//\\ \\_\\ \\/_/\\/__,_ /\\/___/  \\/_____/\\/_____/", -1, xOffset, yOffset+6, Color::FromHSV(h+270, 1, 1));
            UI::Graphics::DrawString("          \\ \\____/", -1, xOffset, yOffset+7, Color::FromHSV(h+315, 1, 1));
            UI::Graphics::DrawString("           \\/___/", -1, xOffset, yOffset+8, Color::FromHSV(h, 1, 1));

            //for(int i = 0; i < 5000; i++) i;
            h = ++h % 360;
        }*/
    }

    Terminal::Println("It's morbin' time");
    

    VMM::SBRK(PAGE_SIZE);
    Terminal::Println("Heap mapping: %x, heap size: %d", VMM::GetPhysAddr((void*)VMM::KERNEL_HEAP_START), VMM::GetHeapSize());

    VMM::SBRK(2*PAGE_SIZE);
    Terminal::Println("Heap mapping: %x, heap size: %d", VMM::GetPhysAddr((void*)VMM::KERNEL_HEAP_START + 2 * PAGE_SIZE), VMM::GetHeapSize());

    VMM::SBRK(-3*PAGE_SIZE);
    Terminal::Println("Heap mapping: %x, heap size: %d", VMM::GetPhysAddr((void*)VMM::KernelHeapEnd), VMM::GetHeapSize());

    VMM::SBRK(2*PAGE_SIZE);
    Terminal::Println("Heap mapping: %x, heap size: %d", VMM::GetPhysAddr((void*)VMM::KERNEL_HEAP_START + PAGE_SIZE), VMM::GetHeapSize());

    /*
    void* testBuffer[100];

    for(int i = 0; i < 100; i++){
        testBuffer[i] = malloc(i*10);
        for(int j = 0; j < i * 10; j++){
            *((uint8_t*)testBuffer[i] + j) = 0;
        }
        Terminal::Println("Malloc %d %x", i, testBuffer[i]);
        Terminal::Println("Physical %x", VMM::GetPhysAddr(testBuffer[i]));
    }

    for(int i = 0; i < 100; i++){
        free(testBuffer[i]);
        //Terminal::Println("Check %x", testBuffer[i]);
    }*/

    Terminal::Println("No interrupts");
    while(true);
}

void enableFPU(){
    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x200;
    asm volatile("mov %0, %%cr4" :: "r"(cr4));
    const uint16_t controlWord = 0x37f;
    asm volatile("fldcw %0" :: "m"(controlWord));
}