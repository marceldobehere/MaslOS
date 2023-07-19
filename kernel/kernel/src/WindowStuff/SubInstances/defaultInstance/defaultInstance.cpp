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

    if ((((uint64_t)FreeFuncHelp) & 0xFFFF000000000000) == 0xFFFF000000000000)
        Panic("INSTANCE HAS NOT BEEN INSTANTIATED WITH NEW!", true);

    if (FreeFunc != NULL)
    {
        Serial::Writeln("> FREE FUNC HELP: {}", ConvertHexToString((uint64_t)FreeFuncHelp));
        FreeFunc(FreeFuncHelp);
        _Free(this);
    }
    else
    {
        Panic("> Instance has no FreeFunc!", true);
    }
}