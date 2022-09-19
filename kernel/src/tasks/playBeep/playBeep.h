#pragma once
#include "../task.h"
#include <stdint.h>

class TaskPlayBeep : public Task
{
    private:
        uint64_t startTime;
        uint64_t endTime;
        uint64_t onDuration; 
        uint64_t offDuration;
        uint64_t totalDuration;
        bool currentState;
        uint64_t switchTime;

    public:

        TaskPlayBeep(uint64_t onDuration, uint64_t offDuration, uint64_t totalDuration);
        void Do();
        void Free();
};

TaskPlayBeep* NewBeepTask(uint64_t onDuration, uint64_t offDuration, uint64_t totalDuration);