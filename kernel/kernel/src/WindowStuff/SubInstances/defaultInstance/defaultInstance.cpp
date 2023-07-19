#include "defaultInstance.h"
#include "../terminalInstance/terminalInstance.h"
#include "../newTerminalInstance/newTerminalInstance.h"
#include "../connect4Instance/connect4Instance.h"
#include "../guiInstance/guiInstance.h"
#include "../../../memory/heap.h"
#include "../../../audio/audio.h"
#include "../../../interrupts/panic.h"
#include "../../../devices/serial/serial.h"
#include "../../../cStdLib/cStdLib.h"


void DefaultInstance::DefaultFree()
{
    if (audioSource != NULL)
    {
        Audio::BasicAudioSource* source = (Audio::BasicAudioSource*)audioSource;
        source->Free();
        source = NULL;
    }

    if (CheckKernelSpaceAddr(this))
        Panic("INSTANCE HAS NOT BEEN INSTANTIATED WITH NEW!", true);

    if (FreeFunc != NULL)
    {
        FreeFunc(this);
        _Free(this);
    }
    else
    {
        Panic("> Instance has no FreeFunc!", true);
    }
}