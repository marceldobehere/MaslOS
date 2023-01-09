#pragma once
#include "../task.h"
#include "../../WindowStuff/Window/window.h"
#include <stdint.h>

class TaskTaskManager : public Task
{
    private:    
        Window* window;
        int callsPerFrame = 200;
        int callCount = 0;
        int totalCount = 0;
        const char* oldTitle;
        uint64_t nextTime = 0;
        kernelFiles::ImageFile* oldIcon;

    public:
        bool exit = false;
        TaskTaskManager(Window* window);
        void Do();
        void Free();

};

TaskTaskManager* NewTaskManagerTask(Window* window);