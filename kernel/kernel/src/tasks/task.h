#pragma once
#include <stdint.h>
#include <stddef.h>

enum TaskType
{
    NONE,
    SLEEP,
    HANDLEENTER,
    CLOSEWINDOW,
    BEEP,
    BF,
    MAAB,
    TASK_MGR,
    TESTO,
    DEBUG_VIEWER,
    SIMPLE_DATA,
    DOOM,
    READ_FILE,
    WRITE_FILE,
};

class Task
{
    protected:
        bool done;
        TaskType type;

    public:
        const char* TaskText = "<IDK>";

        void* DoTaskFuncHelp = NULL;
        void (*DoTaskFunc)(void* bruh) = NULL;

        void* FreeTaskFuncHelp = NULL;
        void (*FreeTaskFunc)(void* bruh) = NULL;

    public:
        int32_t tempTime = 0;
        int32_t actTime = 0;
        bool GetDone();
        TaskType GetType();
        Task();



    bool operator==(Task other)
    {
        return done == other.done && type == other.type;
    }
};

void DoTask(Task* task);
void FreeTask(Task* task);