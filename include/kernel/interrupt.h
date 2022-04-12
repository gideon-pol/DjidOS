#ifndef IDT_H
#define IDT_H

//#include <stdio>
//#include <cstring>

#include <common.h>
#include <interface.h>
#include <kernel/io.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <kernel/interrupt_stubs.h>

#define IDT_InterruptGate 0b10001110

#define PIC_IRQ_OFFSET 0x20

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

class InterruptTableEntry{
public:
    InterruptTableEntry();
    InterruptTableEntry(void(*func)(struct interrupt_frame* frame));
    void SetOffset(void(*func)(struct interrupt_frame* frame));

private:
    uint16_t offset_0;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attributes;
    uint16_t offset_1;
    uint32_t offset_2;
    uint32_t zero = 0;

} __attribute__((packed));

struct InterruptHandler{
    uint8_t irq;
    void(*handler)();
} __attribute__((packed));

void DivByZeroHandler();
void PageFaultHandler();
void UnhandledInterrupt();

namespace InterruptManager{
    void Initialize();
    void RemapPIC();
    void SetInterruptEntry(uint8_t irq, InterruptTableEntry entry, void (*handler)());
    void SetHandler(uint8_t irq, void (*handler)());
    void HandleInterrupt(uint8_t irq);
}

#endif