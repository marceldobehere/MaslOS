#include "panic.h"
#include "../Rendering/BasicRenderer.h"
#include "../Rendering/Cols.h" 
#include "../OSDATA/osdata.h"
#include "../OSDATA/MStack/MStackM.h"

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
}


void Panic(const char* panicMessage, const char* var, bool lock)
{
    AddToStack();
    osData.crashed = true;

    // GlobalRenderer->Clear(Colors.black);
    // GlobalRenderer->Println("Remaining Count 0: {}", to_string(osData.maxNonFatalCrashCount--), Colors.yellow);
    // GlobalRenderer->Println("Remaining Count 1: {}", to_string(osData.maxNonFatalCrashCount--), Colors.yellow);
    // while(true);

    if (osData.maxNonFatalCrashCount-- > 0)
    {
        if (osData.tempCrash)
        {
            for (int i = 0; i < osData.windows.getCount(); i++)
                osData.windows[i]->hidden = true;
        }
        else
        {
            osData.tempCrash = true;
            Window* crashWindow;
            {
                crashWindow = (Window*)malloc(sizeof(Window), "Crash Window");
                Size size = Size(800, 16*10 + (MStackData::stackPointer * (16*4)));
                Position pos = Position(((GlobalRenderer->framebuffer->Width - size.width) / 2), ((GlobalRenderer->framebuffer->Height) / 5));
                *(crashWindow) = Window(NULL, size, pos, "Crash Window", true, true, true);
                osData.windows.add(crashWindow);

                activeWindow = crashWindow;
                osData.mainTerminalWindow = crashWindow;
                crashWindow->moveToFront = true;
            }

            {
                crashWindow->renderer->Clear(Colors.black);
                crashWindow->renderer->Println("------------------------------------------------", Colors.bred);
                crashWindow->renderer->Println("A (probably) non-fatal Kernel Panic has occured!", Colors.bred);
                crashWindow->renderer->Println("------------------------------------------------", Colors.bred);
                crashWindow->renderer->Println();
                crashWindow->renderer->Println("Panic Message:", Colors.yellow);
                crashWindow->renderer->Println(panicMessage, var, Colors.bred);
                crashWindow->renderer->Println();
                PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer, crashWindow->renderer, Colors.yellow);
            }


            osData.tempCrash = false;    
        }
    }
    else
    {
        GlobalRenderer->ClearDotted(Colors.red);
        GlobalRenderer->Println();
        GlobalRenderer->Println();
        GlobalRenderer->Println("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA", Colors.white);
        GlobalRenderer->Println();
        GlobalRenderer->Println(panicMessage, var, Colors.white);
        GlobalRenderer->Println();

        osData.crashCount++;
        if (osData.crashCount == 1)
        {
            osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - (osData.debugTerminalWindow->size.width + 2);
            osData.debugTerminalWindow->position.y = 23;
            osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
            osData.debugTerminalWindow->Render(osData.debugTerminalWindow->framebuffer, GlobalRenderer->framebuffer, osData.debugTerminalWindow->position, osData.debugTerminalWindow->size, osData.debugTerminalWindow);
        }
        else
        {
            GlobalRenderer->Println("(BTW the rendering of the debug terminal is causing issues so no debug terminal)");
            GlobalRenderer->Println();
        }

        PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
        GlobalRenderer->Println();
        GlobalRenderer->Println();
        PrintRegisterDump(GlobalRenderer);
        
        if (lock)
            while(true);
    }

    RemoveFromStack();
} 

void Panic(const char* panicMessage, const char* var)
{
    Panic(panicMessage, var, true);
}

void Panic(const char* panicMessage)
{
    Panic(panicMessage, NULL, true);
}

void Panic(const char* panicMessage, bool lock)
{
    Panic(panicMessage, NULL, lock);
}




