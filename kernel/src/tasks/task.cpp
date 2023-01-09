#include "task.h"
#include "../memory/heap.h"
#include "sleep/taskSleep.h"
#include "enterHandler/taskEnterHandler.h"
#include "closeWindow/taskWindowClose.h"
#include "bfTask/bfTask.h"
#include "playBeep/playBeep.h"
#include "maab/maabTask.h"
#include "taskMgrTask/taskMgrTask.h"



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
}


const char* TaskTypeToString(TaskType type)
{
    if (type == TaskType::NONE)
        return "NONE";
    if (type == TaskType::BEEP)
        return "BEEP HANDLER";
    if (type == TaskType::BF)
        return "BF INTERPRETER";
    if (type == TaskType::CLOSEWINDOW)
        return "CLOSE WINDOW HANDLER";
    if (type == TaskType::HANDLEENTER)
        return "ENTER PRESSED HANDLER";
    if (type == TaskType::MAAB)
        return "MAAB INTERPRETER";
    if (type == TaskType::SLEEP)
        return "SLEEP / WAIT";
    if (type == TaskType::TASK_MGR)
        return "TASK MANAGER";

    return "UNKNOWN!";
}


void DoTask(Task* task)
{
    if (task->GetDone())
        return;
    
    switch (task->GetType())
    {
        case TaskType::NONE:
        {
            
            break;
        }
        case TaskType::SLEEP:
        {
            //GlobalRenderer->Println("SLEEP TASK!");
            TaskSleep* sleep = (TaskSleep*)task;
            sleep->Do();
            break;
        }
        case TaskType::BEEP:
        {
            //GlobalRenderer->Println("SLEEP TASK!");
            TaskPlayBeep* beep = (TaskPlayBeep*)task;
            beep->Do();
            break;
        }
        case TaskType::HANDLEENTER:
        {
            //GlobalRenderer->Println("ENTER TASK!");
            TaskEnterHandler* enter = (TaskEnterHandler*)task;
            enter->Do();
            break;
        }
        case TaskType::CLOSEWINDOW:
        {
            TaskWindowClose* close = (TaskWindowClose*)task;
            close->Do();
            break;
        }
        case TaskType::BF:
        {
            TaskBF* bf = (TaskBF*)task;
            bf->Do();
            break;
        }
        case TaskType::MAAB:
        {
            TaskMAAB* maab = (TaskMAAB*)task;
            maab->Do();
            break;
        }
        case TaskType::TASK_MGR:
        {
            TaskTaskManager* maab = (TaskTaskManager*)task;
            maab->Do();
            break;
        }
    }
}

void FreeTask(Task* task)
{
    switch (task->GetType())
    {
        case TaskType::NONE:
        {
            free((void*)task);
            break;
        }
        case TaskType::SLEEP:
        {
            TaskSleep* sleep = (TaskSleep*)task;
            sleep->Free();
            free((void*)sleep);
            break;
        }
        case TaskType::BEEP:
        {
            TaskPlayBeep* beep = (TaskPlayBeep*)task;
            beep->Free();
            free((void*)beep);
            break;
        }
        case TaskType::HANDLEENTER:
        {
            TaskEnterHandler* enter = (TaskEnterHandler*)task;
            enter->Free();
            free((void*)enter);
            break;
        }
        case TaskType::CLOSEWINDOW:
        {
            TaskWindowClose* close = (TaskWindowClose*)task;
            close->Free();
            free((void*)close);
            break;
        }
        case TaskType::BF:
        {
            TaskBF* bf = (TaskBF*)task;
            bf->Free();
            free((void*)bf);
            break;
        }
        case TaskType::MAAB:
        {
            TaskMAAB* maab = (TaskMAAB*)task;
            maab->Free();
            free((void*)maab);
            break;
        }
        case TaskType::TASK_MGR:
        {
            TaskTaskManager* maab = (TaskTaskManager*)task;
            maab->Free();
            free((void*)maab);
            break;
        }
    }  
}