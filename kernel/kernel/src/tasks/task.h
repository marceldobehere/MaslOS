#pragma once
#include <stdint.h>

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
    DOOM
};

const char* TaskTypeToString(TaskType type);

class Task
{
    protected:
        bool done;
        TaskType type;

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