#pragma once
#include "../task.h"
#include <stdint.h>

class TaskSimpleData : public Task
{
    private:
        

    public:
        void* data;

        TaskSimpleData();
        void Do();
        void SetDone();
        void Free();
};

//TaskSleep* NewSleepTask(double time);
TaskSimpleData* NewSimpleDataTask();