#include <kernel/scheduling/time.h>

namespace Time{
    uint64_t count;

    uint64_t GetUptime(){
        return count;
    }

    void HandleTimerInterrupt(cpu_state* state){
        count++;
        IO::Out(PIC1_COMMAND_PORT, 0x20);
        Scheduler::ScheduleNext(state);
    }

    void Setup(){
        uint32_t div = PIT_FREQUENCY / TIMER_FREQUENCY;

        IO::Out(PIT_COMMAND_PORT, 0x36);
        IO::Out(0x40, div & 0xFF);
        IO::Out(0x40, (div & 0xFF00) >> 8);
    }
}