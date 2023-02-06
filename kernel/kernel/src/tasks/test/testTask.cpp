#include "testTask.h"
#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"

TaskTest::TaskTest(uint64_t ms)
{
    done = false;
    type = TaskType::SLEEP;
    startTime = PIT::TimeSinceBootMS();
    endTime = startTime + ms;
}


void TaskTest::Do()
{
    if (PIT::TimeSinceBootMS() >= endTime)
        done = true;
}

TaskTest* NewTestTask(uint64_t ms)
{
    TaskTest* task = (TaskTest*)_Malloc(sizeof(TaskTest), "New Test Task");
    *task = TaskTest(ms);
    return task;
}

void TaskTest::Free()
{
    
}