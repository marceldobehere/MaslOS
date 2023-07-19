#include "taskSimpleData.h"
#include "../../memory/heap.h"

TaskSimpleData::TaskSimpleData()
{
    done = false;
    type = TaskType::SIMPLE_DATA;
    data = NULL;

    this->TaskText = "<SIMPLE DATA TASK>";
    this->DoTaskFunc = (void(*)(void*))(void*)&Do;
    this->FreeTaskFunc = (void(*)(void*))(void*)&Free;
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
    TaskSimpleData* task = new TaskSimpleData();
    return task;
}
