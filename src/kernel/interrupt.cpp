#include <kernel/interrupt.h>

InterruptHandler handlers[256];

extern "C" void Handler0x0(void);
extern "C" void Handler0x1(void);
extern "C" void Handler0x2(void);
extern "C" void Handler0x3(void);
extern "C" void Handler0x4(void);
extern "C" void Handler0x5(void);
extern "C" void Handler0x6(void);
extern "C" void Handler0x7(void);
extern "C" void Handler0x8(void);
extern "C" void Handler0x9(void);
extern "C" void Handler0xA(void);
extern "C" void Handler0xB(void);
extern "C" void Handler0xC(void);
extern "C" void Handler0xD(void);
extern "C" void Handler0xE(void);
extern "C" void Handler0xF(void);
extern "C" void Handler0x10(void);
extern "C" void Handler0x11(void);
extern "C" void Handler0x12(void);
extern "C" void Handler0x13(void);
extern "C" void Handler0x14(void);
extern "C" void Handler0x15(void);
extern "C" void Handler0x16(void);
extern "C" void Handler0x17(void);
extern "C" void Handler0x18(void);
extern "C" void Handler0x19(void);
extern "C" void Handler0x1A(void);
extern "C" void Handler0x1B(void);
extern "C" void Handler0x1C(void);
extern "C" void Handler0x1D(void);
extern "C" void Handler0x1E(void);
extern "C" void Handler0x1F(void);
extern "C" void PitHandler(void);
extern "C" void Handler0x21(void);
extern "C" void Handler0x22(void);
extern "C" void Handler0x23(void);
extern "C" void Handler0x24(void);
extern "C" void Handler0x25(void);
extern "C" void Handler0x26(void);
extern "C" void Handler0x27(void);
extern "C" void Handler0x28(void);
extern "C" void Handler0x29(void);
extern "C" void Handler0x2A(void);
extern "C" void Handler0x2B(void);
extern "C" void Handler0x2C(void);

void BSOD(char* s){
    asm("cli");

    UI::Graphics::DrawBSOD(s);
    
    for(int i = 0; i < 1000000000; i++){ i; }
    //IO::Out(0x64, 0xFE);
    asm("hlt");
}

void DivByZeroHandler(){
    BSOD("You divided by zero, nerd");
}

void PageFaultHandler(){
    //BSOD("Faulty page access");

    uintptr_t faultingAddress;
    asm("mov %0, %%cr2" : "=r"(faultingAddress));
    
    Graphics::DrawBox({0, 0}, {100, 100}, Color::Red);

    Terminal::Println("Faulting address: %lx", faultingAddress);
    asm("hlt");
}

void UnhandledInterrupt(){
    BSOD("Unhandled interrupt");
}

InterruptTableEntry::InterruptTableEntry(){
    type_attributes = IDT_InterruptGate;
    selector = 0x8;
}

InterruptTableEntry::InterruptTableEntry(void(*handler)()){
    type_attributes = IDT_InterruptGate;
    selector = 0x8;
    SetOffset(handler);
}

void InterruptTableEntry::SetOffset(void(*handler)()){
    offset_0 = (uint16_t)(((uintptr_t)handler & 0x000000000000ffff));
    offset_1 = (uint16_t)(((uintptr_t)handler & 0x00000000ffff0000) >> 16);
    offset_2 = (uint32_t)(((uintptr_t)handler & 0xffffffff00000000) >> 32);
}

namespace InterruptManager{
    uint16_t limit = 0;
    uint64_t base = 0;

    InterruptTableEntry idt[256];

    void Initialize(){
        limit = sizeof(InterruptTableEntry) * 256 - 1;
        base = (uint64_t)&idt;

        for(int i = 0; i < sizeof(InterruptTableEntry) * 256; i++){
            *((uint8_t*)&idt + i) = 0;
        }
        
        SetInterruptEntry(0x0, InterruptTableEntry(&Handler0x0), &DivByZeroHandler);
        SetInterruptEntry(0x1, InterruptTableEntry(&Handler0x1), &UnhandledInterrupt);
        SetInterruptEntry(0x2, InterruptTableEntry(&Handler0x2), &UnhandledInterrupt);
        SetInterruptEntry(0x3, InterruptTableEntry(&Handler0x3), &UnhandledInterrupt);
        SetInterruptEntry(0x4, InterruptTableEntry(&Handler0x4), &UnhandledInterrupt);
        SetInterruptEntry(0x5, InterruptTableEntry(&Handler0x5), &UnhandledInterrupt);
        SetInterruptEntry(0x6, InterruptTableEntry(&Handler0x6), &UnhandledInterrupt);
        SetInterruptEntry(0x7, InterruptTableEntry(&Handler0x7), &UnhandledInterrupt);
        SetInterruptEntry(0x8, InterruptTableEntry(&Handler0x8), &UnhandledInterrupt);
        SetInterruptEntry(0x9, InterruptTableEntry(&Handler0x9), &UnhandledInterrupt);
        SetInterruptEntry(0xA, InterruptTableEntry(&Handler0xA), &UnhandledInterrupt);
        SetInterruptEntry(0xB, InterruptTableEntry(&Handler0xB), &UnhandledInterrupt);
        SetInterruptEntry(0xC, InterruptTableEntry(&Handler0xC), &UnhandledInterrupt);
        SetInterruptEntry(0xD, InterruptTableEntry(&Handler0xD), &UnhandledInterrupt);
        SetInterruptEntry(0xE, InterruptTableEntry(&PageFaultHandler), &UnhandledInterrupt);
        SetInterruptEntry(0xF, InterruptTableEntry(&Handler0xF), &UnhandledInterrupt);

        SetInterruptEntry(0x20, InterruptTableEntry(&PitHandler), &UnhandledInterrupt);
        SetInterruptEntry(0x21, InterruptTableEntry(&Handler0x21), &UnhandledInterrupt);
        SetInterruptEntry(0x22, InterruptTableEntry(&Handler0x22), &UnhandledInterrupt);
        SetInterruptEntry(0x23, InterruptTableEntry(&Handler0x23), &UnhandledInterrupt);
        SetInterruptEntry(0x24, InterruptTableEntry(&Handler0x24), &UnhandledInterrupt);
        SetInterruptEntry(0x25, InterruptTableEntry(&Handler0x25), &UnhandledInterrupt);
        SetInterruptEntry(0x26, InterruptTableEntry(&Handler0x26), &UnhandledInterrupt);
        SetInterruptEntry(0x27, InterruptTableEntry(&Handler0x27), &UnhandledInterrupt);
        SetInterruptEntry(0x28, InterruptTableEntry(&Handler0x28), &UnhandledInterrupt);
        SetInterruptEntry(0x29, InterruptTableEntry(&Handler0x29), &UnhandledInterrupt);
        SetInterruptEntry(0x2A, InterruptTableEntry(&Handler0x2A), &UnhandledInterrupt);
        SetInterruptEntry(0x2B, InterruptTableEntry(&Handler0x2B), &UnhandledInterrupt);
        SetInterruptEntry(0x2C, InterruptTableEntry(&Handler0x2C), &UnhandledInterrupt);

        struct idt_ptr{
            uint16_t limit;
            uint64_t base;
        } __attribute__((packed));

        struct idt_ptr ptr = {limit, base};
        asm volatile("lidt %0" :: "m"(ptr));
    }

    void RemapPIC(){
        uint8_t a1, a2;
        a1 = IO::In(PIC1_DATA_PORT);
        IO::Wait();
        a2 = IO::In(PIC2_DATA_PORT);
        IO::Wait();

        IO::Out(PIC1_COMMAND_PORT, 0x11);
        IO::Out(PIC2_COMMAND_PORT, 0x11);

        IO::Out(PIC1_DATA_PORT, 0x20);
        IO::Out(PIC2_DATA_PORT, 0x28);

        IO::Out(PIC1_DATA_PORT, 0x04);
        IO::Out(PIC2_DATA_PORT, 0x02);

        IO::Out(PIC1_DATA_PORT, 0x01);
        IO::Out(PIC2_DATA_PORT, 0x01);

        IO::Out(PIC1_DATA_PORT, a1);
        IO::Out(PIC2_DATA_PORT, a2);
        
        IO::Out(PIC1_DATA_PORT, 0b11111000);
        IO::Out(PIC2_DATA_PORT, 0b11101111);
    }

    void SetInterruptEntry(uint8_t irq, InterruptTableEntry entry, void (*handler)()){
        idt[irq] = entry;
        InterruptHandler interruptHandler = {irq, handler};
        handlers[irq] = interruptHandler;
    }

    void SetHandler(uint8_t irq, void (*handler)()){
        handlers[irq].handler = handler;
    }

    void HandleInterrupt(int_frame_t* frame){
        if((void*)handlers[frame->int_number].handler == (void*)&UnhandledInterrupt){
            Terminal::Println("%crUnhandled interrupt: irq: %d%cw", frame->int_number);
            Terminal::Println("Retip: %lx", frame->retip);
            asm("hlt");
        }

        handlers[frame->int_number].handler();
        
        if(frame->int_number >= 0x20 && frame->int_number < 0x30){
            IO::Out(PIC1_COMMAND_PORT, 0x20);
            if(frame->int_number >= 0x2A){
                IO::Out(PIC2_COMMAND_PORT, 0x20);
            }
        }
    }
}