#include <interface.h>
#include <kernel/interrupt.h>
#include <kernel/io.h>
#include <kernel/multiboot2.h>
#include <kernel/memory.h>
#include <kernel/scheduling/time.h>
#include <kernel/scheduling/scheduler.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

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
            }
        }
    }

    PMM::Initialize(info.largest_available_memory_section);
    VMM::SetupPaging((void*)((uintptr_t)PMM::bitmap.Map + PMM::bitmap.Size), 1);

    kernel_main(info);
}

void Task1(){
    int i = 0;
    while(true){
        DrawBox(0, 0, CurrentFrame.Width, CurrentFrame.Height, Color::White);
        Task::Wait(1000);
        DrawBox(0, 0, CurrentFrame.Width, CurrentFrame.Height, Color::Black);
        Task::Wait(1000);
    }
}

void Task2(){
    int width = 55;
    int height = 9;
    int xOffset = (CurrentFrame.GetColumns() - width) / 2;
    int yOffset = (CurrentFrame.GetRows() - height * 1.5) / 2;

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

        h = ++h % 360;
    }
}

void Profiler(){
    Task* t = Scheduler::GetCurrentTask();

    while(true){
        int j = 0;
        for(int i = 0; i < MAX_TASK_COUNT; i++){
            if(Scheduler::Tasks[i].IsAlive){
                if(t == &Scheduler::Tasks[i]){
                    Terminal::Print("%d. Task %d (Profiler), used cpu time: %ld", 0, j, j, Scheduler::Tasks[i].ID, Scheduler::Tasks[i].CpuTime);
                    
                } else {
                    Terminal::Print("%d. Task %d, used cpu time: %ld", 0, j, j, Scheduler::Tasks[i].ID, Scheduler::Tasks[i].CpuTime);
                }
                j++;
            }
        }

        Task::Wait(100);
    }
}

void kernel_main(KernelInfo info){
    InterruptManager::Initialize();
    
    UI::Graphics::Setup(info.frame_width, info.frame_height, info.framebuffer);    
    Terminal::Setup();

    keyboardDriver.Initialize();
    mouseDriver.Initialize();
    Time::Setup();

    enableFPU();

    Scheduler::Initialize();

    Task task1 = Task((void*)&Task1);
    Task task2 = Task((void*)&Task2);
    Task profiler = Task((void*)&Profiler);

    Scheduler::StartTask(task1);
    Scheduler::StartTask(task2);
    InterruptManager::RemapPIC();

    Scheduler::StartTask(profiler);
    asm("hlt");

/*
    void* testBuffer[10000];

    for(int i = 0; i < 100; i++){
        Terminal::Println("Malloc %d, %d", i, i*10000);

        testBuffer[i] = malloc(i*10000);
        if(testBuffer[i] != nullptr){
            if(VMM::GetPhysAddr(testBuffer[i]) == (void*)-1){
                Terminal::Println("Virtual %lx", testBuffer[i]);

                Terminal::Println("Physical %lx", VMM::GetPhysAddr(testBuffer[i]));
                Terminal::Println("Something went wrong with the paging!");
                while (true);
            }
            for(int j = 0; j < i * 10; j++){
                *((uint8_t*)testBuffer[i] + j) = 0;
            }

            //Terminal::Println("Virtual %lx", testBuffer[i]);
            //Terminal::Println("Physical %lx", VMM::GetPhysAddr(testBuffer[i]));
        } else {
            Terminal::Println("Out of memory!");
            while (true);       
        }
    }

    for(int i = 0; i < 100; i++){
        Terminal::Println("Freeing %lx", testBuffer[i]);

        free(testBuffer[i]);
    }

    void* x = malloc(100);*/
}

void enableFPU(){
    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x200;
    asm volatile("mov %0, %%cr4" :: "r"(cr4));
    const uint16_t controlWord = 0x37f;
    asm volatile("fldcw %0" :: "m"(controlWord));
}