#pragma once

enum TaskType
{
    NONE,
    SLEEP,
    HANDLEENTER,
};



class Task
{
    protected:
        bool done;
        TaskType type;

    public:
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