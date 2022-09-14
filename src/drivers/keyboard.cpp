#include <drivers/keyboard.h>
#include <kernel/io.h>
#include <interface/interface.h>
#include <kernel/interrupt.h>

KeyboardDriver keyboardDriver = KeyboardDriver();

void KeyboardDriver::Initialize(){
    InterruptManager::SetHandler(KEYBOARD_IRQ, (void(*)())&KeyboardDriver::HandleInterrupt);
}

void KeyboardDriver::HandleInterrupt(){
    uint8_t scanCode = IO::In(0x60);

    KeyCode code = KeyCode::FromScanCode(scanCode);

    char d = code.ToChar();

    if(!(scanCode & SCANCODE_KEY_RELEASED)){
        if(code == KeyCode::Backspace) Terminal::RemoveInputCharacter();
        else if(d != '\0') Terminal::AddInputCharacter(d);
    }

    while(IO::In(0x64) & 0x1) IO::In(0x60);
}