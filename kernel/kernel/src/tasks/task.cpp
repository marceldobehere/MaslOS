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
    DoTaskFuncHelp = NULL;
    DoTaskFunc = NULL;
    FreeTaskFuncHelp = NULL;
    FreeTaskFunc = NULL;
}


void DoTask(Task* task)
{
    if (task->GetDone())
        return;

    if (task->DoTaskFunc != NULL)
        task->DoTaskFunc(task->DoTaskFuncHelp);
    else
        Panic("Task has no DoTaskFunc!");
}

void FreeTask(Task* task)
{
    if (task->FreeTaskFunc != NULL)
    {
        task->FreeTaskFunc(task->FreeTaskFuncHelp);
        _Free((void*)task);
    }
    else
        Panic("Task has no FreeTaskFunc!");
}