#include <kernel/interrupt.h>
#include <drivers/keyboard.h>
#include <kernel/memory.h>

void DivByZeroHandler(){
    Interface::Clear();
    Interface::Print("You divided by zero, nerd - rebooting", 0, -1, Color::White, Color::Red);
    while(true);
}

void PageFaultHandler(){
    Interface::Clear();
    Interface::Print("Faulty page access - rebooting", 0, -1, Color::White, Color::Red);
    while(true);
}

void UnhandledInterrupt(){
    Interface::Clear();
    Interface::Print("Unforseen interrupt occurred - rebooting", 0, -1, Color::White, Color::Red);
    while(true);
}

InterruptTableEntry::InterruptTableEntry(){
    type_attributes = IDT_InterruptGate;
    selector = 0x8;
}

InterruptTableEntry::InterruptTableEntry(void(*handler)(struct interrupt_frame* frame)){
    type_attributes = IDT_InterruptGate;
    selector = 0x8;
    SetOffset(handler);
}

void InterruptTableEntry::SetOffset(void(*handler)(struct interrupt_frame* frame)){
    offset_0 = (uint16_t)(((uint64_t)handler & 0x000000000000ffff));
    offset_1 = (uint16_t)(((uint64_t)handler & 0x00000000ffff0000) >> 16);
    offset_2 = (uint32_t)(((uint64_t)handler & 0xffffffff00000000) >> 32);
}

namespace InterruptManager{
    uint16_t limit = 0;
    uint64_t base = 0;

    InterruptTableEntry idt[256];
    InterruptHandler handlers[256];

    void Initialize(){
        limit = sizeof(InterruptTableEntry) * 256 - 1;
        base = (uint64_t)&idt;

        for(int i = 0; i < sizeof(InterruptTableEntry) * 256; i++){
            *(uint8_t*)(&idt + i) = 0;
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
        SetInterruptEntry(0xE, InterruptTableEntry(&Handler0xE), &PageFaultHandler);
        SetInterruptEntry(0xF, InterruptTableEntry(&Handler0xF), &UnhandledInterrupt);

        SetInterruptEntry(0x20, InterruptTableEntry(&Handler0x20), &UnhandledInterrupt);
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
        
        IO::Out(PIC1_DATA_PORT, 0b11111001);
        IO::Out(PIC2_DATA_PORT, 0b11101111);
        asm("sti");
    }

    void SetInterruptEntry(uint8_t irq, InterruptTableEntry entry, void (*handler)()){
        idt[irq] = entry;
        InterruptHandler interruptHandler = {irq, handler};
        handlers[irq] = interruptHandler;
    }

    void SetHandler(uint8_t irq, void (*handler)()){
        handlers[irq].handler = handler;
    }

    void HandleInterrupt(uint8_t irq){
        if(handlers[irq].handler != UnhandledInterrupt){
            handlers[irq].handler();
        } else {
            Interface::Clear();
            Interface::Print("Unhandled interrupt occured, code: ", 0, -1, Color::White, Color::Red);
            Interface::Print(irq, 1, Color::White, Color::Red);
            while(true);
        }
        
        Interface::FillRow(23, '^');
        if(irq >= 0x20 && irq < 0x30){
            IO::Out(PIC1_COMMAND_PORT, 0x20);
            if(irq >= 0x2A){
                IO::Out(PIC2_COMMAND_PORT, 0x20);
            }
        }
    }
}