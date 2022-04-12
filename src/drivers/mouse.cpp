#include <drivers/mouse.h>
#include <kernel/io.h>
#include <kernel/interrupt.h>
#include <interface.h>


MouseDriver mouseDriver = MouseDriver();

void MouseDriver::Initialize(){
    IO::Out(0x64, 0xA8);
    IO::Out(0x64, 0x20);
    uint8_t status = IO::In(0x60);
    status |= 0x2;
    //status &= ~(0x1 << 5);
    IO::Out(0x64, 0x60);
    IO::Out(0x60, status);

    IO::Out(0x64, 0xD4);
    IO::Out(0x60, 0xF6);
    
    IO::Out(0x64, 0xD4);    
    IO::Out(0x60, 0xF4);

    IO::In(0x60);

    InterruptManager::SetHandler(0x2C, (void (*)())&MouseDriver::HandleInterrupt);
}

void MouseDriver::HandleInterrupt(){
    uint8_t scanCode = IO::In(0x60);

    Interface::Clear();
    Interface::Print(scanCode, 6);
}