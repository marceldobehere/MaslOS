#pragma once
#include "../task.h"
#include "../../WindowStuff/Window/window.h"
#include <stdint.h>

class TaskDebugViewer : public Task
{
    private:    
        Window* window;
        int callsPerFrame = 200;
        int callCount = 0;
        int totalCount = 0;
        const char* oldTitle;
        uint64_t nextTime = 0;
        kernelFiles::ImageFile* oldIcon;

        int scrollY;
        bool showAddr;
        bool waitForInput = false;
        int lastL = 0;
        bool lastB = false;
        int64_t currentAddr;

    public:
        bool exit = false;
        TaskDebugViewer(Window* window);
        void Do();
        void Free();

};

TaskDebugViewer* NewDebugViewerTask(Window* window);