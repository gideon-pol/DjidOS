#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <kernel/interrupt.h>
#include <kernel/scheduling/time.h>
#include <kernel/memory.h>

#define MAX_TASK_COUNT 64

class Task{
public:
    Task(){};
    Task(void* func);
    void Start();
    void Stop();
    void Sleep(int milliseconds);
    bool IsSleeping();
    bool IsAlive = false;
    
    bool ShouldStop = false;

    uint64_t ID = -1;

    uint64_t CpuTime = 0;
    volatile uint64_t SleepTicks = 0;

    cpu_state* State;

    void* operator new(size_t size, void* func); 

    static void Wait(int milliseconds);
    static void Exit();

private:
    void construct(void* func);
    void* stack;
};

namespace Scheduler{
    extern Task Tasks[MAX_TASK_COUNT];

    void Initialize();
    Task* StartTask(Task t);
    void ScheduleNext(cpu_state* state);
    Task* GetCurrentTask();
    uint64_t GetIdleTime();
}

#endif