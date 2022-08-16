#include <kernel/scheduling/timer.h>

namespace Timer{
    uint64_t count;

    uint64_t GetUptime(){
        return count / TIMER_FREQUENCY;
    }

    void timerCallback(){
        count++;
    }

    void Setup(){
        uint32_t div = PIT_FREQUENCY / TIMER_FREQUENCY;

        IO::Out(PIT_COMMAND_PORT, 0x36);
        IO::Out(0x40, div & 0xFF);
        IO::Out(0x40, (div & 0xFF00) >> 8);

        InterruptManager::SetHandler(PIT_IRQ, &timerCallback);
    }
}