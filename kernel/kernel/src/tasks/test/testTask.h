#pragma once
#include "../task.h"
#include <stdint.h>

class TaskTest : public Task
{
    private:
        uint64_t startTime;
        uint64_t endTime;

    public:

        //TaskSleep(double time);
        TaskTest(uint64_t ms);
        void Do();
        void Free();
};

//TaskSleep* NewSleepTask(double time);
TaskTest* NewTestTask(uint64_t ms);