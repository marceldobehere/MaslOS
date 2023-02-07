#pragma once
#include "../task.h"
#include <stdint.h>
#include "../../WindowStuff/Window/window.h"
#include "commonStructs.h"

class TaskTest : public Task
{
    private:
        void* dataCopy;
        uint64_t dataLen;
        Window* window;
        KernelAppData kernelAppData;

    public:

        //TaskSleep(double time);
        TaskTest(void* data, uint64_t len, Window* window);
        void Do();
        void Free();



        void load_obj(void* data, uint64_t len);
        int DoStuff(void* data, uint16_t len);
        void execute_funcs();



};

//TaskSleep* NewSleepTask(double time);
TaskTest* NewTestTask(void* data, uint64_t len, Window* window);