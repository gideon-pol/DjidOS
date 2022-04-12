#include <kernel/interrupt_stubs.h>

#define IRQ_DEF(index)                                    \
__attribute__((interrupt)) void Handler##index(struct interrupt_frame* frame){ \
    InterruptManager::HandleInterrupt(index);            \
}

IRQ_DEF(0x0);
IRQ_DEF(0x1);
IRQ_DEF(0x2);
IRQ_DEF(0x3);
IRQ_DEF(0x4);
IRQ_DEF(0x5);
IRQ_DEF(0x6);
IRQ_DEF(0x7);
IRQ_DEF(0x8);
IRQ_DEF(0x9);
IRQ_DEF(0xA);
IRQ_DEF(0xB);
IRQ_DEF(0xC);
IRQ_DEF(0xD);
IRQ_DEF(0xE);
IRQ_DEF(0xF);


IRQ_DEF(0x20);
IRQ_DEF(0x21);
IRQ_DEF(0x22);
IRQ_DEF(0x23);
IRQ_DEF(0x24);
IRQ_DEF(0x25);
IRQ_DEF(0x26);
IRQ_DEF(0x27);
IRQ_DEF(0x28);
IRQ_DEF(0x29);
IRQ_DEF(0x2A);
IRQ_DEF(0x2B);
IRQ_DEF(0x2C);