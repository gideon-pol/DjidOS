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

    //IO::Out(0x64, 0xD4);
    //IO::Out(0x60, 0xF6);
    
    IO::Out(0x64, 0xD4);    
    IO::Out(0x60, 0xF4);

    IO::In(0x60);

    InterruptManager::SetHandler(0x2C, (void (*)())&MouseDriver::HandleInterrupt);
}

void MouseDriver::HandleInterrupt(){
    uint8_t status = IO::In(0x64);
    uint8_t buffer[3];
    while(status & 0x1){
        uint8_t packet = IO::In(0x60);
        if(status & 0x20){
            buffer[packetNumber] = packet;
            packetNumber = (packetNumber + 1) % 3;
 
            if(packetNumber == 0){
                Interface::PrintByte(buffer[0], 3);
                Interface::PrintByte(buffer[1], 4);
                Interface::PrintByte(buffer[2], 5);

                if(buffer[0] & 0b00001000){
                    if(buffer[0] & 0b00000001){
                        Interface::Print("Left mouse button clicked!", 6);
                    }
                    if(buffer[0] & 0b00000010){
                        Interface::Print("Right mouse button clicked!", 6);
                    }

                    float xMovement = (float)(int8_t)buffer[1];
                    float yMovement = (float)(int8_t)buffer[2];

                    Interface::MoveCursor(xMovement * 25, -yMovement * 25);
                }

                while(IO::In(0x64) & 0x1) IO::In(0x60);
                break;
            }
        }

        status = IO::In(0x64);
    }
}