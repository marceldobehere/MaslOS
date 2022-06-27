#pragma once
#include "../task.h"
#include "../../WindowStuff/Window/window.h"


class TaskWindowClose : public Task
{
    private:
        Window* window;

    public:
        TaskWindowClose(Window* window);
        void Do();
        void Free();
};

TaskWindowClose* NewWindowCloseTask(Window* window);