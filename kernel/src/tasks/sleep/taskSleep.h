#pragma once
#include "../task.h"


class TaskSleep : public Task
{
    private:
        double startTime;
        double endTime;

    public:

        TaskSleep(double time);
        void Do();
        void Free();
};

TaskSleep* NewSleepTask(double time);