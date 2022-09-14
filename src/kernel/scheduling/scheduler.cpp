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
    size_t taskStackSize = 1048576;

    IsAlive = false;
    ShouldStop = false;
    
    stack = malloc(taskStackSize);
    CpuState = (cpu_state*)((uintptr_t)stack + taskStackSize - sizeof(cpu_state));

    CpuState->retip = (uint64_t)func;
    CpuState->retrsp = (uintptr_t)stack + taskStackSize;
    CpuState->rflags = 0b001000000010;
    CpuState->ss = 0;//(uintptr_t)stack;
    CpuState->cs = 0x8;
}

void Task::Start(){
    Scheduler::StartTask(this);
}

void Task::Stop(){
    ShouldStop = true;
    State = TaskState::Stopping;
}

void Task::Sleep(int milliseconds){
    SleepTicks = milliseconds;
    State = TaskState::Sleeping;
}

void Task::Wait(int milliseconds){
    Task* t = Scheduler::GetCurrentTask();
    t->Sleep(milliseconds);
    while(t->SleepTicks > 0);
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

    Spinlock taskStartLock;

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
        taskStartLock.Acquire();

        for(int i = taskCount; i < MAX_TASK_COUNT; i++){
            if(Tasks[i].State == TaskState::Stopped){
                Tasks[i] = t;
                Tasks[i].ID = idCounter;
                Tasks[i].State = TaskState::Idle;
                idCounter++;
                taskCount++;
                taskStartLock.Unlock();

                Terminal::Println("Started task %ld", Tasks[i].ID);
                return &Tasks[i];
            }
        }

        taskStartLock.Unlock();
        return nullptr;
    }

    void Schedule(cpu_state* state){
        if(currentTask != -1){
            Tasks[currentTask].CpuState = state;
            if(Tasks[currentTask].State == TaskState::Running)
                Tasks[currentTask].State = TaskState::Idle;
        } else if(idle->IsAlive){
            idle->CpuState = state;
        }

        if(taskCount == 0){
            currentTask = -1;
            idle->CpuTime++;
            load_cpu_state(idle->CpuState);
        }

        bool nextTaskFound = false;
        int nextTask = -1;

        for(int i = 1; i <= taskCount; i++){
            Task* t = &Tasks[(currentTask + i) % taskCount];

            switch(t->State){
                case TaskState::Idle:{
                    if(!nextTaskFound){
                        nextTaskFound = true;
                        nextTask = (currentTask + i) % taskCount;
                    }
                    break;
                };
                case TaskState::Sleeping:{
                    t->SleepTicks--;
                    if(t->SleepTicks == 0){
                        t->State = TaskState::Idle;
                    }
                    break;
                };
                case TaskState::Stopping:{
                    Tasks[(currentTask + i) % taskCount] = Tasks[taskCount - 1];
                    Tasks[taskCount - 1] = nullptr;
                    t->State = TaskState::Stopped;
                    taskCount--;
                    Terminal::Println("Stopped task %ld", t->ID);
                    break;
                }
            }
        }

        if(nextTaskFound){
            currentTask = nextTask;
            Tasks[currentTask].State = TaskState::Running;
            Tasks[currentTask].CpuTime++;
            load_cpu_state(Tasks[currentTask].CpuState);
        } else {
            currentTask = -1;
            idle->CpuTime++;
            load_cpu_state(idle->CpuState);
        }
    }

    Task* GetCurrentTask(){
        if(currentTask == -1) return nullptr;
        return &Tasks[currentTask];
    }

    uint64_t GetIdleTime(){
        return idle->CpuTime;
    }
}