#include "taskWindowClose.h"
#include "../../devices/pit/pit.h"
#include "../../memory/heap.h"
#include "../../osData/osData.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"

// TaskSleep::TaskSleep(double time)
// {
//     done = false;
//     type = TaskType::SLEEP;
//     startTime = PIT::TimeSinceBoot;
//     endTime = startTime + time;
// }

TaskWindowClose::TaskWindowClose(Window* window)
{
    done = false;
    type = TaskType::CLOSEWINDOW;
    this->window = window;
}


void TaskWindowClose::Do()
{
    AddToStack();
    if (window == NULL)
    {
        done = true;
        RemoveFromStack();
        return;
    }

    int64_t index = osData.windows.getIndexOf(window);
    if (index == -1)
    {
        done = true;
        RemoveFromStack();
        return;
    }


    if (window == osData.debugTerminalWindow)
    {
        osData.showDebugterminal = false;

        osData.windowPointerThing->UpdatePointerRect(
            window->position.x - 1, 
            window->position.y - 23, 
            window->position.x + window->size.width, 
            window->position.y + window->size.height
            );
    }
    else
    {
        osData.windows.removeAt(index);

        osData.windowPointerThing->UpdatePointerRect(
            window->position.x - 1, 
            window->position.y - 23, 
            window->position.x + window->size.width, 
            window->position.y + window->size.height
            );
            
        if (window == activeWindow)
            activeWindow = NULL;

        // if (window->title != NULL)
        //     _TryFree(window->title);

        window->Free();
        _Free(window);
        window = NULL;
    }

    done = true;
    RemoveFromStack();
}

TaskWindowClose* NewWindowCloseTask(Window* window)
{
    TaskWindowClose* task = (TaskWindowClose*)_Malloc(sizeof(TaskWindowClose), "New Window Close Task");
    *task = TaskWindowClose(window);
    return task;
}

void TaskWindowClose::Free()
{
    
}