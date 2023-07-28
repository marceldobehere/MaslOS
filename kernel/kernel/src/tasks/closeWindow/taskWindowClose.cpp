#include "taskWindowClose.h"
#include "../../devices/pit/pit.h"
#include "../../memory/heap.h"
#include "../../osData/osData.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"


TaskWindowClose::TaskWindowClose(Window* window)
{
    done = false;
    type = TaskType::CLOSEWINDOW;
    this->window = window;

    this->TaskText = "<CLOSE WINDOW TASK>";
    this->DoTaskFunc = (void(*)(void*))(void*)&Do;
    this->FreeTaskFunc = (void(*)(void*))(void*)&Free;
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

    int64_t index = osData.windows.GetIndexOf(window);
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
        osData.windows.RemoveAt(index);

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
    TaskWindowClose* task = new TaskWindowClose(window);
    return task;
}

void TaskWindowClose::Free()
{
    
}