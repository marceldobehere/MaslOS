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

int kernelPanicCount = 0;

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

    if (osData.maxNonFatalCrashCount-- > 0 && !lock && !osData.booting)
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
                    *(crashWindow) = Window(NULL, size, pos, "Crash Window", true, true, true);
                    //GlobalRenderer->Println("BRUH 4.6", Colors.yellow);
                    osData.windows.add(crashWindow);
                    //GlobalRenderer->Println("BRUH 4.7", Colors.yellow);

                    activeWindow = crashWindow;
                    osData.mainTerminalWindow = crashWindow;
                    crashWindow->moveToFront = true;
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
                crashWindow->renderer->Println("------------------------------------------------", Colors.bred);
                crashWindow->renderer->Println("A (probably) non-fatal Kernel Panic has occured!", Colors.bred);
                crashWindow->renderer->Println("------------------------------------------------", Colors.bred);
                crashWindow->renderer->Println();
                //GlobalRenderer->Println("BRUH 5.3", Colors.yellow);
                crashWindow->renderer->Println("Panic Message:", Colors.yellow);
                crashWindow->renderer->Println(panicMessage, var, Colors.bred);
                crashWindow->renderer->Println();
                //GlobalRenderer->Println("BRUH 5.4", Colors.yellow);
                PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer, crashWindow->renderer, Colors.yellow);
                //GlobalRenderer->Println("BRUH 5.5", Colors.yellow);
            }
            //GlobalRenderer->Println("BRUH 6", Colors.yellow);

            osData.tempCrash = false;    
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
        for (int i = 0; i < kernelPanicCount; i++)
            GlobalRenderer->Println();
        GlobalRenderer->Println(panicMessage, var, Colors.white);
        GlobalRenderer->Println();

        osData.crashCount++;
        if (osData.crashCount <= 2)
        {
            osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - 500;
            osData.debugTerminalWindow->position.y = 23;
            osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
            osData.debugTerminalWindow->Render(osData.debugTerminalWindow->framebuffer, GlobalRenderer->framebuffer, osData.debugTerminalWindow->position, osData.debugTerminalWindow->size, osData.debugTerminalWindow);
        }
        else
        {
            GlobalRenderer->Println("(BTW the rendering of the debug terminal is causing issues so no debug terminal)");
            GlobalRenderer->Println();

            PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
            GlobalRenderer->Println();
            GlobalRenderer->Println();
            PrintRegisterDump(GlobalRenderer);

            while(true)
                asm("hlt");
        }
        
        PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
        GlobalRenderer->Println();
        GlobalRenderer->Println();
        PrintRegisterDump(GlobalRenderer);
        
        //if (lock)
        while(true)
            asm("hlt");
    }
    
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




