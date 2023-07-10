#pragma once
#include "../task.h"
#include <stdint.h>

class TaskWriteFile : public Task
{
    private:
        

    public:
        const char* path;
        char* data;
        uint64_t dataLen;
        uint64_t dataPos;
        const static int MaxBufferStep = 1024 * 128;

        TaskWriteFile(const char* path, char* buffer, uint64_t len);
        void Do();
        void SetDone();
        void Free();
};

//TaskSleep* NewSleepTask(double time);
TaskWriteFile* NewWriteFileTask(const char* path, char* buffer, uint64_t len);