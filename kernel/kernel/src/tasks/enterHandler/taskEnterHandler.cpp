#include "taskEnterHandler.h"
#include "../../cStdLib/cstr.h"
#include "../../interrupts/panic.h"
#include "../../memory/heap.h"
#include "../../devices/serial/serial.h"

TaskEnterHandler::TaskEnterHandler(TerminalInstance* terminal)
{
    this->terminal = terminal;
    this->done = false;
    this->type = TaskType::HANDLEENTER;
    this->magic = 134127;

    this->TaskText = "<ENTER HANDLER TASK>";
    this->DoTaskFuncHelp = (void*)this;
    this->DoTaskFunc = (void(*)(void*))(void*)&Do;
    this->FreeTaskFuncHelp = (void*)this;
    this->FreeTaskFunc = (void(*)(void*))(void*)&Free;
}

void TaskEnterHandler::Do()
{

    if (magic != 134127)
    {
        Panic("INVALID TASK EXECUTED");
        done = true;
        return;
    }
    if (terminal != NULL)
        terminal->HandleEnter();
    done = true;
}

TaskEnterHandler* NewEnterTask(TerminalInstance* terminal)
{
    TaskEnterHandler* task = new TaskEnterHandler(terminal);
    return task;
}

void TaskEnterHandler::Free()
{

}