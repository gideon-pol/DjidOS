#include <interface.h>
#include <kernel/interrupt.h>
#include <kernel/io.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

void enableFPU();

extern "C"
void kernel_main(){
    InterruptManager::Initialize();
    Interface::Clear();
    keyboardDriver.Initialize();
    mouseDriver.Initialize();
    InterruptManager::RemapPIC();

    enableFPU();

    Interface::FillRow(0, '#');
    Interface::FillRow(24, '#');

    Interface::Print("DjidOS - beste OS", 1);
    Interface::Print("this is a test", 2);

    while(true);
    
    /*
    int j = 0;
    while(true){
        for(int i = 0; i < 500000000; i++){
            continue;
        }
        Interface::Clear(); 
        Interface::Print(j, 1);
        //Interface::MoveCursor(10,10);

        j++;
    }*/
}

void enableFPU(){
    size_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x200;
    asm volatile("mov %0, %%cr4" :: "r"(cr4));
    const uint16_t controlWord = 0x37f;
    asm volatile("fldcw %0" :: "m"(controlWord));
}
