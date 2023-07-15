#include "cmdParser.h"
#include "../Rendering/BasicRenderer.h"
#include "../cStdLib/cstrTools.h"
#include "../Rendering/Cols.h"
#include <stdint.h>
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "../OSDATA/osdata.h"
#include "../kernelStuff/other_IO/pit/pit.h"
#include "../tasks/sleep/taskSleep.h"
#include "../tasks/playBeep/playBeep.h"
#include "../tasks/test/testTask.h"
#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/file/fileDiskInterface.h"
#include "../kernelStuff/Disk_Stuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../kernelStuff/Disk_Stuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"
#include "../WindowStuff/SubInstances/connect4Instance/connect4Instance.h"
#include "../tasks/taskMgrTask/taskMgrTask.h"
#include "../tasks/closeWindow/taskWindowClose.h"
#include "../tasks/bfTask/bfTask.h"
#include "../tasks/debugViewTask/debugViewTask.h"
#include "../fsStuff/fsStuff.h"
#include "../tasks/maab/maabTask.h"

void Println(Window* window)
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Println();
}

void Print(Window* window, const char* msg)
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Print(msg);
}
void Print(Window* window, const char* chrs, const char* var)
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Print(chrs, var);
}
void Print(Window* window, char chr)
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Print(chr);
}
void Print(Window* window, const char* chrs, uint32_t col)
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Print(chrs, col);
}
void Print(Window* window, const char* chrs, const char* var, uint32_t col)
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Print(chrs, var, col);
}
void Print(Window* window, const char* chrs, dispVar vars[])
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Print(chrs, vars);
}
void Print(Window* window, const char *chrs, dispVar vars[], uint32_t col)
{
    NewTerminalInstance* temp = (NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance);
    temp->Print(chrs, vars, col);
}



/*
void Println(Window* window)
{
    window->renderer->Println();
}

void Print(Window* window, const char* msg)
{
    window->renderer->Print(msg);
}
void Print(Window* window, const char* chrs, const char* var)
{
    window->renderer->Print(chrs, var);
}
void Print(Window* window, char chr)
{
    window->renderer->Print(chr);
}
void Print(Window* window, const char* chrs, uint32_t col)
{
    window->renderer->Print(chrs, col);
}
void Print(Window* window, const char* chrs, const char* var, uint32_t col)
{
    window->renderer->Print(chrs, var, col);
}
void Print(Window* window, const char* chrs, dispVar vars[])
{
    window->renderer->Print(chrs, vars);
}
void Print(Window* window, const char *chrs, dispVar vars[], uint32_t col)
{
    uint64_t tempcol = window->renderer->color;
    window->renderer->color = col;

    Print(window, chrs, vars);

    window->renderer->color = tempcol;
}
*/


void Println(Window* window, const char* msg)
{
    Print(window, msg);
    Println(window);
}
void Println(Window* window, const char* chrs, const char* var)
{
    Print(window, chrs, var);
    Println(window);
}
void Println(Window* window, const char* chrs, uint32_t col)
{
    Print(window, chrs, col);
    Println(window);
}
void Println(Window* window, const char* chrs, const char* var, uint32_t col)
{
    Print(window, chrs, var, col);
    Println(window);
}

void Println(Window* window, const char* chrs, dispVar vars[])
{
    Print(window, chrs, vars);
    Println(window);
}

void Println(Window* window, const char *chrs, dispVar vars[], uint32_t col)
{
    Print(window, chrs, vars, col);
    Println(window);
}


//void Print(Window* window, const char* chrs);
//void Print(Window* window, const char* chrs, bool allowEscape);
//void Println(Window* window, const char* chrs);


// void Print(Window* window, const char* chrs, dispVar vars[]); 
// void Print(Window* window, const char *chrs, dispVar vars[], uint32_t col);
// void PrintArg(Window* window, dispVar var);





void LogError(const char* msg, Window* window)
{
    AddToStack();
    Println(window, msg, Colors.bred);
    RemoveFromStack();
}

void LogError(const char* msg, const char* var, Window* window)
{
    AddToStack();
    Println(window, msg, var, Colors.bred);
    RemoveFromStack();
}

void LogInvalidArgumentCount(int expected, int found, Window* window)
{
    AddToStack();
    Print(window, "Invalid Argument count. Expected {} but got ", to_string((uint64_t)expected), Colors.bred);
    Println(window, "{} instead.", to_string((uint64_t)found), Colors.bred);
    RemoveFromStack();
}


void EditPartitionSetting(PartitionInterface::PartitionInfo* part, const char* param, const char* val, OSUser* user, Window* window)
{
    if (StrEquals(param, "hidden"))
    {
        if (StrEquals(val, "true") || StrEquals(val, "TRUE"))
        {
            part->hidden = true;
            Println(window, "Parameter changed!");
        }
        else if (StrEquals(val, "false") || StrEquals(val, "FALSE"))
        {
            part->hidden = false;
            Println(window, "Parameter changed!");
        }
        else
            LogError("Invalid Value for Paramter!", window);
    }
    else if (StrEquals(param, "name"))
    {
        _Free((void*)part->name);
        part->name = StrCopy(val);
        part->nameLen = StrLen(val);
        Println(window, "Parameter changed!");
    }
    else if (StrEquals(param, "description"))
    {
        _Free((void*)part->description);
        part->description = StrCopy(val);
        part->descriptionLen = StrLen(val);
        Println(window, "Parameter changed!");
    }
    else if (StrEquals(param, "drive name"))
    {
        _Free((void*)part->driveName);
        part->driveName = StrCopy(val);
        part->driveNameLen = StrLen(val);
        Println(window, "Parameter changed!");
    }
    else
        LogError("Invalid Parameter!", window);
    
}

#include "../sysApps/explorer/explorer.h"
#include "../sysApps/tetris/tetris.h"
#include "../sysApps/notepad/notepad.h"
#include "../sysApps/imgTest/imgTest.h"
#include "../sysApps/musicPlayer/musicPlayer.h"
#include "../tasks/doomTask/taskDoom.h"

#include "../audio/audioDevStuff.h"
//#include "../kernelStuff/other_IO/sb16/sb16.h"
#include "../kernelStuff/other_IO/serial/serial.h"


BuiltinCommand BuiltinCommandFromStr(char* i)
{
  if (StrEquals(i, "help")) return Command_Help;
  else if (StrEquals(i, "cls")) return Command_Clear;
  else if (StrEquals(i, "clear")) return Command_Clear;
  else if (StrEquals(i, "benchmark reset")) return Command_BenchMarkReset;
  else if (StrEquals(i, "bench res")) return Command_BenchMarkReset;
  else if (StrEquals(i, "malloc")) return Command_Malloc;
  else if (StrEquals(i, "explorer")) return Command_Explorer;
  else if (StrEquals(i, "notepad")) return Command_NotePad;
  else if (StrEquals(i, "img")) return Command_Image;
  else if (StrEquals(i, "music")) return Command_MusicPlayer;
  else if (StrEquals(i, "doom")) return Command_Doom;
  else if (StrEquals(i, "music test")) return Command_MusicTest;
  else if (StrEquals(i, "ac97 reset")) return Command_AC97Reset;
  else if (StrEquals(i, "tetris")) return Command_Tetris;
  else if (StrEquals(i, "heap check")) return Command_HeapCheck;
  else if (StrEquals(i, "shutdown")) return Command_ShutDown;
  else if (StrEquals(i, "exit")) return Command_Exit;
  else if (StrEquals(i, "connect 4")) return Command_ConnectFour;
  else if (StrEquals(i, "connect four")) return Command_ConnectFour;
  else if (StrEquals(i, "taskmgr")) return Command_TaskManager;
  else if (StrEquals(i, "dbg")) return Command_DebugViewer;
  else if (StrEquals(i, "debug viewer")) return Command_DebugViewer;
  else if (StrEquals(i, "crash")) return Command_Crash;
  else if (StrEquals(i, "crash 2")) return Command_Crash2;
  else if (StrEquals(i, "crash 3")) return Command_Crash3;
  else if (StrEquals(i, "crash 4")) return Command_Crash4;
  else if (StrEquals(i, "renderloop")) return Command_RenderLoop;
  else if (StrEquals(i, "resdefspk")) return Command_ResetDefaultSpeaker;
  else return Command_Invalid;
}

void HelpCommand(Window* window)
{
    const char* helpMessage =
        "Help Commands: (More Details are in the MaslOS Wiki)\n"
        " - help                    get this message\n"
        " - exit                    exit terminal\n"
        " - clear                   clears the terminal screen\n"
        " - benchmark reset         resets the bench mark\n"
        " - malloc                  mallocs memory 20KB\n"
        " - ac97 reset              resets the ac97 card\n"
        " - music test              test music\n"
        " - shutdown                turn off operating system\n"
        " - explorer                open explorer\n"
        " - notepad                 open notepad\n"
        " - img                     open image viewer\n"
        " - doom                    open doom game\n"
        " - tetris                  open tetris game\n"
        " - connect [four | 4]      open connect four game\n"
        " - taskmgr                 open task manager\n"
        " - dbg | debug viewer      open debug viewer\n"
        " - heapCheck               Performs a Heap Check\n"
        " - crash                   Causes a trivial kernel panic\n"
        " - crash 2                 Causes a trivial but blocking kernel panic\n"
        " - crash 3                 Causes a kernel panic\n"
        " - crash 4                 Causes a memory corruption and crashes\n"
        " - resdefspk               Resets the default speaker\n"
        ;
    Print(window, helpMessage);
}

void ParseCommand(char* input, char* oldInput, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;
    BuiltinCommand command = BuiltinCommandFromStr(input);

    switch (command) {
        case Command_Help: {
            HelpCommand(window);
            RemoveFromStack();
            return;
        }
        case Command_Clear: {
            ((TerminalInstance*)window->instance)->Cls();
            RemoveFromStack();
            return;
        }
        case Command_BenchMarkReset: {
            MStackData::BenchmarkStackPointerSave = 0;
            RemoveFromStack();
            return;
        }
        case Command_Malloc: {
            _Malloc(0x5000, "Test malloc command");
            RemoveFromStack();
            return;
        }
        case Command_Explorer: {
            new SysApps::Explorer();
            RemoveFromStack();
            return;
        }
        case Command_NotePad: {
            new SysApps::Notepad();
            RemoveFromStack();
            return;
        }
        case Command_Image: {
            new SysApps::ImageViewer("");
            RemoveFromStack();
            return;
        }
        case Command_MusicPlayer: {
            new SysApps::MusicPlayer("");
            RemoveFromStack();
            return;
        }
        case Command_Doom: {
            terminal->tasks.add(NewDoomTask(window));
            RemoveFromStack();
            return;
        }
        case Command_AC97Reset: 
        {
            Println(window, "> Resetting AC97 thingy");
            AudioDeviceStuff::resetTest();
            if (osData.ac97Driver != NULL)
            {
                osData.ac97Driver->DoQuickCheck();
            }
            RemoveFromStack();
            return;
        }
        case Command_ResetDefaultSpeaker:
        {
            Println(window, "> Resetting default speaker to the PC Speaker");
            osData.defaultAudioOutputDevice = osData.pcSpeakerDev;
            RemoveFromStack();
            return;
        }
        case Command_MusicTest: 
        {
            AddToStack();
            DefaultInstance* instance = window->instance;
            if (instance != NULL && osData.defaultAudioOutputDevice != NULL)
            {
                int sampleRate = 44100;
                int sampleCount = sampleRate * 10;
                if (instance->audioSource == NULL)
                {
                    Println(window, "> Creating Audiosource");
                    instance->audioSource = (void*)new Audio::BasicAudioSource(
                        Audio::AudioBuffer::Create16BitMonoBuffer(sampleRate, sampleCount)
                    );
                    Println(window, "> Linking Audiosource to Default Output Device \"{}\"", osData.defaultAudioOutputDevice->deviceName);
                    ((Audio::BasicAudioSource*)instance->audioSource)->ConnectTo(osData.defaultAudioOutputDevice->destination);
                }
                Audio::BasicAudioSource* src = (Audio::BasicAudioSource*)instance->audioSource;

                if (!src->readyToSend)
                {
                    Println(window, "> Filling Data");
                    uint16_t* arr = (uint16_t*)src->buffer->data;
                    int dif = 40;
                    for (int i = 0; i < dif; i++)
                    {
                        Audio::FillArray(arr, (i * sampleCount)/dif, sampleCount/dif, ((1000*(i+1)) / dif), sampleRate);
                    }
                    src->buffer->sampleCount = sampleCount;
                    src->samplesSent = 0;
                    src->readyToSend = true;
                    Println(window, "> Ready To send");
                }
                else
                {
                    Print(window, "> Still sending Data. ({}", to_string(src->samplesSent));
                    Println(window, " of {} samples)", to_string(src->buffer->sampleCount));
                }
            }

            RemoveFromStack();
            return;
        }
        case Command_Tetris: {
            new SysApps::Tetris();
            RemoveFromStack();
            return;
        }
        case Command_HeapCheck: {
            HeapCheck(true);
            osData.windowPointerThing->Clear(true);
            osData.windowPointerThing->RenderWindows();
            RemoveFromStack();
            return;
        }
        case Command_ShutDown: {
            Println(window, "Shutting down...");
            osData.exit = true;
            RemoveFromStack();
            return;
        }
        case Command_Exit: {
            osData.osTasks.add(NewWindowCloseTask(window));
            RemoveFromStack();
            return;
        }
        case Command_ConnectFour: {
            {
                Window* con4Window = (Window*)_Malloc(sizeof(Window), "Connect 4 Window");
                Connect4Instance* connect4 = (Connect4Instance*)_Malloc(sizeof(Connect4Instance), "Connect 4 Instance");
                *connect4 = Connect4Instance(con4Window);
                *(con4Window) = Window((DefaultInstance*)connect4, Size(200, 200), Position(10, 40), "Connect 4", true, true, true);
                osData.windows.add(con4Window);

                connect4->Init();

                osData.windowsToGetActive.add(con4Window);

                RemoveFromStack();
                return;
            }
            RemoveFromStack();
            return;
        }
        case Command_TaskManager: {
            terminal->tasks.add(NewTaskManagerTask(window));
            RemoveFromStack();
            return;
        }
        case Command_DebugViewer: {
            terminal->tasks.add(NewDebugViewerTask(window));
            RemoveFromStack();
            return;
        }
        case Command_Crash: {
            Println(window, "Crashing...");

            // int Ax = 0;
            // int Ay = 0;

            // int Az = Ax/Ay;

            asm("int $0x01");
            RemoveFromStack();
            return;
        }
        case Command_Crash2: {
            Println(window, "Crashing 2...");

            // int Ax = 0;
            // int Ay = 0;

            // int Az = Ax/Ay;
            osData.NO_INTERRUPTS = true;
            asm("int $0x0D");
            RemoveFromStack();
            return;
        }
        case Command_Crash3: {
            Println(window, "Crashing 3...");

            Panic("DEBUG KERNEL PANIC", true);
            RemoveFromStack();
            return;
        }
        case Command_RenderLoop: {
            Println(window, "Re-Entering Render Loop...");
            DoSafe();
            RemoveFromStack();
            return;
        }
        case Command_Crash4: {
            Println(window, "Crashing 4...");

            void* testo = _Malloc(10);
            char* bruhus = (char*) testo;
            for (int i = -100; i < 100; i++)
                bruhus[i] = ((i * 7 + 1234) % 255);
        
            // Should crash
            //_Free(testo);
            _Malloc(10);

            RemoveFromStack();
            return;
        }
    }

    StringArrData* data = SplitLine(oldInput);

    if (StrEquals(data->data[0], "login") && terminal->mode == commandMode::enterPassword)
    {
        terminal->mode = commandMode::mode_none;
        StringArrData* data2 = SplitLine(input);
        if (data->len == 2)
        {
            if (data2->len == 1)
                login(data->data[1], data2->data[0], user, window);
            else
                LogError("Password can only be one Argument long!", window);
        }
        
        _Free(data);
        _Free(data2);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "set") && terminal->mode == commandMode::enterPassword)
    {
        terminal->mode = commandMode::mode_none;
        StringArrData* data2 = SplitLine(input);
        if (data->len == 2 || data->len == 3)
        {
            if (StrEquals(data->data[1], "password"))
            {
                if (data2->len == 1)
                {
                    (*user)->password = StrCopy(data2->data[0]);
                    //Println("Password is now \"{}\".", (*user)->password, Colors.yellow);
                }
                else
                    LogError("Password can only be one Argument long!", window);
            }
        }
        
        _Free(data);
        _Free(data2);
        RemoveFromStack();
        return;
    }

    _Free(data);
    data = SplitLine(input);

    if (data->len == 0)
    {
        _Free(data);
        RemoveFromStack();
        return;
    }

    // Println("Parts:");
    // for (int i = 0; i < data->len; i++)
    //     Println(" - \"{}\"", data->data[i], Colors.bgreen);

    if (StrEquals(data->data[0], "echo"))
    {
        if (data->len == 2)
            Println(window, data->data[1]);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "io"))
    {
        // io [outb, outw, outl] port value (4)
        // io [inb, inw, inl] port (3)
        // io pci [outb, outw, outl] addr field value (6)
        // io pci [inb, inw, inl] addr field (5)
        
        if (data->len == 4) // io [outb, outw, outl] port value (4)
        {
            if (StrEquals(data->data[1], "outb"))
            {
                //Print(window, "OUTB: {}, ", to_string((uint16_t)to_int(data->data[2])), Colors.yellow);
                //Println(window, "{}", to_string((uint8_t)to_int(data->data[3])), Colors.yellow);
                outb((uint16_t)to_int(data->data[2]), (uint8_t)to_int(data->data[3]));
            }
            else if (StrEquals(data->data[1], "outw"))
                outw((uint16_t)to_int(data->data[2]), (uint16_t)to_int(data->data[3]));
            else if (StrEquals(data->data[1], "outl"))
                outl((uint16_t)to_int(data->data[2]), (uint32_t)to_int(data->data[3]));
            else
                LogError("Invalid IO Command!", window);
        }
        else if (data->len == 3) // io [inb, inw, inl] port (3)
        {
            if (StrEquals(data->data[1], "inb"))
                Println(window, "INB: {}", to_string(inb((uint16_t)to_int(data->data[2]))), Colors.yellow);
            else if (StrEquals(data->data[1], "inw"))
                Println(window, "INW: {}", to_string(inw((uint16_t)to_int(data->data[2]))), Colors.yellow);
            else if (StrEquals(data->data[1], "inl"))
                Println(window, "INL: {}", to_string((uint64_t)inl((uint16_t)to_int(data->data[2]))), Colors.yellow);
            else
                LogError("Invalid IO Command!", window);
        }
        else if (data->len == 6) // io pci [outb, outw, outl] addr field value (6)
        {
            if (StrEquals(data->data[2], "outb"))
                PCI::write_byte((uint8_t)to_int(data->data[4]), (uint8_t)to_int(data->data[4]), (uint8_t)to_int(data->data[5]));
            else if (StrEquals(data->data[2], "outw"))
                PCI::write_word((uint8_t)to_int(data->data[4]), (uint8_t)to_int(data->data[4]), (uint16_t)to_int(data->data[5]));
            else if (StrEquals(data->data[2], "outl"))
                PCI::write_dword((uint8_t)to_int(data->data[4]), (uint8_t)to_int(data->data[4]), (uint32_t)to_int(data->data[5]));
            else
                LogError("Invalid IO Command!", window);
        }
        else if (data->len == 5) // io pci [inb, inw, inl] addr field (5)
        {
            if (StrEquals(data->data[2], "inb"))
                Println(window, "INB: {}", to_string(PCI::read_byte((uint8_t)to_int(data->data[4]), (uint8_t)to_int(data->data[4]))), Colors.yellow);
            else if (StrEquals(data->data[2], "inw"))
                Println(window, "INW: {}", to_string(PCI::read_word((uint8_t)to_int(data->data[4]), (uint8_t)to_int(data->data[4]))), Colors.yellow);
            else if (StrEquals(data->data[2], "inl"))
                Println(window, "INL: {}", to_string((uint64_t)PCI::read_dword((uint8_t)to_int(data->data[4]), (uint8_t)to_int(data->data[4]))), Colors.yellow);
            else
                LogError("Invalid IO Command!", window);
        }
        else
            LogError("Invalid IO Command!", window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "serial"))
    {
        if (data->len == 2)
        {
            if (StrEquals(data->data[1], "init"))
                Println(window, "Init: {}", to_string(Serial::Init()), Colors.yellow);
            else if (StrEquals(data->data[1], "read"))
                Println(window, "Read: {}", to_string(Serial::Read()), Colors.yellow);
            else
                LogError("Invalid Serial Command!", window);
        }
        else if (data->len == 3)
        {
            if (StrEquals(data->data[1], "write"))
                Serial::Write(data->data[2]);
            else
                LogError("Invalid Serial Command!", window);
        }
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "bf"))
    {
        if (data->len == 2)
            terminal->tasks.add(NewBFTask(data->data[1], window));
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "img"))
    {
        if (data->len == 2)
            new SysApps::ImageViewer(data->data[1]);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "music"))
    {
        if (data->len == 2)
            new SysApps::MusicPlayer(data->data[1]);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "run") || StrEquals(data->data[0], "opn") || StrEquals(data->data[0], "open"))
    {
        if (data->len == 2)
            FS_STUFF::OpenFile(data->data[1]);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "dbg") || StrEquals(data->data[0], "debug") || StrEquals(data->data[0], "hex"))
    {
        if (data->len == 2)
        {
            //  FS_STUFF::OpenFile(data->data[1]);
            char* buf = NULL;
            int len = 0;
            if (FS_STUFF::ReadFileFromFullPath(data->data[1], &buf, &len))
                terminal->tasks.add(NewDebugViewerTask(window, buf, len));
            else
                LogError("File not found!", window);
        }
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }
    

    if (StrEquals(data->data[0], "sleep"))
    {
        if (data->len == 2)
        {
            int64_t time = to_int(data->data[1]);
            if (time > 0)
            {
                Println(window, "Sleeping for {} ms...", to_string(time), Colors.yellow);
                //PIT::Sleep((uint64_t)time);
                {

                    if (window->instance != NULL)
                    {
                        if (window->instance->instanceType == InstanceType::Terminal)
                        {
                            TerminalInstance* terminal = (TerminalInstance*)window->instance;

                            terminal->tasks.add(NewSleepTask(time));
                        }
                    }

                }
            }
            else
                LogError("You cannot sleep less than 0ms!", window);
        }
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "set"))
    {
        if (data->len == 3)
            SetCmd(data->data[1], data->data[2], user, window);
        else if (data->len == 2 && StrEquals(data->data[1], "password"))
            SetCmd(data->data[1], "", user, window);
        else
            LogInvalidArgumentCount(2, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "get"))
    {
        if (data->len == 2)
            GetCmd(data->data[1], *user, window);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "maab"))
    {
        if (data->len == 2)
        {
            //Println(window, "Reading File \"{}\"...", data->data[1], Colors.yellow);

            // {
            //     char* t = FS_STUFF::GetDriveNameFromFullPath(data->data[1]);
            //     if (t != NULL)
            //     {
            //         Println(window, "Drive: \"{}\"", t, Colors.yellow);
            //         free((void*)t);
            //     }
            // }

            // {
            //     char* t = FS_STUFF::GetFilePathFromFullPath(data->data[1]);
            //     if (t != NULL)
            //     {
            //         Println(window, "Path: \"{}\"", t, Colors.yellow);
            //         free((void*)t);
            //     }
            // }

            FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(data->data[1]);
            if (fsInterface != NULL)
            {
                char* relPath = FS_STUFF::GetFilePathFromFullPath(data->data[1]);
                if (relPath != NULL)
                {
                    if (fsInterface->FileExists(relPath))
                    {
                        uint8_t* buf = NULL;
                        fsInterface->ReadFile(relPath, (void**)(&buf));
                        if (buf != NULL)
                        {   
                            //Println(window, "> File exists!");
                            int fSize = fsInterface->GetFileInfo(relPath)->sizeInBytes;

                            terminal->tasks.add(NewMAABTask(fSize, buf, window, terminal));

                            _Free((void*)buf);
                        }
                        else
                            LogError("Reading File failed!", window);
                    }
                    else
                        LogError("File does not exist!", window);

                    _Free(relPath);
                }
                else
                    LogError("Path is invalid!", window);
            }
            else
                LogError("Path is invalid!", window);

        }
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "testo"))
    {
        if (data->len == 2)
        {
            //Println(window, "Reading File \"{}\"...", data->data[1], Colors.yellow);

            // {
            //     char* t = FS_STUFF::GetDriveNameFromFullPath(data->data[1]);
            //     if (t != NULL)
            //     {
            //         Println(window, "Drive: \"{}\"", t, Colors.yellow);
            //         free((void*)t);
            //     }
            // }

            // {
            //     char* t = FS_STUFF::GetFilePathFromFullPath(data->data[1]);
            //     if (t != NULL)
            //     {
            //         Println(window, "Path: \"{}\"", t, Colors.yellow);
            //         free((void*)t);
            //     }
            // }

            FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(data->data[1]);
            if (fsInterface != NULL)
            {
                char* relPath = FS_STUFF::GetFilePathFromFullPath(data->data[1]);
                if (relPath != NULL)
                {
                    if (fsInterface->FileExists(relPath))
                    {
                        uint8_t* buf = NULL;
                        fsInterface->ReadFile(relPath, (void**)(&buf));
                        if (buf != NULL)
                        {   
                            //Println(window, "> File exists!");
                            int fSize = fsInterface->GetFileInfo(relPath)->sizeInBytes;

                            terminal->tasks.add(NewTestTask(buf, fSize, window));
                            
                            _Free((void*)buf);
                        }
                        else
                            LogError("Reading File failed!", window);
                    }
                    else
                        LogError("File does not exist!", window);

                    _Free(relPath);
                }
                else
                    LogError("Path is invalid!", window);
            }
            else
                LogError("Path is invalid!", window);

        }
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "substr"))
    {
        if (data->len == 3)
        {
            const char* dataa = StrSubstr(data->data[1], to_int(data->data[2]));
            Println(window, "Result: {}", dataa);
            _Free((void*)dataa);
        }
        else
            LogInvalidArgumentCount(2, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }


    if (StrEquals(data->data[0], "login"))
    {
        if (data->len == 2)
            login(data->data[1], user, window);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        _Free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "beep"))
    {        
        if (data->len == 4)
        {
            int onDur = to_int(data->data[1]);
            int offDur = to_int(data->data[2]);
            int totDur = to_int(data->data[3]);
            int size = to_int(data->data[3]);
            terminal->tasks.add(NewBeepTask(onDur, offDur, totDur));
            Println(window, "Playing beep...");

            _Free(data);
            RemoveFromStack();
            return;
        }
        else
            LogInvalidArgumentCount(3, data->len-1, window);
    }
    
    if (StrEquals(data->data[0], "disk"))
    {
        // if (data->len == 3)
        // {
        //     if (StrEquals(data->data[1], "create"))
        //     {
        //         int size = to_int(data->data[2]);
        //         osData.diskInterfaces.add(new DiskInterface::RamDiskInterface(size));
        //         Println("Ram Disk with {} sectors created!", to_string(size), (*user)->colData.defaultTextColor);
        //     }
        //     else
        //         LogError("No valid arguments passed!", window);
        // }
        // else 
        int BLEH = 0;
        if (data->len > 1)
            BLEH = to_int(data->data[1]);
        // Println(window, "TEST VAL: {}", to_string(BLEH), Colors.yellow);
        if (data->len > 1 && !StrEquals(data->data[1], "create") && (BLEH < 0 || BLEH >= osData.diskInterfaces.getCount()))
            LogError("Invalid Disk Number selected!", window);
        else if (data->len == 4)
        {
            if (StrEquals(data->data[1], "create"))
            {
                if (StrEquals(data->data[2], "ram disk"))
                {
                    int size = to_int(data->data[3]);
                    osData.diskInterfaces.add(new DiskInterface::RamDiskInterface(size));
                    Println(window, "Ram Disk with {} sectors created!", to_string(size), (*user)->colData.defaultTextColor);
                }
                else
                    LogError("No valid arguments passed!", window);
            }
            else if (StrEquals(data->data[2], "read"))
            {
                int num = to_int(data->data[1]);
                int size = to_int(data->data[3]);

                uint8_t* buffer = (uint8_t*)_Malloc((((size - 1) / 512) + 1) * 512, "Malloc for disk read buffer");
                _memset((void*)buffer, 0, (((size - 1) / 512) + 1) * 512);

                if (osData.diskInterfaces[num]->Read(0, ((size - 1) / 512) + 1, (void*)buffer))
                {
                    Println(window, "Raw Data:");
                    for (int i = 0; i < size; i++)
                        Print(window, buffer[i]);
                    Println(window);
                }
                else
                {
                    LogError("Disk read failed!", window);
                    Println(window, "Raw Debug Data:");
                    for (int i = 0; i < size; i++)
                        Print(window, buffer[i]);
                    Println(window);
                }


                _Free(buffer);
                
            }
            else if (StrEquals(data->data[2], "get"))
            {
                int num = to_int(data->data[1]);

                if (StrEquals(data->data[3], "max sector count"))
                {
                    Println(window, "Max Sector Count: {}", to_string((uint64_t)osData.diskInterfaces[num]->GetMaxSectorCount()), (*user)->colData.defaultTextColor);
                }
                else
                {
                    LogError("No valid arguments for get function passed!", window);
                }
            }
            else if (StrEquals(data->data[2], "partition"))
            {
                int num = to_int(data->data[1]);

                if (StrEquals(data->data[3], "create"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)new PartitionInterface::MRAPSPartitionInterface(diskInterface);

                    if (diskInterface->partitionInterface != NULL)
                        Println(window, "Partition Interface creation Success!", (*user)->colData.defaultTextColor);
                    else
                        LogError("Partition Interface creation failed!", window);

                }
                else if (StrEquals(data->data[3], "init"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        const char* res = partInterface->InitAndSavePartitionTable();
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            Println(window, "Partition Init Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Init failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else if (StrEquals(data->data[3], "save"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        const char* res = partInterface->SavePartitionTable();
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            Println(window, "Partition Save Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Save failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else if (StrEquals(data->data[3], "load"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        const char* res = partInterface->LoadPartitionTable();
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            Println(window, "Partition Load Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Load failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else if (StrEquals(data->data[3], "list"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        uint64_t partCount = partInterface->partitionList.getCount();
                        Println(window, "Partition Count: {}", to_string(partCount), Colors.yellow);
                        Println(window, "Partition Data:", Colors.yellow);
                        for (int i = 0; i < partCount; i++)
                        {   
                            PartitionInterface::PartitionInfo* info = partInterface->partitionList[i];
                            Println(window, " + Partition {}:", to_string(i), Colors.orange);
                            Println(window, "    - Name:        \"{}\"", info->name, Colors.yellow);
                            Println(window, "    - Description: \"{}\"", info->description, Colors.yellow);
                            Println(window, "    - Drive Name:  \"{}\"", info->driveName, Colors.yellow);
                            Println(window, "    - Size:         {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                            //Println("    - Location:     0x{}", ConvertHexToString(info->locationInBytes), Colors.yellow);
                            {
                                Print(window, "    - Location:     0x{} ", ConvertHexToString(info->locationInBytes), Colors.yellow);
                                Print(window, "({} - ", to_string(info->locationInBytes), Colors.yellow);
                                Print(window, "{})", to_string(info->locationInBytes + info->sizeInBytes - 1), Colors.yellow);
                                Println(window);
                            }
                            Println(window, "    - Owner:        0x{}", ConvertHexToString((uint64_t)info->owner), Colors.yellow);
                            Println(window, "    - Type:         {}", PartitionInterface::PartitionTypeStr[(uint8_t)info->type], Colors.yellow);
                            Println(window, "    - FS Type:      {}", FilesystemInterface::FilesystemInterfaceTypeStr[(uint8_t)info->fsType], Colors.yellow);
                            Println(window, "    - FS Interface: 0x{}", ConvertHexToString((uint64_t)info->fsInterface), Colors.yellow);
                            Println(window, "    - Hidden:       {}", info->hidden ? "True" : "False", Colors.yellow);
                        }
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else
                {
                    LogError("No valid arguments for partition function passed!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 5)
        {
            if (StrEquals(data->data[2], "partition"))
            {
                int diskNum = to_int(data->data[1]);
                int partNum = to_int(data->data[4]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                {
                    if (StrEquals(data->data[3], "delete"))
                    {
                        const char* res = partInterface->DeletePartition(partInterface->partitionList[partNum]);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                            Println(window, "Partition Deletion Success!", (*user)->colData.defaultTextColor);
                        else
                            LogError("Partition Deletion failed! Error: \"{}\"", res, window);

                    }
                    else if (StrEquals(data->data[3], "wipe"))
                    {
                        const char* res = partInterface->WipePartitionContents(partInterface->partitionList[partNum]);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                            Println(window, "Partition Wipe Success!", (*user)->colData.defaultTextColor);
                        else
                            LogError("Partition Wipe failed! Error: \"{}\"", res, window);
                    }
                    else if (StrEquals(data->data[3], "show"))
                    {
                        PartitionInterface::PartitionInfo* info = partInterface->partitionList[partNum];
                        Println(window, " + Partition {}:", to_string(partNum), Colors.orange);
                        Println(window, "    - Name:        \"{}\"", info->name, Colors.yellow);
                        Println(window, "    - Description: \"{}\"", info->description, Colors.yellow);
                        Println(window, "    - Drive Name:  \"{}\"", info->driveName, Colors.yellow);
                        Println(window, "    - Size:         {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                        //Println("    - Location:     0x{}", ConvertHexToString(info->locationInBytes), Colors.yellow);
                        {
                            Print(window, "    - Location:     0x{} ", ConvertHexToString(info->locationInBytes), Colors.yellow);
                            Print(window, "({} - ", to_string(info->locationInBytes), Colors.yellow);
                            Print(window, "{})", to_string(info->locationInBytes + info->sizeInBytes - 1), Colors.yellow);
                            Println(window);
                        }
                        Println(window, "    - Owner:        0x{}", ConvertHexToString((uint64_t)info->owner), Colors.yellow);
                        Println(window, "    - Type:         {}", PartitionInterface::PartitionTypeStr[(uint8_t)info->type], Colors.yellow);
                        Println(window, "    - FS Type:      {}", FilesystemInterface::FilesystemInterfaceTypeStr[(uint8_t)info->fsType], Colors.yellow);
                        Println(window, "    - FS Interface: 0x{}", ConvertHexToString((uint64_t)info->fsInterface), Colors.yellow);
                        Println(window, "    - Hidden:       {}", info->hidden ? "True" : "False", Colors.yellow);
                    }
                    else
                        LogError("No valid arguments passed!", window);
                }
            }
            else if (StrEquals(data->data[2], "read"))
            {
                int num = to_int(data->data[1]);
                int start = to_int(data->data[3]);
                int count = to_int(data->data[4]);

                // Disk 0 read 0 100
                uint8_t* buffer = (uint8_t*)_Malloc(count, "Malloc for disk read buffer");

                if (osData.diskInterfaces[num]->ReadBytes(start, count, (void*)buffer))
                {
                    Println(window, "Raw Data:");
                    for (int i = 0; i < count; i++)
                        Print(window, buffer[i]);
                    Println(window);
                }
                else
                {
                    LogError("Disk read failed!", window);
                }


                _Free(buffer);
                
            }
            else if (StrEquals(data->data[2], "write"))
            {   
                int num = to_int(data->data[1]);
                int size = to_int(data->data[4]);
                
                uint8_t val = data->data[3][0];
                if (StrEquals(data->data[3], "NULL"))
                    val = 0;

                // disk 0 write A 100
                uint8_t* buffer = (uint8_t*)_Malloc((((size - 1) / 512) + 1) * 512, "Malloc for disk read buffer");
                _memset(buffer, 0, (((size - 1) / 512) + 1) * 512);
                _memset(buffer, val, size);

                if (osData.diskInterfaces[num]->Write(0, ((size - 1) / 512) + 1, (void*)buffer))
                {
                    Println(window, "Disk write successful!");
                }
                else
                {
                    LogError("Disk write failed!", window);
                }


                _Free(buffer);
            }
            else if (StrEquals(data->data[2], "write_cached"))
            {   
                int num = to_int(data->data[1]);
                int size = to_int(data->data[4]);
                
                uint8_t val = data->data[3][0];
                if (StrEquals(data->data[3], "NULL"))
                    val = 0;

                // disk 0 write A 100
                uint8_t* buffer = (uint8_t*)_Malloc((((size - 1) / 512) + 1) * 512, "Malloc for disk read buffer");
                _memset(buffer, 0, (((size - 1) / 512) + 1) * 512);

                if (!osData.diskInterfaces[num]->Read(0, ((size - 1) / 512) + 1, (void*)buffer))
                    LogError("Disk read failed!", window);
                {
                    _memset(buffer, val, size);

                    if (osData.diskInterfaces[num]->Write(0, ((size - 1) / 512) + 1, (void*)buffer))
                    {
                        Println(window, "Disk write successful!");
                    }
                    else
                    {
                        LogError("Disk write failed!", window);
                    }
                }
                _Free(buffer);
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 6)
        {
            if (StrEquals(data->data[1], "create"))
            {
                if (StrEquals(data->data[2], "file disk"))
                {
                    int64_t diskID = to_int(data->data[3]);
                    int64_t partID = to_int(data->data[4]);
                    const char* filename = data->data[5];

                    if (diskID < 0 || diskID >= osData.diskInterfaces.getCount())
                        LogError("Invalid Disk selected!", window);
                    else
                    {
                        DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskID];
                        PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                        if (partInterface == NULL)
                            LogError("Drive has no Partition Manager!", window);
                        else if (partID < 0 || partID >= partInterface->partitionList.getCount())
                            LogError("Invalid Partition selected!", window);
                        else
                        {
                            FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partID]->fsInterface;
                            if (fsInterface == NULL)
                                LogError("Partition has no Filesystem!", window);
                            else
                            {
                                if (!fsInterface->FileExists(filename))
                                    LogError("The File was not found!", window);
                                else
                                {
                                    osData.diskInterfaces.add(new DiskInterface::FileDiskInterface(filename, fsInterface));
                                    Println(window, "File Disk from file \"{}\" created!", filename, (*user)->colData.defaultTextColor);
                                }
                            }
                        }
                    }




                    //int size = to_int(data->data[3]);
                    //osData.diskInterfaces.add(new DiskInterface::RamDiskInterface(size));
                    //Println("Ram Disk with {} sectors created!", to_string(size), (*user)->colData.defaultTextColor);
                }
                else
                    LogError("No valid arguments passed!", window);
            }
            else if (StrEquals(data->data[2], "partition"))
            {
                int diskNum = to_int(data->data[1]);
                if (StrEquals(data->data[4], "fs"))
                {
                    int partNum = to_int(data->data[3]);
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface == NULL)
                        LogError("Drive has no Partition Manager!", window);
                    else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                        LogError("Invalid Partition selected!", window);
                    else
                    {
                        if (StrEquals(data->data[5], "create"))
                        {
                            FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)new FilesystemInterface::MrafsFilesystemInterface(partInterface, partInterface->partitionList[partNum]);

                            if (fsInterface != NULL)
                                Println(window, "Filesystem Interface Creation Success!");
                            else
                                LogError("Filesystem Interface Creation failed!", window);
                        }
                        else
                        {
                            FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                            if (fsInterface != NULL)
                            {
                                if (StrEquals(data->data[5], "init"))
                                {
                                    const char* res = fsInterface->InitAndSaveFSTable();
                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                        Println(window, "Filesystem Interface Init Success!");
                                    else
                                        LogError("Filesystem Interface Init failed! Error: {}", res, window);
                                }
                                else if (StrEquals(data->data[5], "save"))
                                {
                                    const char* res = fsInterface->SaveFSTable();
                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                        Println(window, "Filesystem Interface Save Success!");
                                    else
                                        LogError("Filesystem Interface Save failed! Error: {}", res, window);
                                }
                                else if (StrEquals(data->data[5], "load"))
                                {
                                    const char* res = fsInterface->LoadFSTable();

                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                        Println(window, "Filesystem Interface Load Success!");
                                    else
                                        LogError("Filesystem Interface Load failed! Error: {}", res, window);
                                }
                                else if (StrEquals(data->data[5], "list"))
                                {
                                    AddToStack();
                                    const char* res = fsInterface->LoadFSTable();
                                    RemoveFromStack();
                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    {
                                        //Println("Filesystem Interface Load Success!");
                                        
                                        if (fsInterface->InterfaceType == FilesystemInterface::FilesystemInterfaceType::Mrafs)
                                        {
                                            FilesystemInterface::MrafsFilesystemInterface* mrafsInterface = (FilesystemInterface::MrafsFilesystemInterface*)fsInterface;

                                            AddToStack();
                                            {
                                                uint64_t partCount = mrafsInterface->fsPartitionList.getCount();
                                                Println(window, "Partition Count: {}", to_string(partCount), Colors.yellow);
                                                Println(window, "Partition Data:", Colors.yellow);
                                                for (int i = 0; i < partCount; i++)
                                                {   
                                                    FilesystemInterface::MrafsFilesystemInterface::FSPartitionInfo* info = (FilesystemInterface::MrafsFilesystemInterface::FSPartitionInfo*)mrafsInterface->fsPartitionList[i];
                                                    Println(window, " + Partition {}:", to_string(i), Colors.orange);
                                                    Println(window, "    - Free:        {}", info->free ? "true" : "false", Colors.yellow);
                                                    Println(window, "    - Location:    0x{}", ConvertHexToString((uint64_t)info->locationInBytes), Colors.yellow);
                                                    Println(window, "    - Size:        {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                                                }
                                            }
                                            RemoveFromStack();
                                            
                                            AddToStack();
                                            {
                                                uint64_t partCount = mrafsInterface->fsFileList.getCount();
                                                Println(window, "File Count: {}", to_string(partCount), Colors.yellow);
                                                Println(window, "File Data:", Colors.yellow);
                                                for (int i = 0; i < partCount; i++)
                                                {   
                                                    FilesystemInterface::FileInfo* info = mrafsInterface->fsFileList[i];
                                                    Println(window, " + File {}:", to_string(i), Colors.orange);
                                                    Println(window, "    - Path:        \"{}\"", info->baseInfo->path, Colors.yellow);
                                                    Println(window, "    - Hidden:      {}", info->baseInfo->hidden ? "true" : "false", Colors.yellow);
                                                    Println(window, "    - System File: {}", info->baseInfo->systemFile ? "true" : "false", Colors.yellow);
                                                    Println(window, "    - Readonly:    {}", info->baseInfo->writeProtected ? "true" : "false", Colors.yellow);

                                                    Println(window, "    - Location:    0x{}", ConvertHexToString((uint64_t)info->locationInBytes), Colors.yellow);
                                                    Println(window, "    - Size:        {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                                                }
                                            }
                                            RemoveFromStack();
                                            
                                            AddToStack();
                                            {
                                                uint64_t partCount = mrafsInterface->fsFolderList.getCount();
                                                Println(window, "Folder Count: {}", to_string(partCount), Colors.yellow);
                                                Println(window, "Folder Data:", Colors.yellow);
                                                for (int i = 0; i < partCount; i++)
                                                {   
                                                    FilesystemInterface::FolderInfo* info = mrafsInterface->fsFolderList[i];
                                                    Println(window, " + Folder {}:", to_string(i), Colors.orange);
                                                    Println(window, "    - Path:        \"{}\"", info->baseInfo->path, Colors.yellow);
                                                    Println(window, "    - Hidden:      {}", info->baseInfo->hidden ? "true" : "false", Colors.yellow);
                                                    Println(window, "    - System File: {}", info->baseInfo->systemFile ? "true" : "false", Colors.yellow);
                                                    Println(window, "    - Readonly:    {}", info->baseInfo->writeProtected ? "true" : "false", Colors.yellow);
                                                }
                                            }
                                            RemoveFromStack();
                                        }
                                        else
                                        {
                                            LogError("Filesystem Interface is not MRAFS! Error: {}", res, window);
                                        }
                                        

                                    }
                                    else
                                        LogError("Filesystem Interface Load failed! Error: {}", res, window);
                                }
                                else
                                    LogError("No valid arguments passed!", window);
                            }
                            else
                                LogError("Filesystem Interface not found!", window);
                        }
                    }
                }
                else
                {
                    int partNum = to_int(data->data[4]);
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface == NULL)
                        LogError("Drive has no Partition Manager!", window);
                    else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                        LogError("Invalid Partition selected!", window);
                    else
                    {
                        if (StrEquals(data->data[3], "resize"))
                        {
                            uint64_t newSize = to_int(data->data[5]);
                            const char* res = partInterface->ResizePartition(partInterface->partitionList[partNum], newSize);
                            if (res == PartitionInterface::CommandResult.SUCCESS)
                                Println(window, "Partition Resize Success!", (*user)->colData.defaultTextColor);
                            else
                                LogError("Partition Resize failed! Error: \"{}\"", res, window);

                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                }
            }
            else if (StrEquals(data->data[2], "write"))
            {   
                int num = to_int(data->data[1]);
                int start = to_int(data->data[4]);
                int count = to_int(data->data[5]);
                
                uint8_t val = data->data[3][0];
                if (StrEquals(data->data[3], "NULL"))
                    val = 0;

                // disk 0 write A 10 52
                uint8_t* buffer = (uint8_t*)_Malloc(count, "Malloc for disk read buffer");
                _memset(buffer, val, count);

                if (osData.diskInterfaces[num]->WriteBytes(start, count, (void*)buffer))
                {
                    Println(window, "Disk write successful!");
                }
                else
                {
                    LogError("Disk write failed!", window);
                }


                _Free(buffer);
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 7)
        {
            int diskNum = to_int(data->data[1]);
            if (StrEquals(data->data[4], "fs"))
            {
                int partNum = to_int(data->data[3]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                { 
                    FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                    if (fsInterface != NULL)
                    {
                        if (StrEquals(data->data[5], "read"))
                        {
                            uint8_t* buffer = NULL;
                            uint64_t byteCount = fsInterface->ReadFile(data->data[6], (void**)&buffer);
                            Println(window, "Size: {} Bytes", to_string(byteCount), (*user)->colData.defaultTextColor);
                            Println(window, "Raw Data:");
                            for (uint64_t i = 0; i < byteCount; i++)
                                Print(window, buffer[i]);
                            Println(window);
                            if (byteCount != 0)
                                _Free(buffer);
                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                    else
                        LogError("Filesystem Interface not found!", window);
                }
            }
            else if (StrEquals(data->data[2], "partition"))
            {
                int partNum = to_int(data->data[4]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                {
                    if (StrEquals(data->data[3], "create"))
                    {
                        int newSize = to_int(data->data[5]);
                        PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[partNum];
                        const char* res = partInterface->CreatePartition(partInfo, newSize);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            _Free((void*)partInfo->name);
                            partInfo->name = StrCopy(data->data[6]);
                            partInfo->nameLen = StrLen(data->data[6]);

                            Println(window, "Partition Creation Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Creation failed! Error: \"{}\"", res, window);
                    }
                    else if (StrEquals(data->data[3], "edit"))
                    {
                        const char* param = data->data[5];
                        const char* val = data->data[6];
                        // EDIT PARAM VAL
                        EditPartitionSetting(partInterface->partitionList[partNum], param, val, *user, window);
                    }
                    else if (StrEquals(data->data[3], "read"))
                    {
                        uint64_t addr = to_int(data->data[5]);
                        uint64_t count = to_int(data->data[6]);
                        uint8_t* buffer = (uint8_t*)_Malloc(count, "Buffer For Partition Read Command");

                        const char* res = partInterface->ReadPartition(partInterface->partitionList[partNum], addr, count, buffer);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            Println(window, "Raw Data:");
                            for (int i = 0; i < count; i++)
                                Print(window, buffer[i]);
                            Println(window);
                        }
                        else
                            LogError("Partition Read failed! Error: \"{}\"", res, window);
                        
                        _Free(buffer);
                    }
                    else if (StrEquals(data->data[3], "write"))
                    {
                        uint64_t addr = to_int(data->data[6]);
                        const char* buffer = data->data[5];

                        const char* res = partInterface->WritePartition(partInterface->partitionList[partNum], addr, StrLen(buffer), (void*)buffer);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                            Println(window, "Partition Write Success!");
                        else
                            LogError("Partition Write failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("No valid arguments passed!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 8)
        {
            int diskNum = to_int(data->data[1]);
            if (StrEquals(data->data[4], "fs"))
            {
                int partNum = to_int(data->data[3]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                { 
                    FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                    if (fsInterface != NULL)
                    {
                        if (StrEquals(data->data[5], "create"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->CreateFile(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "File Creation Success!");
                                else
                                    LogError("File Creation failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                const char* res = fsInterface->CreateFolder(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "Folder Creation Success!");
                                else
                                    LogError("Folder Creation failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "delete"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->DeleteFile(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "File Deletion Success!");
                                else
                                    LogError("File Deletion failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                const char* res = fsInterface->DeleteFolder(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "Folder Deletion Success!");
                                else
                                    LogError("Folder Deletion failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "read"))
                        {
                            uint64_t byteCount = to_int(data->data[7]);
                            uint8_t* buffer = (uint8_t*)_Malloc(byteCount, "Malloc for File read");
                            const char* res = fsInterface->ReadFile(data->data[6], byteCount, buffer);
                            if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                            {
                                Println(window, "Raw Data:");
                                for (uint64_t i = 0; i < byteCount; i++)
                                    Print(window, buffer[i]);
                                Println(window);
                            }
                            else
                                LogError("File Read failed! Error: \"{}\"", res, window);
                            _Free(buffer);
                        }
                        else if (StrEquals(data->data[5], "write"))
                        {
                            uint64_t byteCount = StrLen(data->data[7]);
                            const char* res = fsInterface->WriteFile(data->data[6], byteCount, data->data[7]);
                            if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                Println(window, "File Write Success!");
                            else
                                LogError("File Write failed! Error: \"{}\"", res, window);
                        }
                        else if (StrEquals(data->data[5], "exists"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                if (fsInterface->FileExists(data->data[7]))
                                    Println(window, "File exists!");
                                else
                                    Println(window, "File doesn't exists!");
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                if (fsInterface->FolderExists(data->data[7]))
                                    Println(window, "Folder exists!");
                                else
                                    Println(window, "Folder doesn't exists!");
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "get"))
                        {
                            if (StrEquals(data->data[6], "files"))
                            {
                                uint64_t fCount = 0;
                                const char** files = fsInterface->GetFiles(data->data[7], &fCount);
                                Println(window, "Files: (Count: {})", to_string(fCount), Colors.yellow);
                                for (int i = 0; i < fCount; i++)
                                {
                                    Println(window, " - \"{}\"", files[i], Colors.yellow);
                                    _Free((void*)files[i]);
                                }
                                if (fCount != 0)
                                    _Free(files);
                            }
                            else if (StrEquals(data->data[6], "folders"))
                            {
                                uint64_t fCount = 0;
                                const char** folders = fsInterface->GetFolders(data->data[7], &fCount);
                                Println(window, "Folders: (Count: {})", to_string(fCount), Colors.yellow);
                                for (int i = 0; i < fCount; i++)
                                {
                                    Println(window, " - \"{}\"", folders[i], Colors.yellow);
                                    _Free((void*)folders[i]);
                                }
                                if (fCount != 0)
                                    _Free(folders);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                    else
                        LogError("Filesystem Interface not found!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 9)
        {
            int diskNum = to_int(data->data[1]);
            if (StrEquals(data->data[4], "fs"))
            {
                int partNum = to_int(data->data[3]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                { 
                    FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                    if (fsInterface != NULL)
                    {
                        if (StrEquals(data->data[5], "create"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                int64_t size = to_int(data->data[8]);
                                const char* res = fsInterface->CreateFile(data->data[7], size);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "File with a size of {} bytes got created successfully!", data->data[8], (*user)->colData.defaultTextColor);
                                else
                                    LogError("File Creation failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "copy"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->CopyFile(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "File Copying Success!");
                                else
                                    LogError("File Copying failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                const char* res = fsInterface->CopyFolder(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "Folder Copying Success!");
                                else
                                    LogError("Folder Copying failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "rename"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->RenameFile(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "File Renaming Success!");
                                else
                                    LogError("File Renaming failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                //osData.debugTerminalWindow->renderer->Cls();
                                //osData.debugTerminalWindow->Log("---Rename Folder Debug---");
                                const char* res = fsInterface->RenameFolder(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    Println(window, "Folder Renaming Success!");
                                else
                                    LogError("Folder Renaming failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                    else
                        LogError("Filesystem Interface not found!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else
        {
            LogError("No valid arguments passed!", window);
        }
        
        _Free(data);
        RemoveFromStack();
        return;
    }


    LogError("Unknown command \"{}\"!", data->data[0], window);
    _Free(data);
    RemoveFromStack();
    return;
}


void login(const char* name, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;

    OSUser* usr = getUser(name);
    if (usr == 0)
    {
        LogError("User \"{}\" was not found!", name, window);
        RemoveFromStack();
        return;
    }

    if (StrEquals(usr->password, ""))
        *user = usr;
    else
    {
        Println(window, "Please enter the password down below:");
        terminal->mode = commandMode::enterPassword;
    }
    RemoveFromStack();
}

void login(const char* name, const char* pass, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;

    terminal->mode = commandMode::mode_none;

    OSUser* usr = getUser(name);
    if (usr == 0)
    {
        LogError("User \"{}\" was not found!", name, window);
        RemoveFromStack();
        return;
    }

    if (StrEquals(usr->password, pass))
        *user = usr;
    else
        LogError("Password is incorrect!", name, window); 
    RemoveFromStack();
}

void SetCmd(const char* name, const char* val, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;

    if (StrEquals(name, "user color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            (*user)->colData.userColor = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "str"))
    {
        uint64_t addr = ConvertStringToLongHex(val);
        //Println(window, "VAL: {}",ConvertHexToString(addr), Colors.yellow);
        
        Print(window, "STR: \"", Colors.yellow);
        // for (int i = 0; i < 50; i++)
        // {
        //     char blehus[2];
        //     blehus[0] = ((const char*)addr)[i];
        //     blehus[1] = 0;
        //     Print(window, blehus, Colors.yellow);
        // }
        Print(window, ((const char*)addr), Colors.yellow);
        Println(window, "\"", Colors.yellow);
    }
    else if (StrEquals(name, "str2"))
    {
        uint64_t addr = ConvertStringToLongHex(val);
        //Println(window, "VAL: {}",ConvertHexToString(addr), Colors.yellow);
        
        Print(window, "STR: \"", Colors.yellow);
        for (int i = 0; i < 50; i++)
        {
            char blehus[2];
            blehus[0] = ((const char*)addr)[i];
            blehus[1] = 0;
            Print(window, blehus, Colors.yellow);
        }
        //Print(window, ((const char*)addr), Colors.yellow);
        Println(window, "\"", Colors.yellow);
    }
    else if (StrEquals(name, "bg task run"))
    {
        osData.bgTaskRun = StrEquals(val, "true");
    }
    else if (StrEquals(name, "benchmark"))
    {
        MStackData::BenchmarkEnabled = StrEquals(val, "true");
    }
    else if (StrEquals(name, "benchmark val"))
    {
        MStackData::BenchmarkStackPointerSave = to_int(val);
    }
    else if (StrEquals(name, "default color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
        {
            ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->foregroundColor = data.col;
            ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->backgroundColor = Colors.black;
        }
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "default color fg"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
        {
            ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->foregroundColor = data.col;
        }
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "default color bg"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
        {
            ((NewTerminalInstance*)(((TerminalInstance*)window->instance)->newTermInstance))->backgroundColor = data.col;
        }
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "fps"))
    {
        int fpsVal = to_int(val);
        if (fpsVal > 0 && fpsVal < 100000)
        {
            osData.wantedFps = fpsVal;
            Println(window, "FPS set to {}.", val);
        }
        else
            LogError("Wanted FPS of {} is out of range!", val, window);
    }
    else if (StrEquals(name, "mdiv"))
    {
        int mDiv = to_int(val);
        if (mDiv > 10 && mDiv < 300)
        {
            PIT::MusicDiv = mDiv;
            Println(window, "Music Div set to {}.", val);
        }
        else
            LogError("Wanted Music Div of {} is out of range!", val, window);
    }
    else if (StrEquals(name, "mvol"))
    {
        int mVol = to_int(val);
        if (mVol >= 0 && mVol <= 100)
        {
            if (osData.defaultAudioOutputDevice != NULL)
            {
                osData.defaultAudioOutputDevice->destination->buffer->volume = mVol;
                Println(window, "Music Volume set to {}.", val);
            }
            else
                LogError("No Audio Output Device is set!", window);
        }
        else
            LogError("Wanted Music Volume of {} is out of range!", val, window);
    }
    else if (StrEquals(name, "username"))
    {
        (*user)->userName = StrCopy(val);
    }
    else if (StrEquals(name, "amfv") || StrEquals(name, "active memory flag value"))
    {
        activeMemFlagVal= to_int(val);
    }
    else if (StrEquals(name, "mnfcc") || StrEquals(name, "max non fatal crash count"))
    {
        osData.maxNonFatalCrashCount = to_int(val);
    }
    else if (StrEquals(name, "debug terminal"))
    {
        if (StrEquals(val, "on") || StrEquals(val, "shown"))
        {
            osData.showDebugterminal = true;
            osData.debugTerminalWindow->hidden = false;
            osData.windowPointerThing->UpdatePointerRect(
                osData.debugTerminalWindow->position.x - 1, 
                osData.debugTerminalWindow->position.y - 23, 
                osData.debugTerminalWindow->position.x + osData.debugTerminalWindow->size.width, 
                osData.debugTerminalWindow->position.y + osData.debugTerminalWindow->size.height
                );
            
        }
        else if (StrEquals(val, "off") || StrEquals(val, "hidden"))
        {
            osData.showDebugterminal = false;
            osData.debugTerminalWindow->hidden = true;
            osData.windowPointerThing->UpdatePointerRect(
                osData.debugTerminalWindow->position.x - 1, 
                osData.debugTerminalWindow->position.y - 23, 
                osData.debugTerminalWindow->position.x + osData.debugTerminalWindow->size.width, 
                osData.debugTerminalWindow->position.y + osData.debugTerminalWindow->size.height
                );
            
        }
        else
            LogError("value has to be \"on\", \"shown\", \"off\" or \"hidden\"!", window);
    }
    else if (StrEquals(name, "background image"))
    {
        osData.drawBackground = StrEquals(val, "true") || StrEquals(val, "on");
        if (!osData.drawBackground)
        {
            if (!(StrEquals(val, "false") || StrEquals(val, "off")))
                LogError("Value \"{}\" has to be \"true\", \"false\", \"on\" or \"off\"!", val, window); 
        }
    }
    else if (StrEquals(name, "mouse offset"))
    {
        mouseCycleSkip = to_int(val);
    }
    else if (StrEquals(name, "mouse sensitivity") || StrEquals(name, "mouse sens"))
    {
        osData.mouseSensitivity = to_int(val) / 100.0;
    }
    else if (StrEquals(name, "test interlace"))
    {
        int interlace = to_int(val);
        if (interlace < 0)
            LogError("Interlace cannot be less than 0", window);
        else if (interlace > 8)
            LogError("Interlace cannot be higher than 8", window);
        else
        {
            int inter = 1;
            for (int i = 0; i < interlace - 1; i++)
                inter *= 2;
            if (interlace == 0)
                inter = 0;
            
            WindowManager::testInterlace = inter;
        }
    }
    else if (StrEquals(name, "mouse image"))
    {
        currentMouseImageName = StrCopy(val);
        if (kernelFiles::ZIP::GetFileFromFileName(mouseZIP, currentMouseImageName) == NULL)
            LogError("Mouse Image File doesn't exist! Setting to default...", window);
    }
    //mouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));
    else if (StrEquals(name, "password"))
    {
        terminal->mode = commandMode::enterPassword;
        Println(window, "Please enter the new password down below:");
    }
    else if (StrEquals(name, "mouse color front"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            mouseColFront = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "mouse color back"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            mouseColBack = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "window resolution") || StrEquals(name, "window size"))
    {
        StringArrData* split = SplitLine(val);
        if (split->len == 2)
        {
            int x = to_int(split->data[0]);
            int y = to_int(split->data[1]);

            if (x >= 10 && y >= 10)
            {
                int x1 = window->position.x - 1;
                int y1 = window->position.y - 23;
                int sx1 = window->size.width + 3;
                int sy1 = window->size.height + 25;

                window->Resize(Size(x, y));

                int x2 = window->position.x - 1;
                int y2 = window->position.y - 23;
                int sx2 = window->size.width + 3;
                int sy2 = window->size.height + 25;

                {
                    osData.windowPointerThing->UpdatePointerRect(x1, y1, x2 + sx2, y2 + sy2);

                    osData.windowPointerThing->UpdatePointerRect(x2, y2 + sy2, x1 + sx1, y1 + sy1);

                    osData.windowPointerThing->UpdatePointerRect(x2 + sx2, y1, x1 + sx1, y2 + sy2);

                    osData.windowPointerThing->UpdatePointerRect(x1, y2, x2, y1 + sy1);

                    osData.windowPointerThing->RenderWindow(window);
                }

            }
            else
                LogError("Invalid size given!", window);
        }
        else
            LogInvalidArgumentCount(2, split->len, window);

        _Free((void*)split);
    }
    else
    {
        LogError("Parameter \"{}\" does not exist.", name, window);
    }
    RemoveFromStack();
}



void GetCmd(const char* name, OSUser* user, Window* window)
{
    AddToStack();
    if (StrEquals(name, "free ram"))
    {
        Println(window, "Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    }
    else if (StrEquals(name, "benchmark"))
    {
        bool tempBench = MStackData::BenchmarkEnabled;
        MStackData::BenchmarkEnabled = false;
        int bMax = MStackData::BenchmarkStackPointerSave;
        if (bMax > 1500)
            bMax = 1500;
        Println(window, "Benchmark:");
        for (int i = 0; i < bMax; i++)
        {
            for (int x = 0; x < MStackData::BenchmarkStackArrSave[i].layer; x++)
                Print(window, "--", Colors.yellow);

            bool skipNext = false;
            if (i  + 1 < bMax && !MStackData::BenchmarkStackArrSave[i].close && MStackData::BenchmarkStackArrSave[i + 1].close && 
                StrEquals(MStackData::BenchmarkStackArrSave[i].name, MStackData::BenchmarkStackArrSave[i + 1].name))
            {
                Print(window, "o {}: ", to_string(i), Colors.yellow);
                skipNext = true;
            }
            else
            {
                if (MStackData::BenchmarkStackArrSave[i].close)
                    Print(window, "< {}: ", to_string(i), Colors.yellow);
                else
                    Print(window, "> {}: ", to_string(i), Colors.yellow);
            }
            
            Print(window, "L: {}", to_string(MStackData::BenchmarkStackArrSave[i].layer), Colors.bgreen);
            Print(window, ", Name: {}", MStackData::BenchmarkStackArrSave[i].name, Colors.yellow);
            Print(window, ", File {}", MStackData::BenchmarkStackArrSave[i].filename, Colors.orange);
            Print(window, ", Line {}", to_string(MStackData::BenchmarkStackArrSave[i].line), Colors.orange);
            Print(window, ", Time {}", to_string(MStackData::BenchmarkStackArrSave[i].time), Colors.orange);

            Println(window);

            if (skipNext)
                i++;
        }


        MStackData::BenchmarkEnabled = tempBench;
    }
    else if (StrEquals(name, "malloc cache"))
    {
        //MallocCache16BytesSize
        Println(window, "Malloc cache:", "", Colors.yellow);
        uint64_t _time;
        for (int i = 0; i < MallocCacheCount; i++)
        {
            Print(window, "> Malloc {}: ", to_string(i), Colors.yellow);
            Print(window, "Free {}, ", to_string(MallocCache16BytesFree[i]), Colors.yellow);
            _time = PIT::TimeSinceBootMS();
            Print(window, "Time {}, ", to_string(_time - MallocCache16BytesTime[i]), Colors.yellow);
            Print(window, "Addr {}", ConvertHexToString((uint64_t)MallocCache16BytesAddr[i]), Colors.yellow);

            Println(window);
        }
    }
    else if (StrEquals(name, "drives"))
    {
        Println(window, "Available Drives:", Colors.bgreen);
        for (int i = 0; i < osData.diskInterfaces.getCount(); i++)
        {
            DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[i];
            if (diskInterface->partitionInterface == NULL)
                continue;
            PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
            for (int i2 = 0; i2 < partInterface->partitionList.getCount(); i2++)
            {
                PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[i2];
                if (!partInfo->hidden && partInfo->type == PartitionInterface::PartitionType::Normal)
                {
                    Println(window, " - Drive: \"{}\"", partInfo->driveName, Colors.bgreen);
                }
            }
        }
    }
    else if (StrEquals(name, "free pages"))
    {
        Println(window, "Free Page Count: {} pages.", to_string(GlobalAllocator->GetFreePageCount()), Colors.bgreen);
    }
    else if (StrEquals(name, "bg task run"))
    {
        if (osData.bgTaskRun)
            Println(window, "Background Task Running is enabled!", Colors.bgreen);
        else
            Println(window, "Background Task Running is disabled!", Colors.bred);
    }
    else if (StrEquals(name, "window resolution") || StrEquals(name, "window size"))
    {
        dispVar vars[] = {dispVar(window->size.width), dispVar(window->size.height)};
        Println(window, "Window Resolution: {0}x{1}.", vars);
    }
    else if (StrEquals(name, "screen resolution"))
    {
        dispVar vars[] = {dispVar((uint64_t)GlobalRenderer->framebuffer->Width), dispVar((uint64_t)GlobalRenderer->framebuffer->Height)};
        Println(window, "Screen Resolution: {0}x{1}.", vars);
    }
    else if (StrEquals(name, "mnfcc") || StrEquals(name, "max non fatal crash count"))
    {
        Println(window, "Max non-fatal crash count: {} crashes..", to_string(osData.maxNonFatalCrashCount), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "stack trace"))
    {

        //PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer, window->renderer, user->colData.defaultTextColor);
        int64_t size  = MStackData::stackPointer;
        MStack* stack = MStackData::stackArr;
        uint32_t col = Colors.yellow;

        #if RECORD_STACK_TRACE  
        int count = 0;
        for (int i = 0; i < size; i++)
        {
            int index = (size - i) - 1;
            if (stack[index].line != -1)
                count++;
        }
        Println(window, "STACK TRACE: ({} Elements)\n", to_string(count), col);
        if (size > 50)
            size = 50;
        for (int i = 0; i < size; i++)
        {
            int index = (size - i) - 1;
            if (stack[index].line != -1)
            {
                Println(window, "> At \"{}\"", stack[index].name, col);
                Println(window, "  > in file \"{}\"", stack[index].filename, col);
                Println(window, "  > At line: {}", to_string(stack[index].line), col);
                Println(window);
            }
        }
        Println(window);

        #else

            Println(window, "M-Stack traces are disabled!");
            Println(window);

        #endif

    }
    else if (StrEquals(name, "TSB S"))
    {
        Println(window, "Time since boot: {} s.", to_string(PIT::TimeSinceBootS()), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "TSB MS"))
    {
        Println(window, "Time since boot: {} ms.", to_string(PIT::TimeSinceBootMS()), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "disk count"))
    {
        Println(window, "Disk Count: {}", to_string(osData.diskInterfaces.getCount()), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "heap stats"))
    {
        Println(window, "Heap Statistics:", Colors.yellow);
        {
            uint64_t totalSegCount = 0, freeSegCount = 0, usedSegCount = 0, totalSegSpace = 0, freeSegSpace = 0, usedSegSpace = 0; 
            for (HeapSegHdr* current = (HeapSegHdr*) heapStart; current != NULL; current = current->next)
            {
                if (current->free)
                {
                    freeSegCount += 1;
                    freeSegSpace += current->length;
                }
                else
                {
                    usedSegCount += 1;
                    usedSegSpace += current->length;
                }

                totalSegCount += 1;
                totalSegSpace += current->length;
            }
            Println(window, "> Total Amount of Heapsegments: {}", to_string(totalSegCount), Colors.yellow);
            Println(window, "> Amount of Free Heapsegments:  {}", to_string(freeSegCount), Colors.yellow);
            Println(window, "> Amount of Used Heapsegments:  {}", to_string(usedSegCount), Colors.yellow);
            Println(window);
            Println(window, "> Total Heap Space:             {} Bytes", to_string(totalSegSpace), Colors.yellow);
            Println(window, "> Free Heap Space:              {} Bytes", to_string(freeSegSpace), Colors.yellow);
            Println(window, "> Used Heap Space:              {} Bytes", to_string(usedSegSpace), Colors.yellow);
        }
    }
    else if (StrEquals(name, "heap stats detail"))
    {
        int64_t oA = activeMemFlagVal;
        activeMemFlagVal = oA + 1;
        Println(window, "Heap Statistics (Detailed):", Colors.yellow);
        // GlobalRenderer->Clear(Colors.dgray);
        // GlobalRenderer->Println("Heap Statistics (Detailed):", Colors.yellow);
        
        {
            uint64_t totCount = 0;
            for (HeapSegHdr* current = (HeapSegHdr*) heapStart; current != NULL; current = current->next)
                totCount++;


            uint64_t totalSegCount = 0, freeSegCount = 0, usedSegCount = 0, totalSegSpace = 0, freeSegSpace = 0, usedSegSpace = 0; 
            //for (HeapSegHdr* current = (HeapSegHdr*) heapStart; current->next != NULL; current = current->next)
            

            {
                HeapSegHdr* current = (HeapSegHdr*) heapStart;
                for (uint64_t tempCount = 0; tempCount < totCount && current != NULL; tempCount++)
                {
                    uint32_t col = Colors.gray;
                    if (current->free)
                    {
                        freeSegCount += 1;
                        freeSegSpace += current->length;
                        if (current->activeMemFlagVal == oA && oA != 0)
                            col = Colors.bgreen;
                        else
                            col = Colors.green;
                    }
                    else
                    {
                        usedSegCount += 1;
                        usedSegSpace += current->length;
                        if (current->activeMemFlagVal == oA && oA != 0)
                            col = Colors.yellow;
                        else
                            col = Colors.bred;
                    }
                    
                    dispVar vars[] = 
                    {
                        dispVar(totalSegCount), dispVar(current->length), 
                        dispVar(current->text), dispVar(current->activeMemFlagVal), 
                        dispVar(current->func), dispVar(current->file),
                        dispVar(current->line),
                        dispVar(current->time),
                        dispVar(ConvertHexToString((uint64_t)current + sizeof(HeapSegHdr)))
                    };
                    

                    //GlobalRenderer->Println("> Heap# {0} - Size: {1} Bytes, Title: \"{2}\", AMFV: {3}, ADDR: {4}, NEXT: {5}", vars, col);
                    if (oA == 0 || current->activeMemFlagVal == oA)
                        Println(window, "> Heap# {0} - Size: {1} Bytes, Title: \"{2}\", Func: \"{4}\", File: \"{5}\", Line: {6}, AMFV: {3}, TIME: {7}, Addr: {8}", vars, col);
                    //GlobalRenderer->Print("<T>");
                    //GlobalRenderer->Println(" Heap# {0} - Size: {1} Bytes, Title: \"{2}\", AMFV: {3}, ADDR: {4}, NEXT: {5}", vars, col);

                    // if(GlobalRenderer->CursorPosition.y > 600)
                    // {
                    //     PIT::Sleep(2000);
                    //     GlobalRenderer->Clear(Colors.dgray);
                    // }

                    totalSegCount += 1;
                    totalSegSpace += current->length;
                    current = current->next;
                }

                while (current != NULL && current->next != NULL)
                    current = current->next;
                if (current != NULL)
                {
uint32_t col = Colors.gray;
                    if (current->free)
                    {
                        freeSegCount += 1;
                        freeSegSpace += current->length;
                        if (current->activeMemFlagVal == oA && oA != 0)
                            col = Colors.bgreen;
                        else
                            col = Colors.green;
                    }
                    else
                    {
                        usedSegCount += 1;
                        usedSegSpace += current->length;
                        if (current->activeMemFlagVal == oA && oA != 0)
                            col = Colors.yellow;
                        else
                            col = Colors.bred;
                    }
                    
                    dispVar vars[] = 
                    {
                        dispVar(totalSegCount), dispVar(current->length), 
                        dispVar(current->text), dispVar(current->activeMemFlagVal), 
                        dispVar(current->func), dispVar(current->file),
                        dispVar(current->line)
                    };
                    

                    //GlobalRenderer->Println("> Heap# {0} - Size: {1} Bytes, Title: \"{2}\", AMFV: {3}, ADDR: {4}, NEXT: {5}", vars, col);
                    if (oA == 0 || current->activeMemFlagVal == oA)
                        Println(window, "> Heap# {0} - Size: {1} Bytes, Title: \"{2}\", Func: \"{4}\", File: \"{5}\", Line: {6}, AMFV: {3}", vars, col);
                    //GlobalRenderer->Print("<T>");
                    //GlobalRenderer->Println(" Heap# {0} - Size: {1} Bytes, Title: \"{2}\", AMFV: {3}, ADDR: {4}, NEXT: {5}", vars, col);

                    // if(GlobalRenderer->CursorPosition.y > 600)
                    // {
                    //     PIT::Sleep(2000);
                    //     GlobalRenderer->Clear(Colors.dgray);
                    // }

                    totalSegCount += 1;
                    totalSegSpace += current->length;

                }
            }

            // {
            //     PIT::Sleep(2000);
            //     GlobalRenderer->Clear(Colors.dblue);
            //     GlobalRenderer->Println("Done printing Heap Stats!", Colors.bgray);
            //     PIT::Sleep(2000);
            //     GlobalRenderer->Clear(Colors.dgray);
            // }

            Println(window);
            Println(window, "-----------------------------------");
            Println(window);
            Println(window, "> Total Amount of Heapsegments: {}", to_string(totalSegCount), Colors.yellow);
            Println(window, "> Amount of Free Heapsegments:  {}", to_string(freeSegCount), Colors.yellow);
            Println(window, "> Amount of Used Heapsegments:  {}", to_string(usedSegCount), Colors.yellow);
            Println(window);
            Println(window, "> Total Heap Space:             {} Bytes", to_string(totalSegSpace), Colors.yellow);
            Println(window, "> Free Heap Space:              {} Bytes", to_string(freeSegSpace), Colors.yellow);
            Println(window, "> Used Heap Space:              {} Bytes", to_string(usedSegSpace), Colors.yellow);
        }

        activeMemFlagVal = oA;
    }
    else
    {
        LogError("Parameter \"{}\" does not exist.", name, window);
    }

    // Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // Println("");
    RemoveFromStack();
}

ParsedColData ParseColor(const char* col)
{
    AddToStack();
    ParsedColData data = ParsedColData();
    data.parseSuccess = false;
    data.col = Colors.white;

    //Println("Col: \"{}\" (", col, Colors.orange);
    //Print(col[0]);
    //Println(")", Colors.orange);

    if (col[0] == '#')
    {
        const char* subStr = StrSubstr(col, 1);
        //Println("Col: \"{}\"", subStr, Colors.cyan);
        data.col = ConvertStringToHex(subStr);
        data.parseSuccess = true;
        _Free((void*)subStr);
        RemoveFromStack();
        return data;
    }

    for (int i = 0; i < colCount; i++)
        if (StrEquals(col, colNames[i]))
        {
            data.col = colValues[i];
            //Println("Color 1: {}", ConvertHexToString(colValues[i]), Colors.white);
            data.parseSuccess = true;
            break;
        }
    
    //Println("Color 2: {}", ConvertHexToString(data.col), Colors.white);

    RemoveFromStack();
    return data;
}

/*
    //Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
    StringArrData* data = SplitLine(input);
    //Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);


    Println("Parts:");
    for (int i = 0; i < data->len; i++)
        Println(" - \"{}\"", data->data[i], Colors.bgreen);


    //free(splitLine);
    GlobalAllocator->FreePage(data);
    //Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
*/

StringArrData* SplitLine(const char* input)
{
    AddToStack();
    uint64_t index = 0;
    uint64_t parts[256];
    uint64_t partIndex = 0;

    for (int i = 0; i < 256; i++)
        parts[i] = 0;

    bool inString = false;

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
            partIndex++;
        else 
        {
            parts[partIndex]++;
            if (input[index] == '\\')
                if (input[index + 1] != 0)
                    index++;
        }
    }

    int partCount = partIndex + 1;

    int totalsize = sizeof(char**) + sizeof(StringArrData);
    for (int i = 0; i < partCount; i++)
    {
        totalsize += parts[i] + 1 + sizeof(char*);
    }

    //uint64_t datAddr = (uint64_t) GlobalAllocator->RequestPage();
    int64_t datAddr = (uint64_t) _Malloc(totalsize, "Space for split command parameters");
    StringArrData* data = (StringArrData*) datAddr;

    //char** splitLine = (char**) GlobalAllocator->RequestPage(); //(char**)calloc(partCount, sizeof(char*));

    //Println("ADDR OG:      {}",ConvertHexToString((uint64_t)data),Colors.cyan);

    //Println("ADDR datAddr: {}",ConvertHexToString(datAddr),Colors.bgreen);

    data->addrOfData = (void*)(datAddr + sizeof(StringArrData));

    data->data = (char**)data->addrOfData;


    //Println("ADDR of Data 1: {}",ConvertHexToString((uint64_t)data->data),Colors.cyan);
    //Println("ADDR of Data 2: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    char** splitLine = data->data;
    data->len = partCount;

    // Println("ADDR 1: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    // Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    uint64_t splitIndex = sizeof(char**) + sizeof(char*) * partCount;

    //Println("ADDR of Data 3: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    for (int i = 0; i < partCount; i++)
    {
        //Println("INDEX  {}", to_string((uint64_t)splitIndex), Colors.cyan); 
        //Println("Count: {}", to_string((uint64_t)parts[i]), Colors.cyan);
        splitLine[i] = (char*)((uint64_t)data->addrOfData + splitIndex);
        //Println("ADDR 1: {}", ConvertHexToString((uint64_t)&splitLine[i][0]), Colors.yellow);

        for (int i2 = 0; i2 < parts[i] + 1; i2++)
            splitLine[i][i2] = 0;
       
        //Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine[i]), Colors.yellow);
        //Println("ADDR 3: {}", ConvertHexToString((uint64_t)data->addrOfData + splitIndex), Colors.yellow);
        splitIndex += parts[i] + 1;
    }

    // ---------------------------------------------------------------------------------------------------------------------------------

    inString = false;
    partIndex = 0;
    index = 0;

    for (int i = 0; i < 100; i++)
        parts[i] = 0;
    
    //Println("AAA");
    //Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    //Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);

    //Println("Lines:", Colors.yellow);

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
        {
            //Println();
            //Println("PartIndex:   {}", to_string(partIndex), Colors.cyan);
            //Println("PartIndex 2: {}", to_string(parts[partIndex]), Colors.cyan);
            //Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
            //Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
            splitLine[partIndex][parts[partIndex]] = 0;
            //Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
            //Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);
            partIndex++;
        }
        else 
        {
            if (input[index] == '\\')
                if (input[index + 1] != 0)
                    index++;
            
            //splitLine[partIndex][parts[partIndex]] = input[index];
            //*(splitLine[partIndex] + parts[partIndex]) = input[index];
            //Print(input[index]);
            //Println("Index:           {}", to_string(index), Colors.bgreen);
            //Println("Part Index:      {}", to_string(partIndex), Colors.bgreen);
            //Println("Part Part Index: {}", to_string(parts[partIndex]), Colors.bgreen);

            splitLine[partIndex][parts[partIndex]] = input[index];

            parts[partIndex]++;
        }
    }

    ///Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
    // Println();
    // Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    // Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
    // Print(" - \"{}\"", splitLine[partIndex], Colors.yellow);
    // Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);

    RemoveFromStack();
    return data;
}
