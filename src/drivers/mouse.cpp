#include <drivers/mouse.h>

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

void MouseDriver::HandleInterrupt(int_frame* frame){
    uint8_t status = IO::In(0x64);
    uint8_t buffer[3];
    uint8_t packetNumber = 0;

    while(status & 0x1){
        uint8_t packet = IO::In(0x60);

        if(status & 0x20){
            buffer[packetNumber] = packet;
            packetNumber = (packetNumber + 1) % 3;
 
            if(packetNumber == 0){
                if(buffer[0] & 0b00001000){
                    if(buffer[0] & 0b00000001){
                        //UI::Old::Print("Left mouse button clicked!", 6);
                    }
                    if(buffer[0] & 0b00000010){
                        //UI::Old::Print("Right mouse button clicked!", 6);
                    }

                    float xMovement = (float)(int8_t)buffer[1];
                    float yMovement = (float)(int8_t)buffer[2];

                    //UI::Old::MoveCursor(xMovement * 25, -yMovement * 25);
                }

                while(IO::In(0x64) & 0x1) IO::In(0x60);
                break;
            }
        }

        status = IO::In(0x64);
    }

    IO::Out(PIC1_COMMAND_PORT, 0x20);
    IO::Out(PIC2_COMMAND_PORT, 0x20);
}