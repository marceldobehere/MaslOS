#include "taskEnterHandler.h"
#include "../../cStdLib/cstr.h"
#include "../../interrupts/panic.h"
#include "../../memory/heap.h"

TaskEnterHandler::TaskEnterHandler(TerminalInstance* terminal)
{
    this->terminal = terminal;
    this->done = false;
    this->type = TaskType::HANDLEENTER;
    magic = 134127;
}

void TaskEnterHandler::Do()
{
    //GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)terminal), Colors.bblue);
    if (magic != 134127)
        Panic("INVALID TASK EXECUTED", true);
    if (terminal != NULL)
        terminal->HandleEnter();
    done = true;
}

TaskEnterHandler* NewEnterTask(TerminalInstance* terminal)
{
    TaskEnterHandler* task = (TaskEnterHandler*)_Malloc(sizeof(TaskEnterHandler), "New Enter Handler Task");
    *task = TaskEnterHandler(terminal);
    return task;
}

void TaskEnterHandler::Free()
{

}