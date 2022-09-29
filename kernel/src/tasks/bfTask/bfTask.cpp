#include "bfTask.h"
#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"
#include "../../interrupts/interrupts.h"
#include "../../cmdParsing/cstrTools.h"


TaskBF::TaskBF(const char* code)
{
    this->code = StrCopy(code);
    index = 0;
    len = StrLen(code);
    done = false;
    type = TaskType::BF;
}

void TaskBF::Do()
{
    if (index >= len || done)
    {
        done = true;
        return;
    }

    index++;


}

void TaskBF::Free()
{
    free((void*)code);
}

TaskBF* NewBFTask(const char* code)
{
    TaskBF* task = (TaskBF*)malloc(sizeof(TaskBF));
    *task = TaskBF(code);
    return task;
}