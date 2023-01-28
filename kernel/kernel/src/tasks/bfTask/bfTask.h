#pragma once
#include "../task.h"
#include "../../WindowStuff/Window/window.h"
#include <stdint.h>

class TaskBF : public Task
{
    private:    
        const char* code;
        uint8_t* mem;
        uint64_t len;
        uint64_t memLen;
        int64_t pIndex;
        int64_t mIndex;
        bool waitInput;
        Window* window;
        int cyclesPerCall = 500;

    public:
        TaskBF(const char* code, Window* window);
        void Do();
        void Free();

};

TaskBF* NewBFTask(const char* code, Window* window);