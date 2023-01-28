#pragma once
#include "../task.h"
#include "../../WindowStuff/SubInstances/terminalInstance/terminalInstance.h"

class TaskEnterHandler : public Task
{
    private:
        TerminalInstance* terminal;
        uint32_t magic;

    public:
        TaskEnterHandler(TerminalInstance* terminal);
        void Do();
        void Free();
};

TaskEnterHandler* NewEnterTask(TerminalInstance* terminal);