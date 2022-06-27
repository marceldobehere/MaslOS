#include "taskWindowClose.h"
#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"

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

    osData.windows.removeAt(index);

    osData.windowPointerThing->UpdatePointerRect(
        window->position.x - 1, 
        window->position.y - 23, 
        window->position.x + window->size.width, 
        window->position.y + window->size.height
        );

    window->Free();
    free(window);
    window = NULL;
    done = true;
    RemoveFromStack();
}

TaskWindowClose* NewWindowCloseTask(Window* window)
{
    TaskWindowClose* task = (TaskWindowClose*)malloc(sizeof(TaskWindowClose));
    *task = TaskWindowClose(window);
    return task;
}

void TaskWindowClose::Free()
{
    
}