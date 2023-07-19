#include "playBeep.h"
#include "../../devices/pit/pit.h"
#include "../../memory/heap.h"
#include "../../interrupts/interrupts.h"


TaskPlayBeep::TaskPlayBeep(uint64_t onDuration, uint64_t offDuration, uint64_t totalDuration)
{
    done = false;
    type = TaskType::BEEP;
    currentState = false;
    this->onDuration = onDuration;
    this->offDuration = offDuration;
    this->totalDuration = totalDuration;
    startTime = PIT::TimeSinceBootMicroS();
    switchTime = startTime;
    endTime = startTime + totalDuration * 1000;

    this->TaskText = "<PLAY BEEP TASK>";
    this->DoTaskFunc = (void(*)(void*))(void*)&Do;
    this->FreeTaskFunc = (void(*)(void*))(void*)&Free;
}


void TaskPlayBeep::Do()
{
    uint64_t curr = PIT::TimeSinceBootMicroS();
    if (curr >= endTime)
    {
        done = true;
        TestSetSpeakerPosition(false);
        return;
    }

    bool switchT = false;
    if (currentState)
    {
        if (curr > switchTime + onDuration)
            switchT = true;
    }
    else
    {
        if (curr > switchTime + offDuration)
            switchT = true;
    }

    if (switchT)
    {
        switchTime = curr;
        currentState = !currentState;
        TestSetSpeakerPosition(currentState);
    }
}


TaskPlayBeep* NewBeepTask(uint64_t onDuration, uint64_t offDuration, uint64_t totalDuration)
{
    TaskPlayBeep* task = new TaskPlayBeep(onDuration, offDuration, totalDuration);
    return task;
}

void TaskPlayBeep::Free()
{
    TestSetSpeakerPosition(false);
}