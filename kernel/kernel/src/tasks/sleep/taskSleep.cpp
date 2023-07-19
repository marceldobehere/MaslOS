#include "taskSleep.h"
#include "../../devices/pit/pit.h"
#include "../../memory/heap.h"


TaskSleep::TaskSleep(uint64_t ms)
{
    done = false;
    type = TaskType::SLEEP;
    startTime = PIT::TimeSinceBootMS();
    endTime = startTime + ms;

    this->TaskText = "<SLEEP TASK>";
    this->DoTaskFunc = (void(*)(void*))(void*)&Do;
    this->FreeTaskFunc = (void(*)(void*))(void*)&Free;
}


void TaskSleep::Do()
{
    if (PIT::TimeSinceBootMS() >= endTime)
        done = true;
}

// TaskSleep* NewSleepTask(double time)
// {
//     TaskSleep* task = (TaskSleep*)malloc(sizeof(TaskSleep));
//     *task = TaskSleep(time);
//     return task;
// }

TaskSleep* NewSleepTask(uint64_t ms)
{
    TaskSleep* task = new TaskSleep(ms);
    return task;
}

void TaskSleep::Free()
{
    
}