#include "taskSleep.h"
#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"

// TaskSleep::TaskSleep(double time)
// {
//     done = false;
//     type = TaskType::SLEEP;
//     startTime = PIT::TimeSinceBoot;
//     endTime = startTime + time;
// }

TaskSleep::TaskSleep(uint64_t ms)
{
    done = false;
    type = TaskType::SLEEP;
    startTime = PIT::TimeSinceBootMS();
    endTime = startTime + ms;
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
    TaskSleep* task = (TaskSleep*)_Malloc(sizeof(TaskSleep), "New Sleep Task");
    *task = TaskSleep(ms);
    return task;
}

void TaskSleep::Free()
{
    
}