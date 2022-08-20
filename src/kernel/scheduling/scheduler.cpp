#include <kernel/scheduling/scheduler.h>

extern "C" void load_cpu_state(cpu_state* state);

Task::Task(void* func){
    construct(func);
}

void* Task::operator new(size_t size, void* func){
    Task* t = (Task*)malloc(size);
    t->construct(func);
    return t;
}

void Task::construct(void* func){
    size_t threadStackSize = 1048576;

    stack = malloc(threadStackSize);
    State = (cpu_state*)((uintptr_t)stack + threadStackSize - sizeof(cpu_state));

    State->retip = (uint64_t)func;
    State->retrsp = (uint64_t)stack + threadStackSize;
    State->rflags = 0b001000000010;
    State->ss = 0x0;
    State->cs = 0x8;
}

void Task::Start(){
    Scheduler::StartTask(this);
}

void Task::Stop(){
    ShouldStop = true;
}

void Task::Sleep(int milliseconds){
    SleepTicks = milliseconds / 10;
    while(SleepTicks > 0);
}

bool Task::IsSleeping(){
    return SleepTicks > 0;
}

void Task::Wait(int milliseconds){
    Scheduler::GetCurrentTask()->Sleep(milliseconds);
}

void Task::Exit(){
    Scheduler::GetCurrentTask()->Stop();
    asm("hlt");
}

namespace Scheduler{
    Task Tasks[MAX_TASK_COUNT];

    int taskCount = 0;
    int currentTask = -1;

    uint64_t idCounter = 0;

    Task* idle;

    void idleTask(){
        idle->IsAlive = true;
        while(true){
            asm("hlt");
        }
    }

    void Initialize(){
        idle = new((void*)&idleTask) Task;
    }

    Task* StartTask(Task t){
        for(int i = 0; i < MAX_TASK_COUNT; i++){
            if(!Tasks[i].IsAlive){
                Tasks[i] = t;
                Tasks[i].IsAlive = true;
                Tasks[i].ID = idCounter;
                idCounter++;
                taskCount++;
                return &Tasks[i];
            }
        }

        return nullptr;
    }

    void ScheduleNext(cpu_state* state){
        if(currentTask != -1){
            Tasks[currentTask].State = state;
        } else if(idle->IsAlive){
            idle->State = state;
        }

        if(taskCount == 0){
            currentTask = -1;
            load_cpu_state(idle->State);
        }

        bool nextTaskFound = false;

        for(int i = 1; i <= MAX_TASK_COUNT; i++){
            Task* t = &Tasks[(currentTask + i) % MAX_TASK_COUNT];
            if(t->IsAlive){
                if(t->ShouldStop){
                    t->IsAlive = false;
                    continue;
                }
                if(t->IsSleeping()){
                    t->SleepTicks--;
                } else if(!nextTaskFound){
                    nextTaskFound = true;
                    currentTask = (currentTask + i) % MAX_TASK_COUNT;
                }
            }
        }

        if(nextTaskFound){
            Tasks[currentTask].CpuTime++;
            load_cpu_state(Tasks[currentTask].State);
        } else {
            currentTask = -1;
            load_cpu_state(idle->State);
        }
    }

    Task* GetCurrentTask(){
        if(currentTask == -1) return nullptr;
        return &Tasks[currentTask];
    }
}