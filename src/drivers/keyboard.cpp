#include <drivers/keyboard.h>
#include <kernel/io.h>
#include <interface.h>
#include <kernel/interrupt.h>

KeyboardDriver keyboardDriver = KeyboardDriver();

void KeyboardDriver::Initialize(){
    InterruptManager::SetHandler(0x21, (void(*)())&KeyboardDriver::HandleInterrupt);
}

void KeyboardDriver::HandleInterrupt(){
    uint8_t scanCode = IO::In(0x60);

    KeyCode code = KeyCode::FromScanCode(scanCode);

    char d = code.ToChar();
    UI::Old::Print(&d, 7, 1);

    while(IO::In(0x64) & 0x1) IO::In(0x60);
}