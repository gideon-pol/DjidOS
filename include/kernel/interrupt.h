#ifndef IDT_H
#define IDT_H

#include <common/common.h>
#include <interface.h>
#include <kernel/io.h>

#define DISABLE_INTERRUPTS asm("cli")

#define IDT_InterruptGate 0b10001110

#define PIC_IRQ_OFFSET 0x20

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

using namespace UI::Old;

typedef struct {
    uint64_t r11, r10, r9, r8, rdi, rsi, rbp, rdx, rcx, rax;
    uint32_t int_number, err;
    uint64_t retip, cs, rflags, retrsp, ss;
} cpu_state __attribute__((packed));

class InterruptTableEntry{
public:
    InterruptTableEntry();
    InterruptTableEntry(void(*func)());
    void SetOffset(void(*func)());

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

namespace InterruptManager{
    void Initialize();
    void RemapPIC();
    void SetInterruptEntry(uint8_t irq, InterruptTableEntry entry, void (*handler)());
    void SetHandler(uint8_t irq, void (*handler)());
    cpu_state* HandleInterrupt(cpu_state* state);
}

#endif