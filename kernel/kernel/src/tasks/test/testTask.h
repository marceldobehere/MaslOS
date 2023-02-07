#pragma once
#include "../task.h"
#include <stdint.h>

class TaskTest : public Task
{
    private:
        void* dataCopy;
        uint64_t dataLen;
    public:

        //TaskSleep(double time);
        TaskTest(void* data, uint64_t len);
        void Do();
        void Free();
};

//TaskSleep* NewSleepTask(double time);
TaskTest* NewTestTask(void* data, uint64_t len);