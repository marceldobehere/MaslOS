#pragma once
#include "../task.h"
#include <stdint.h>

class TaskBF : public Task
{
    private:    
        const char* code;
        uint64_t len;
        uint64_t index;

    public:
        TaskBF(const char* code);
        void Do();
        void Free();

};

TaskBF* NewBFTask(const char* code);