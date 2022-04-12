#include <interface.h>
#include <kernel/interrupt.h>
#include <kernel/io.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

extern "C"
void kernel_main(){
    InterruptManager::Initialize();
    Interface::Clear();
    keyboardDriver.Initialize();
    mouseDriver.Initialize();
    InterruptManager::RemapPIC();

    Interface::FillRow(0, '#');
    Interface::FillRow(24, '#');

    Interface::Print("DjidOS - beste OS", 1);
    Interface::Print("this is a test", 2);

    int j = 0;
    while(true){
        for(int i = 0; i < 500000000; i++){
            continue;
        }
        Interface::Clear(); 
        Interface::Print(j, 1);
        j++;
    }
}
