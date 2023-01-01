#include "taskMgrTask.h"
#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"
#include "../../interrupts/interrupts.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../OSDATA/MStack/MStackM.h"
#include "../../OSDATA/osStats.h"


TaskTaskManager::TaskTaskManager(Window* window)
{
    this->window = window;
    done = false;
    type = TaskType::TASK_MGR;
    callCount = 0;

    oldTitle = window->title;
    exit = false;
    window->title = "Task Manager";
    osData.windowPointerThing->UpdateWindowBorder(window);

    ((TerminalInstance*)window->instance)->userlen = 0;
    ((TerminalInstance*)window->instance)->takeInput = false;
}

void TaskTaskManager::Do()
{
    AddToStack()
    if (exit)
    {
        window->title = oldTitle;
        done = true;
        window->renderer->Clear(Colors.black);
        osData.windowPointerThing->UpdateWindowBorder(window);
        ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->Reload();
        RemoveFromStack();
        return;
    }

    if (++callCount < callsPerFrame)
    {
        RemoveFromStack();
        return;        
    }
    totalCount++;
    callCount = 0;


    int tempTaskCount = osData.osTasks.getCount();
    {
        for (int i = 0; i < osData.windows.getCount(); i++)
        {     
            //GlobalRenderer->Print("B");
                
            Window* window = osData.windows[i];
            if (window->instance == NULL)
                continue;
            if (window->instance->instanceType != InstanceType::Terminal)
                continue;

            TerminalInstance* terminal = (TerminalInstance*)window->instance;

            tempTaskCount += terminal->tasks.getCount();
        }
    }



    window->renderer->Clear(Colors.black);
    window->renderer->Println("Total running Tasks: {}", to_string(tempTaskCount));
    window->renderer->Println("Window Count: {}", to_string(osData.windows.getCount()));
    window->renderer->Println("------------------");
    window->renderer->Println("Windows:");
    for (int i = 0; i < osData.windows.getCount(); i++)
    {
        Window* temp = osData.windows.elementAt(i);
        window->renderer->Print(" + Window {}: ", to_string(i));
        window->renderer->Println("\"{}\"", temp->title);

        if (temp->instance == NULL)
            continue;
        if (temp->instance->instanceType != InstanceType::Terminal)
            continue;

        TerminalInstance* terminal = (TerminalInstance*)temp->instance;

        for (int i2 = 0; i2 < terminal->tasks.getCount(); i2++)
        {
            window->renderer->Print("   - Task {}: ", to_string(i2));
            window->renderer->Println("\"{}\"", TaskTypeToString(terminal->tasks.elementAt(i2)->GetType()));
        }
    }
    window->renderer->Println();
    window->renderer->Println("------------------");
    window->renderer->Println("OS TASKS:");
    for (int i2 = 0; i2 < osData.osTasks.getCount(); i2++)
    {
        window->renderer->Print(" - Task {}: ", to_string(i2));
        window->renderer->Println("\"{}\"", TaskTypeToString(osData.osTasks.elementAt(i2)->GetType()));
    }
    window->renderer->Println();
    window->renderer->Println("------------------");
    window->renderer->Println("Time Stats: (in microseconds)");

    if (osStats.totalFrameTime != 0)
    {
        window->renderer->Print(" - Total Frame Time:         {}", to_string(osStats.totalFrameTime));
        window->renderer->Println("  ({} %)", to_string((100* osStats.totalFrameTime)/osStats.totalFrameTime));

        window->renderer->Print(" - Total Render Time:        {}", to_string(osStats.totalRenderTime));
        window->renderer->Println("  ({} %)", to_string((100* osStats.totalRenderTime)/osStats.totalFrameTime));

        window->renderer->Print(" - Total Window Update Time: {}", to_string(osStats.totalWindowUpdateTime));
        window->renderer->Println("  ({} %)", to_string((100* osStats.totalWindowUpdateTime)/osStats.totalFrameTime));

        window->renderer->Print(" - Total Task Time:          {}", to_string(osStats.totalTaskTime));
        window->renderer->Println("  ({} %)", to_string((100* osStats.totalTaskTime)/osStats.totalFrameTime));

        window->renderer->Print(" - Total OS Task Time:       {}", to_string(osStats.totalOsTaskTime));
        window->renderer->Println("  ({} %)", to_string((100* osStats.totalOsTaskTime)/osStats.totalFrameTime));
    }

    window->renderer->Println();
    window->renderer->Println("------------------");


    RemoveFromStack();
}

void TaskTaskManager::Free()
{
    // free((void*)code);
    // free((void*)mem);
}

TaskTaskManager* NewTaskManagerTask(Window* window)
{
    TaskTaskManager* task = (TaskTaskManager*)malloc(sizeof(TaskTaskManager));
    *task = TaskTaskManager(window);
    return task;
}