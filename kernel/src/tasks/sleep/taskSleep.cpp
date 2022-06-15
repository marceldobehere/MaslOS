#include "taskSleep.h"
#include "../../scheduling-pit/pit.h"
#include "../../memory/heap.h"

TaskSleep::TaskSleep(double time)
{
    done = false;
    type = TaskType::SLEEP;
    startTime = PIT::TimeSinceBoot;
    endTime = startTime + time;
}

void TaskSleep::Do()
{
    if (PIT::TimeSinceBoot >= endTime)
        done = true;
}

TaskSleep* NewSleepTask(double time)
{
    TaskSleep* task = (TaskSleep*)malloc(sizeof(TaskSleep));
    *task = TaskSleep(time);
    return task;
}

void TaskSleep::Free()
{
    
}