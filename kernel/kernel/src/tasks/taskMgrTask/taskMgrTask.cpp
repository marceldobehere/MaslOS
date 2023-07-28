#include "taskMgrTask.h"
#include "../../devices/pit/pit.h"
#include "../../memory/heap.h"
#include "../../interrupts/interrupts.h"
#include "../../cStdLib/cstrTools.h"
#include "../../osData/MStack/MStackM.h"
#include "../../osData/osStats.h"


TaskTaskManager::TaskTaskManager(Window* window)
{
    this->window = window;
    done = false;
    type = TaskType::TASK_MGR;
    callCount = 0;

    this->TaskText = "<TASKMGR TASK>";
    this->DoTaskFunc = (void(*)(void*))(void*)&Do;
    this->FreeTaskFunc = (void(*)(void*))(void*)&Free;

    oldTitle = window->title;
    exit = false;
    window->title = "Task Manager";
    osData.windowPointerThing->UpdateWindowBorder(window);
    nextTime = PIT::TimeSinceBootMS();

    ((TerminalInstance*)window->instance)->userlen = 0;
    ((TerminalInstance*)window->instance)->takeInput = false;
    oldIcon = window->icon;
    window->icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.TASK_MANAGER];
}

void TaskTaskManager::Do()
{
    AddToStack();
    if (exit)
    {
        window->title = oldTitle;
        done = true;
        window->icon = oldIcon;
        window->renderer->Clear(Colors.black);
        osData.windowPointerThing->UpdateWindowBorder(window);
        ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->Reload();
        RemoveFromStack();
        return;
    }

    // if (++callCount < callsPerFrame)
    // {
    //     RemoveFromStack();
    //     return;        
    // }
    // totalCount++;
    // callCount = 0;

    uint64_t cTime = PIT::TimeSinceBootMS();

    if (cTime < nextTime)
    {
        RemoveFromStack();
        return;        
    }
    nextTime = cTime + 400;


    int tempTaskCount = osData.osTasks.GetCount();
    {
        for (int i = 0; i < osData.windows.GetCount(); i++)
        {     
            //GlobalRenderer->Print("B");
                
            Window* window = osData.windows[i];
            if (window->instance == NULL)
                continue;
            if (window->instance->instanceType != InstanceType::Terminal)
                continue;

            TerminalInstance* terminal = (TerminalInstance*)window->instance;

            tempTaskCount += terminal->tasks.GetCount();
        }
    }



    window->renderer->Clear(Colors.black);
    window->renderer->Println("Total running Tasks: {}", to_string(tempTaskCount));
    window->renderer->Println("Window Count: {}", to_string(osData.windows.GetCount()));
    window->renderer->Println("------------------");
    window->renderer->Println("Windows:");
    {
        int tI = 0;
        for (int i = 0; i < osData.windows.GetCount(); i++)
        {
            Window* temp = osData.windows.ElementAt(i);
            window->renderer->Print(" + Window {}: ", to_string(i));
            window->renderer->Println("\"{}\"", temp->title);

            if (temp->instance == NULL)
                continue;
            if (temp->instance->instanceType != InstanceType::Terminal)
                continue;

            TerminalInstance* terminal = (TerminalInstance*)temp->instance;

            for (int i2 = 0; i2 < terminal->tasks.GetCount(); i2++)
            {
                window->renderer->Print("   - Task {}: ", to_string(tI));
                window->renderer->Println("\"{}\"", terminal->tasks.ElementAt(i2)->TaskText);
                tI++;
            }
        }
    }
    window->renderer->Println();
    window->renderer->Println("------------------");
    window->renderer->Println("OS TASKS:");
    for (int i2 = 0; i2 < osData.osTasks.GetCount(); i2++)
    {
        window->renderer->Print(" - OS Task {}: ", to_string(i2));
        window->renderer->Println("\"{}\"", osData.osTasks.ElementAt(i2)->TaskText);
    }
    window->renderer->Println();
    window->renderer->Println("------------------");
    window->renderer->Println("Time Stats: (in microseconds)");

    OSStats osStatsTemp = osStats;

    AddToStack();
    if (osStatsTemp.totalFrameTime != 0)
    {
        window->renderer->Print(" + Total Frame Time: {}", to_string(osStatsTemp.totalFrameTime));
        window->renderer->Println("  ({} %)", to_string((100* osStatsTemp.totalFrameTime)/osStatsTemp.totalFrameTime));

        window->renderer->Print("   - Total Render Time:        {}", to_string(osStatsTemp.totalRenderTime));
        window->renderer->Println("  ({} %)", to_string((100* osStatsTemp.totalRenderTime)/osStatsTemp.totalFrameTime));

        window->renderer->Print("   - Total Window Update Time: {}", to_string(osStatsTemp.totalWindowUpdateTime));
        window->renderer->Println("  ({} %)", to_string((100* osStatsTemp.totalWindowUpdateTime)/osStatsTemp.totalFrameTime));

        window->renderer->Print("   + Total Idle/Task Time:     {}", to_string(osStatsTemp.totalIdleTime));
        window->renderer->Println("  ({} %)", to_string((100* osStatsTemp.totalIdleTime)/osStatsTemp.totalFrameTime));

        AddToStack();
        if (osStatsTemp.totalIdleTime != 0)
        {
            window->renderer->Print("     + Total Task Time:          {}", to_string(osStatsTemp.totalTaskTime));
            window->renderer->Println("  ({} %)", to_string((100* osStatsTemp.totalTaskTime)/osStatsTemp.totalIdleTime));

            AddToStack();
            if (osStatsTemp.totalTaskTime != 0)
            {
                int tI = 0;
                for (int i = 0; i < osData.windows.GetCount(); i++)
                {
                    Window* temp = osData.windows.ElementAt(i);

                    if (temp->instance == NULL)
                        continue;
                    if (temp->instance->instanceType != InstanceType::Terminal)
                        continue;

                    TerminalInstance* terminal = (TerminalInstance*)temp->instance;

                    for (int i2 = 0; i2 < terminal->tasks.GetCount(); i2++)
                    {
                        Task* bruhTask = terminal->tasks.ElementAt(i2);
                        window->renderer->Print("       - Task {} Time: ", to_string(tI));
                        window->renderer->Print("{}", to_string(bruhTask->actTime));
                        window->renderer->Println("  ({} %)", to_string((100* bruhTask->actTime)/osStatsTemp.totalTaskTime));
                        tI++;
                    }
                }
            }
            RemoveFromStack();

            window->renderer->Print("     - Total OS Task Time:       {}", to_string(osStatsTemp.totalOsTaskTime));
            window->renderer->Println("  ({} %)", to_string((100* osStatsTemp.totalOsTaskTime)/osStatsTemp.totalIdleTime));
            // if (osStatsTemp.totalOsTaskTime != 0)
            // {
            //         for (int i2 = 0; i2 < osData.osTasks.getCount(); i2++)
            //         {
            //             Task* bruhTask = osData.osTasks.elementAt(i2);
            //             window->renderer->Print("       - OS Task {} Time: ", to_string(i2));
            //             window->renderer->Print("{}", to_string(bruhTask->actTime));
            //             window->renderer->Println("  ({} %)", to_string((100* bruhTask->actTime)/osStatsTemp.totalOsTaskTime));
            //         }
            // }
        }
        RemoveFromStack();


    }
    RemoveFromStack();

    window->renderer->Println();
    window->renderer->Println("------------------");


    RemoveFromStack();
}

void TaskTaskManager::Free()
{
    if (window->title != oldTitle)
    {
        window->title = oldTitle;
        done = true;
        window->icon = oldIcon;
        window->renderer->Clear(Colors.black);
        osData.windowPointerThing->UpdateWindowBorder(window);
        ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->Reload();
        RemoveFromStack();
        return;
    }
}

TaskTaskManager* NewTaskManagerTask(Window* window)
{
    TaskTaskManager* task = new TaskTaskManager(window);
    return task;
}