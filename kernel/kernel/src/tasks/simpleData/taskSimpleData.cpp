#include "taskSimpleData.h"
#include "../../memory/heap.h"

TaskSimpleData::TaskSimpleData()
{
    done = false;
    type = TaskType::SIMPLE_DATA;
    data = NULL;
}


void TaskSimpleData::SetDone()
{
    done = true;
}
void TaskSimpleData::Do()
{

}

void TaskSimpleData::Free()
{
    
}

TaskSimpleData* NewSimpleDataTask()
{
    TaskSimpleData* task = (TaskSimpleData*)_Malloc(sizeof(TaskSimpleData), "New Simple Data Task");
    *task = TaskSimpleData();
    return task;
}
