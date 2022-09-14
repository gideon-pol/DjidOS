#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <kernel/interrupt.h>
#include <kernel/scheduling/time.h>
#include <kernel/scheduling/locking.h>
#include <kernel/memory.h>

#define MAX_TASK_COUNT 64

typedef struct cpu_state{
    uint64_t rbx, r15, r14, r13;
    uint64_t r12, r11, r10, r9, r8, rdi, rsi, rbp, rdx, rcx, rax;
    uint64_t retip, cs, rflags, retrsp, ss;
} cpu_state_t __attribute__((packed));

enum TaskState { Stopped, Stopping, Idle, Running, Sleeping };

class Task{
public:
    Task(){};
    Task(void* func);
    void* operator new(size_t size, void* func);

    void Start();
    void Stop();
    void Sleep(int milliseconds);
    TaskState State = TaskState::Stopped;

    bool IsAlive = false;
    bool ShouldStop = false;

    uint64_t ID = -1;

    uint64_t CpuTime = 0;
    uint64_t CpuSecond = 0;
    volatile uint64_t SleepTicks = 0;

    cpu_state* CpuState;

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
    void Schedule(cpu_state_t* state);
    Task* GetCurrentTask();
    uint64_t GetIdleTime();
}

#endif