#pragma once
#include "../task.h"
#include "../../WindowStuff/Window/window.h"



class TaskDoom : public Task
{
    private:
    Window* window;

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
//extern List<void*>* openFIles;
extern TaskDoom* currentDoom;

//TaskSleep* NewSleepTask(double time);
TaskDoom* NewDoomTask(Window* window);