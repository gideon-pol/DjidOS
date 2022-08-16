#ifndef TIMER_H
#define TIMER_H

#include <common/common.h>
#include <kernel/io.h>
#include <kernel/interrupt.h>

#define PIT_FREQUENCY 1193182
#define PIT_COMMAND_PORT 0x43
#define PIT_IRQ 0x20

#define TIMER_FREQUENCY 100

namespace Timer{
    void Setup();
    uint64_t GetUptime();
}

#endif