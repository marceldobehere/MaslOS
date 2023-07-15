#pragma once
#include "../task.h"
#include "../../WindowStuff/Window/window.h"
#include "../../cStdLib/list.h"



class TaskDoom : public Task
{
    private:
    Window* window;
    const char* oldTitle;
    bool oldResize;

    public:

        TaskDoom(Window* window);
        void Do();
        void Free();
        void End();
};

extern bool doomRunning;
extern bool doomInit;
extern bool doomKeyboardTemp[];
extern bool doomMouseTemp[];
extern int DOOM_SCALE;
extern List<void*>* openMallocs;
extern TaskDoom* currentDoom;

//TaskSleep* NewSleepTask(double time);
TaskDoom* NewDoomTask(Window* window);