#include "task.h"
#include "../memory/heap.h"
#include "../interrupts/panic.h"


bool Task::GetDone()
{
    return done;
}

TaskType Task::GetType()
{
    return type;
}

Task::Task()
{
    done = false;
    this->type = TaskType::NONE;
    TaskText = "<IDK>";
    DoTaskFunc = NULL;
    FreeTaskFunc = NULL;
}


void DoTask(Task* task)
{
    if (task->GetDone())
        return;

    if (CheckKernelSpaceAddr(task))
        Panic("TASK HAS NOT BEEN INSTANTIATED WITH NEW!", true);

    if (task->DoTaskFunc != NULL)
        task->DoTaskFunc(task);
    else
        Panic("Task has no DoTaskFunc!");
}

void FreeTask(Task* task)
{

    if (CheckKernelSpaceAddr(task))
        Panic("TASK HAS NOT BEEN INSTANTIATED WITH NEW!", true);

    if (task->FreeTaskFunc != NULL)
    {
        task->FreeTaskFunc(task);
        _Free((void*)task);
    }
    else
        Panic("Task has no FreeTaskFunc!");
}