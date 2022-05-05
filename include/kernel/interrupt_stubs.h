#ifndef INTERRUPT_STUBS_H
#define INTERRUPT_STUBS_H

#include <kernel/interrupt.h>

struct interrupt_frame;

#define IRQ(index)                                    \
__attribute__((interrupt)) void Handler##index(struct interrupt_frame* frame);                            \

IRQ(0x0);
IRQ(0x1);
IRQ(0x2);
IRQ(0x3);
IRQ(0x4);
IRQ(0x5);
IRQ(0x6);
IRQ(0x7);
IRQ(0x8);
IRQ(0x9);
IRQ(0xA);
IRQ(0xB);
IRQ(0xC);
IRQ(0xD);
IRQ(0xE);
IRQ(0xF);

IRQ(0x20);
IRQ(0x21);
IRQ(0x22);
IRQ(0x23);
IRQ(0x24);
IRQ(0x25);
IRQ(0x26);
IRQ(0x27);
IRQ(0x28);
IRQ(0x29);
IRQ(0x2A);
IRQ(0x2B);
IRQ(0x2C);

#endif