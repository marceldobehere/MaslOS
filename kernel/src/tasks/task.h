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
    TASK_MGR
};

const char* TaskTypeToString(TaskType type);

class Task
{
    protected:
        bool done;
        TaskType type;

    public:
        uint32_t tempTime;
        uint32_t cpuUsagePercent;
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