#include "panic.h"
#include "../Rendering/BasicRenderer.h"
#include "../Rendering/Cols.h" 
#include "../OSDATA/osdata.h"
#include "../OSDATA/MStack/MStackM.h"
#include "../WindowStuff/SubInstances/customInstance/customInstance.h"
#include "../kernelStuff/other_IO/serial/serial.h"

void PrintRegisterDump(BasicRenderer* renderer)
{
    uint64_t Register = 0;
    //Panic("Page Fault Detected! {}", to_string(*((uint64_t*)frame)), true);
    renderer->Println();
	renderer->Println("Register dump: ");
    renderer->Println();
	renderer->Print("rax: ");
	asm volatile("mov %%rax, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rcx: ");
	asm volatile("mov %%rcx, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rdx: ");
	asm volatile("mov %%rdx, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rbx: ");
	asm volatile("mov %%rbx, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
    renderer->Println();
	renderer->Print("rsp: ");
	asm volatile("mov %%rsp, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rbp: ");
	asm volatile("mov %%rbp, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rsi: ");
	asm volatile("mov %%rsi, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rdi: ");
	asm volatile("mov %%rdi, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
    renderer->Println();

    Serial::Writeln();
    Serial::Writeln("Register dump: ");
    Serial::Writeln();
    Serial::Write("rax: ");
    asm volatile("mov %%rax, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));
    Serial::Write("  ");
    Serial::Write("rcx: ");
    asm volatile("mov %%rcx, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));
    Serial::Write("  ");
    Serial::Write("rdx: ");
    asm volatile("mov %%rdx, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));
    Serial::Write("  ");
    Serial::Write("rbx: ");
    asm volatile("mov %%rbx, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));    
    Serial::Writeln();
    Serial::Write("rsp: ");
    asm volatile("mov %%rsp, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));
    Serial::Write("  ");
    Serial::Write("rbp: ");
    asm volatile("mov %%rbp, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));
    Serial::Write("  ");
    Serial::Write("rsi: ");
    asm volatile("mov %%rsi, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));
    Serial::Write("  ");
    Serial::Write("rdi: ");
    asm volatile("mov %%rdi, %0" : "=r"(Register));
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(Register));
    Serial::Writeln();
}

int kernelPanicCount = 0;


void CreateWindowWithBenchmarkData()
{
    AddToStack();

    SaveBenchmarkStack(MStackData::BenchmarkMode);
    int bMax = MStackData::BenchmarkStackPointerSave;
    if (bMax > 1500)
        bMax = 1500;

    osData.tempCrash = true;
    //GlobalRenderer->Println("BRUH 4", Colors.yellow);
    Window* benchmarkWindow;
    {
        benchmarkWindow = (Window*)_Malloc(sizeof(Window), "Benchmark Window");
        Size size = Size(2000, 16*10 + (bMax * 16)); // 36MB * 2 Max
        Position pos = Position(0, 0);
        
        if (benchmarkWindow != NULL)
        {
            //GlobalRenderer->Println("BRUH 4.5", Colors.yellow);
            *(benchmarkWindow) = Window(new CustomInstance(InstanceType::WARNING), size, pos, "Crash Benchmark", true, true, true);
            //GlobalRenderer->Println("BRUH 4.6", Colors.yellow);
            osData.windows.add(benchmarkWindow);
            
            //GlobalRenderer->Println("BRUH 4.7", Colors.yellow);

            // activeWindow = benchmarkWindow;
            // osData.mainTerminalWindow = benchmarkWindow;
            // osData.activeCrashWindow = benchmarkWindow;
            // benchmarkWindow->moveToFront = true;
        }
    }




    bool tempBench = MStackData::BenchmarkEnabled;
    MStackData::BenchmarkEnabled = false;

    benchmarkWindow->renderer->Println("Benchmark");
    for (int i = 0; i < bMax; i++)
    {
        for (int x = 0; x < MStackData::BenchmarkStackArrSave[i].layer; x++)
            //Print(window, "--", Colors.yellow);
            benchmarkWindow->renderer->Print("--", "", Colors.yellow);


        bool skipNext = false;
        if (i  + 1 < bMax && !MStackData::BenchmarkStackArrSave[i].close && MStackData::BenchmarkStackArrSave[i + 1].close && 
            StrEquals(MStackData::BenchmarkStackArrSave[i].name, MStackData::BenchmarkStackArrSave[i + 1].name))
        {
            skipNext = true;
            benchmarkWindow->renderer->Print("o {}: ", to_string(i), Colors.yellow);
        }
        else
        {
            if (MStackData::BenchmarkStackArrSave[i].close)
                benchmarkWindow->renderer->Print("< {}: ", to_string(i), Colors.yellow);
            else
                benchmarkWindow->renderer->Print("> {}: ", to_string(i), Colors.yellow);
        }   

        benchmarkWindow->renderer->Print("L: {}", to_string(MStackData::BenchmarkStackArrSave[i].layer), Colors.bgreen);
        benchmarkWindow->renderer->Print(", Name: {}", (MStackData::BenchmarkStackArrSave[i].name), Colors.yellow);
        benchmarkWindow->renderer->Print(", File {}", (MStackData::BenchmarkStackArrSave[i].filename), Colors.orange);
        benchmarkWindow->renderer->Print(", Line {}", to_string(MStackData::BenchmarkStackArrSave[i].line), Colors.orange);
        benchmarkWindow->renderer->Print(", Time {}", to_string(MStackData::BenchmarkStackArrSave[i].time), Colors.orange);
        benchmarkWindow->renderer->Println();

        if (skipNext)
                i++;
    }


    MStackData::BenchmarkEnabled = tempBench;
    RemoveFromStack();
}

#include "../memory/heap.h"

void LockOsStuffLoop(Window* termWindow)
{
    AddToStack();

    AddToStack();
    {
        uint64_t tS = PIT::TimeSinceBootMicroS();
        if (osData.osTasks.getCount() > 0)
        {
            uint64_t tS2 = PIT::TimeSinceBootMicroS();
            Task* task = osData.osTasks[0];
            AddToStack();
            DoTask(task);
            RemoveFromStack();
            task->tempTime += PIT::TimeSinceBootMicroS() - tS2;
            if (task->GetDone())
            {
                osData.osTasks.removeFirst();
                FreeTask(task);
            }
        }
        //totOsTaskTemp += PIT::TimeSinceBootMicroS() - tS;
    }
    RemoveFromStack();

    AddToStack();
    {
        for (int i = 0; i < osData.windows.getCount(); i++)
        {     
            i = osData.windows.getCount();
            Window* window = termWindow; // only do terminal window
            if (window->instance == NULL)
                continue;
            if (window->hidden && !osData.bgTaskRun)
                continue;
            if (window->instance->instanceType != InstanceType::Terminal)
                continue;

            TerminalInstance* terminal = (TerminalInstance*)window->instance;

            if (terminal->tasks.getCount() > 0)
            {
                uint64_t tS2 = PIT::TimeSinceBootMicroS();
                Task* task = terminal->tasks[0];
                AddToStack();
                DoTask(task);
                RemoveFromStack();
                task->tempTime += PIT::TimeSinceBootMicroS() - tS2;
                if (task->GetDone())
                {
                    terminal->tasks.removeFirst();
                    FreeTask(task);
                    //GlobalRenderer->Println("TASK DONE");
                    terminal->PrintUserIfNeeded();
                }
            }
        }
    }
    RemoveFromStack();

    RemoveFromStack();
}

#include "../tasks/maab/maabTask.h"
#include "../tasks/bfTask/bfTask.h"
#include "../kernelStuff/other_IO/acpi/acpiShutdown.h"

void LockLoop()
{
    AddToStack();
    Serial::Writeln();
    osData.NO_INTERRUPTS = true;
    Serial::Writeln("<HALTED OS>");
    Serial::Writeln();
    if (backupHeapFailed)
    {
        Serial::Writeln("The Backup heap failed and therefore the OS cannot continue.");
        Serial::Writeln("(DePaST cannot launch without memory working)");
        Serial::Writeln("Please restart the computer.");
        Serial::Writeln();
        while (true)
        {
            asm("hlt");
        }
    }

    Serial::Writeln("MaslOS Debug Panic Serial Terminal (DePaST)");
    Serial::Writeln();


    Serial::Writeln("> Initing...");
    Serial::Writeln("> Creating Debug Terminal Instance...");
    Window* mainWindow;
    OSUser* mainUser = &adminUser;
    TerminalInstance* oldTerm;
    NewTerminalInstance* newTerm;
    {
        mainWindow = (Window*)_Malloc(sizeof(Window), "Main window");
        TerminalInstance* terminal = (TerminalInstance*)_Malloc(sizeof(TerminalInstance), "Terminal Instance for main window");
        *terminal = TerminalInstance(mainUser);
        oldTerm = terminal;

        Serial::Writeln("> Setting Terminal to redirect to Serial...");
        newTerm = ((NewTerminalInstance*)terminal->newTermInstance); 
        newTerm->redirectToSerial = true;
        // NewTerminalInstance* terminal = (NewTerminalInstance*)malloc(sizeof(NewTerminalInstance), "New Terminal Instance for main window");
        // *terminal = NewTerminalInstance(&adminUser, mainWindow);

        Serial::Writeln("> Creating Debug Window...");
        *(mainWindow) = Window((DefaultInstance*)terminal, Size(1, 1), Position(0, 0), "Main Window", true, true, true);
        osData.windows.add(mainWindow);
        Serial::Writeln("> Init Done!");
        terminal->SetWindow(mainWindow);
        terminal->Cls();

        activeWindow = mainWindow;
        //osData.mainTerminalWindow = mainWindow;
    }


    Serial::Writeln();

    const int maxInputLen = 500;
    char input[maxInputLen + 1];
    char lastInput[maxInputLen + 1];
    int inputLen = 0;
    for (int i = 0; i < maxInputLen + 1; i++)
    {
        input[i] = 0;
        lastInput[i] = input[i];
    }
    
    Serial::Writeln();
    Serial::Write("> ");

    osData.exit = false;
    while (!osData.exit)
    {
        LockOsStuffLoop(mainWindow);
        if (Serial::CanRead())
        {
            char c = Serial::Read();

            bool keyHandled = false;
            {
                TaskMAAB* maab = NULL;
                for (int i = 0; i < oldTerm->tasks.getCount(); i++)
                {
                    if (oldTerm->tasks[i]->GetType() == TaskType::MAAB)
                    {
                        maab = (TaskMAAB*)oldTerm->tasks[i];
                        break;
                    }
                }

                if (maab != NULL && maab->waitInput && !maab->gotInput)
                {
                    keyHandled = true;
                    if (c == '\r')
                    {
                        maab->gotInput = true;
                    }
                    else
                    {
                        if (c == '\n' || maab->memUserInputLen >= 490)
                        {
                            maab->gotInput = true;
                        }
                        else if (c == '\b')
                        {
                            if (maab->memUserInputLen > 0)
                            {
                                maab->memUserInput[--maab->memUserInputLen] = 0;
                                newTerm->DeleteLastCharInLine();
                            }
                        }
                        else
                        {
                            maab->memUserInput[maab->memUserInputLen++] = c;
                            newTerm->Print(c);
                        }
                    }
                }
            }

            if (keyHandled)
            {

            }
            else if (c == '\r')
            {
                Serial::Writeln();
                //Serial::Writeln("<TEST>");
                newTerm->Clear();
                
                if (inputLen > 0)
                {
                    input[inputLen] = 0;    
                    ParseCommand(input, lastInput, &mainUser, mainWindow);
                }

                inputLen = 0;
                for (int i = 0; i < maxInputLen + 1; i++)
                {
                    lastInput[i] = input[i];
                    input[i] = 0;
                }
                
                Serial::Writeln();
                Serial::Write("> ");
            }
            else if (c == '\b')
            {
                if (inputLen > 0)
                {
                    Serial::Write("\b \b");
                    input[--inputLen] = 0;
                }
            }
            else
            {
                if (inputLen < maxInputLen)
                {
                    input[inputLen++] = c;
                    Serial::Write(c);
                }
            }
        }
    }
    PowerOffAcpi();

    GlobalRenderer->Clear(Colors.black);
    Serial::Writeln();
    GlobalRenderer->Println("The ACPI shutdown failed!", Colors.yellow);
    Serial::Writeln("The ACPI shutdown failed!");

    GlobalRenderer->Println();
    Serial::Writeln();
    GlobalRenderer->Println("Please shut down the computer manually.", Colors.white);
    Serial::Writeln("Please shut down the computer manually.");

    while (true)
        asm("hlt");

    RemoveFromStack();
}



void Panic(const char* panicMessage, const char* var, bool lock)
{
    if (!lock)
        AddToStack();
    //GlobalRenderer->CursorPosition.x = 0;
    //GlobalRenderer->CursorPosition.y = 0;
    // oh god please no test podcast
    //GlobalRenderer->Println("<BRUH START>", Colors.yellow);
    osData.crashed = true;

    // GlobalRenderer->Clear(Colors.black);
    // GlobalRenderer->Println("Remaining Count 0: {}", to_string(osData.maxNonFatalCrashCount--), Colors.yellow);
    // GlobalRenderer->Println("Remaining Count 1: {}", to_string(osData.maxNonFatalCrashCount--), Colors.yellow);
    // while(true);

    // btw this is an multi crash kernel panic
    // since for some reason the cls somehow did the general protection fault multiple times so multiple crash windows -> so kernel panic
    // since ya know the mnfcc

    // ok so it crashes
    // just get a good cmd working with good fs stuff and a programming lang

    osData.preCrashWindow = activeWindow;

    if (!lock && osData.maxNonFatalCrashCount-- > 0 && !osData.booting && !osData.tempCrash)
    {
        //GlobalRenderer->Println("BRUH 1", Colors.yellow);
        if (osData.tempCrash)
        {
            //GlobalRenderer->Println("BRUH 2", Colors.yellow);
            for (int i = 0; i < osData.windows.getCount(); i++)
                osData.windows[i]->hidden = true;
            //GlobalRenderer->Println("BRUH 3", Colors.yellow);
        }
        else
        {
            AddToStack();
            osData.tempCrash = true;
            //GlobalRenderer->Println("BRUH 4", Colors.yellow);
            Window* crashWindow;
            {
                crashWindow = (Window*)_Malloc(sizeof(Window), "Crash Window");
                Size size = Size(800, 16*10 + (MStackData::stackPointer * (16*4)));
                Position pos = Position(((GlobalRenderer->framebuffer->Width - size.width) / 2), ((GlobalRenderer->framebuffer->Height) / 5));
                
                if (crashWindow != NULL)
                {
                    //GlobalRenderer->Println("BRUH 4.5", Colors.yellow);
                    *(crashWindow) = Window(new CustomInstance(InstanceType::WARNING), size, pos, "Crash Warning", true, true, true);
                    //GlobalRenderer->Println("BRUH 4.6", Colors.yellow);
                    osData.windows.add(crashWindow);
                    
                    //GlobalRenderer->Println("BRUH 4.7", Colors.yellow);

                    // activeWindow = crashWindow;
                    // //osData.mainTerminalWindow = crashWindow;
                    // osData.activeCrashWindow = crashWindow;
                    // crashWindow->moveToFront = true;
                    osData.windowsToGetActive.add(crashWindow);
                }
            }
            // it crashes between 4 and 5, probably while trying to allocate memory since it used all the memory
            //GlobalRenderer->Println("BRUH 5", Colors.yellow);
            
            if (crashWindow != NULL)
            {
                //GlobalRenderer->Println("BRUH 5.1", Colors.yellow);
                //GlobalRenderer->Print("Win x: {}", to_string(crashWindow->size.width), Colors.yellow);
                //GlobalRenderer->Println(", y: {}", to_string(crashWindow->size.height), Colors.yellow);
                crashWindow->renderer->Clear(Colors.black);
                //GlobalRenderer->Println("BRUH 5.2", Colors.yellow);
                crashWindow->renderer->Println("-------------------------------------------------------", Colors.bred);
                crashWindow->renderer->Println("A (probably) non-fatal Kernel Panic has occured! (MNFCC: {})", to_string(osData.maxNonFatalCrashCount), Colors.bred);
                crashWindow->renderer->Println("-------------------------------------------------------", Colors.bred);
                crashWindow->renderer->Println();
                //GlobalRenderer->Println("BRUH 5.3", Colors.yellow);
                crashWindow->renderer->Println("Panic Message:", Colors.yellow);
                crashWindow->renderer->Print(panicMessage, var, Colors.bred);
                crashWindow->renderer->Println("  (MNFCC: {})",  to_string(osData.maxNonFatalCrashCount), Colors.white);

                crashWindow->renderer->Println();
                crashWindow->renderer->Println();
                //GlobalRenderer->Println("BRUH 5.4", Colors.yellow);
                PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer, crashWindow->renderer, Colors.yellow);
                //GlobalRenderer->Println("BRUH 5.5", Colors.yellow);



                CreateWindowWithBenchmarkData();
            }
            //GlobalRenderer->Println("BRUH 6", Colors.yellow);

            osData.tempCrash = false;    
            RemoveFromStack();
        }
        //GlobalRenderer->Println("<BRUH END>", Colors.yellow);
    }
    else
    {
        kernelPanicCount++;
        GlobalRenderer->ClearDotted(Colors.red);
        GlobalRenderer->Println();
        GlobalRenderer->Println();
        GlobalRenderer->Println("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA", Colors.white);
        Serial::Writeln();
        Serial::Writeln();
        Serial::Writeln();
        Serial::Writeln("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA");
        for (int i = 0; i < kernelPanicCount; i++)
            GlobalRenderer->Println();
        GlobalRenderer->Print(panicMessage, var, Colors.white);
        Serial::Write(panicMessage, var, true);
        GlobalRenderer->Println("  (MNFCC: {})",  to_string(osData.maxNonFatalCrashCount), Colors.white);
        Serial::Writeln("  (MNFCC: {})",  to_string(osData.maxNonFatalCrashCount), true);

        GlobalRenderer->Println();
        GlobalRenderer->Println();
        Serial::Writeln();

        osData.crashCount++;
        if (osData.crashCount <= 2 && !osData.booting)
        {
            osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - 500;
            osData.debugTerminalWindow->position.y = 23;
            osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
            osData.debugTerminalWindow->Render(osData.debugTerminalWindow->framebuffer, GlobalRenderer->framebuffer, osData.debugTerminalWindow->position, osData.debugTerminalWindow->size, osData.debugTerminalWindow);
            Serial::Writeln("<INSERT DEBUG TERMINAL DATA HERE>");
        }
        else
        {
            GlobalRenderer->Println("(BTW the rendering of the debug terminal is causing issues so no debug terminal)");
            GlobalRenderer->Println();
            Serial::Writeln("(BTW the rendering of the debug terminal is causing issues so no debug terminal)");

            PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
            GlobalRenderer->Println();
            GlobalRenderer->Println();
            Serial::Writeln();
            Serial::Writeln();
            PrintRegisterDump(GlobalRenderer);

            LockLoop();
        }
        
        PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
        GlobalRenderer->Println();
        GlobalRenderer->Println();
        Serial::Writeln();
        Serial::Writeln();
        PrintRegisterDump(GlobalRenderer);
        
        //if (lock)

        LockLoop();
    }

    osData.tempCrash = false;   

    if (!lock)
        RemoveFromStack(); 
} 

void Panic(const char* panicMessage, const char* var)
{
    Panic(panicMessage, var, true);
}

void Panic(const char* panicMessage)
{
    Panic(panicMessage, NULL, false);
}

void Panic(const char* panicMessage, bool lock)
{
    Panic(panicMessage, NULL, lock);
}




