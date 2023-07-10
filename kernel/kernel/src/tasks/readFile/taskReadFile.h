#pragma once
#include "../task.h"
#include <stdint.h>

class TaskReadFile : public Task
{
    private:
        

    public:
        const char* path;
        char* data;
        int64_t dataLen;
        int64_t dataPos;
        const static int MaxBufferStep = 1024 * 128;

        TaskReadFile(const char* path);
        void Do();
        void SetDone();
        void Free();
};

//TaskSleep* NewSleepTask(double time);
TaskReadFile* NewReadFileTask(const char* path);