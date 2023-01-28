#pragma once
#include "../task.h"
#include <stdint.h>

class TaskSleep : public Task
{
    private:
        uint64_t startTime;
        uint64_t endTime;

    public:

        //TaskSleep(double time);
        TaskSleep(uint64_t ms);
        void Do();
        void Free();
};

//TaskSleep* NewSleepTask(double time);
TaskSleep* NewSleepTask(uint64_t ms);