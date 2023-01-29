#include "kernelStuff/stuff/kernelUtil.h"
#include "OSDATA/osStats.h"
#include "WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "WindowStuff/SubInstances/guiInstance/guiStuff/components/rectangle/rectangleComponent.h"
#include "WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "WindowStuff/SubInstances/guiInstance/guiStuff/components/text/textComponent.h"
#include "WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"
#include "WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"
#include "tasks/closeWindow/taskWindowClose.h"

#include "rnd/rnd.h"

/*
if (osData.enableStackTrace)
    osData.stackPointer.add(MStack("_start", "kernel.cpp"));
if (osData.enableStackTrace)
    osData.stackPointer.removeLast();

*/

void TestClickHandler(GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    btn->position.y += 20;
}

void TestKeyHandler(GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::KeyHitEventInfo info)
{
    btn->position.y -= 20;

}






void RenderLoop()
{
    osData.windowPointerThing->Clear();
    osData.windowPointerThing->RenderWindows();
    bool updateBorder = true;
    bool bgm = osData.drawBackground;
    int frame = 0;
    int tFrame = 0;
    uint64_t oldTime = PIT::TimeSinceBootMS();
    //double fps = 1;
    uint64_t fps = 1;

    uint64_t frameSum = 0;

    
    uint32_t TwantedFps = 1000;
    uint64_t timeForFps = 1000;


    while(!osData.exit)
    {
        RND::lehmer64();
        AddToStack();
        freeCount = 0;
        mallocCount = 0;
        osStats.frameStartTime = PIT::TimeSinceBootMicroS();

        if (!osData.startMenuWindow->hidden && activeWindow != osData.startMenuWindow)
        {
            osData.startMenuWindow->hidden = true;
            osData.startMenuWindow->oldHidden = false;
            activeWindow = NULL;
        }

        if (TwantedFps != osData.wantedFps)
        {
            TwantedFps = osData.wantedFps;
            timeForFps = 1000000 / osData.wantedFps;
        }



        if (++tFrame >= 1000)
            tFrame = 0;

        frame++;  
        if (frame >= 30)
        {
            uint64_t currTime = PIT::TimeSinceBootMS();
            //fps = ((frame - 1)*1000.0) / (currTime - oldTime);
            if ((currTime - oldTime) == 0)
            {
                fps = 9999;
            }
            else
                fps = ((frame - 1)*1000) / (currTime - oldTime);
            oldTime = currTime;
            frame = 0;
        }
        RemoveFromStack();

        //testRect->position.x = frame * 5;
        //testRect->position.y = frame * 3;

        //box->position.x = frame * 6;
        


        ProcessMousePackets();
        HandleKeyboardList(20);

      

        if (bgm != osData.drawBackground)
        {
            osData.windowPointerThing->RenderWindows();
            bgm = osData.drawBackground;
        }

   

        AddToStack();
        {
            uint64_t tS = PIT::TimeSinceBootMicroS();

            if (activeWindow != NULL)
            {
                updateBorder = true;
                if (activeWindow->moveToFront)
                {
                    activeWindow->moveToFront = false;
                    int index = osData.windows.getIndexOf(activeWindow);
                    if (index == osData.windows.getCount() - 1)
                    {
                        osData.windowPointerThing->UpdateWindowBorder(activeWindow);
                        osData.windowPointerThing->RenderWindow(activeWindow);
                    }
                    else if (index != -1)
                    {
                        Window* oldActive = osData.windows[osData.windows.getCount() - 1];
                        osData.windows.removeAt(index);
                        osData.windows.add(activeWindow);
                        
                        osData.windowPointerThing->UpdateWindowBorder(oldActive);

                        osData.windowPointerThing->RenderWindow(activeWindow);
                        osData.windowPointerThing->UpdateWindowBorder(activeWindow);
                    }
                }
            }
            else
            {
                if (updateBorder)
                {
                    updateBorder = false;
                    {
                        Window* oldActive = osData.windows[osData.windows.getCount() - 1];
                        
                        osData.windowPointerThing->UpdateWindowBorder(oldActive);
                    }
                }
            }

            for (int i = 0; i < osData.windows.getCount(); i++)
            {            
                Window* window = osData.windows[i];

                if (window == osData.debugTerminalWindow && !osData.showDebugterminal)
                    continue;
                
                if (window->hidden != window->oldHidden)
                {
                    window->oldHidden = window->hidden;
                    osData.windowPointerThing->UpdatePointerRect(
                        window->position.x - 1, 
                        window->position.y - 23, 
                        window->position.x + window->size.width, 
                        window->position.y + window->size.height
                        );
                }

                {
                    int x1 = window->position.x - 1;
                    int y1 = window->position.y - 23;
                    int sx1 = window->size.width + 3;
                    int sy1 = window->size.height + 25;

                    bool update = false;

                    int x2 = x1;
                    int y2 = y1;
                    int sx2 = sx1;
                    int sy2 = sy2;

                    if (window->maximize != window->oldMaximize)
                    {
                        if (window->maximize)
                        {
                            window->oldPreMaxPosition = window->position;
                            window->oldPreMaxSize = window->size;

                            window->oldPreMaxBorder = window->showBorder;
                            window->oldPreMaxTitle = window->showTitleBar;

                            window->newPosition = Position(0, 23);
                            window->newSize = Size(GlobalRenderer->framebuffer->Width, GlobalRenderer->framebuffer->Height);

                            window->showBorder = false;
                            //window->showTitleBar = false;

                            window->oldMaximize = true;
                        }
                        else
                        {
                            window->newPosition = window->oldPreMaxPosition;
                            window->newSize = window->oldPreMaxSize;

                            window->showBorder = window->oldPreMaxBorder;
                            window->showTitleBar = window->oldPreMaxTitle;

                            window->oldMaximize = false;
                        }
                    }


                    Size nSize = window->newSize;
                    Position nPos = window->newPosition;


                    if (window->size != nSize)
                    {
                        window->Resize(nSize);
                        {
                            x2 = window->position.x - 1;
                            y2 = window->position.y - 23;
                            sx2 = window->size.width + 3;
                            sy2 = window->size.height + 25;

                            update = true;
                        }
                    }

                    if (window->position != nPos)
                    {
                        window->position = nPos;

                        x2 = window->position.x - 1;
                        y2 = window->position.y - 23;
                        sx2 = window->size.width + 3;
                        sy2 = window->size.height + 25;

                        update = true;
                    }

                    if (update)
                    {
                        int rx1 = min(x1, x2);
                        int ry1 = min(y1, y2);
                        int rx2 = max(x1 + sx1, x2 + sx2);
                        int ry2 = max(y1 + sy1, y2 + sy2);

                        int AR = (rx2 - rx1) * (ry2 - ry1);
                        int A1 = sx1 * sy1;
                        int A2 = sx2 * sy2;

                        if (AR <= A1+A2)
                        {
                            osData.windowPointerThing->UpdatePointerRect(rx1, ry1, rx2, ry2);
                        }
                        else
                        {
                            osData.windowPointerThing->UpdatePointerRect(x1, y1, x1 + sx1, y1 + sy1);
                            osData.windowPointerThing->UpdatePointerRect(x2, y2, x2 + sx2, y2 + sy2);
                        }
                    }
                }
                if (window->instance != NULL && !window->hidden && (activeWindow == window || frame % 5 == (i%8)))
                {
                    if (window->instance->instanceType == InstanceType::Terminal)
                    {
                        TerminalInstance* termInst1 = (TerminalInstance*)window->instance;
                        if (termInst1->newTermInstance != NULL)
                        {
                            NewTerminalInstance* termInst2 = (NewTerminalInstance*)termInst1->newTermInstance;
                            termInst2->DoRender();
                        }
                    }   
                    else if (window->instance->instanceType == InstanceType::GUI)
                    {
                        GuiInstance* guiInst = (GuiInstance*)window->instance;
                        guiInst->Render();
                    }   
                }
                
            }

            osStats.totalWindowUpdateTime = PIT::TimeSinceBootMicroS() - tS;
        }
        RemoveFromStack();



        {
            uint64_t tS = PIT::TimeSinceBootMicroS();
            AddToStack();
            Taskbar::RenderTaskbar();
            MPoint mPos = MousePosition;
            DrawMousePointer2(osData.windowPointerThing->virtualScreenBuffer, mPos);
            osData.windowPointerThing->fps = fps;
            osData.windowPointerThing->Render();
            osData.windowPointerThing->UpdatePointerRect(mPos.x - 32, mPos.y - 32, mPos.x + 32, mPos.y + 32);
            osStats.totalRenderTime = PIT::TimeSinceBootMicroS() - tS;
            RemoveFromStack();
        }



        {
            AddToStack();
            uint64_t totTaskTemp = 0;
            uint64_t totOsTaskTemp = 0;

            uint64_t tS1 = PIT::TimeSinceBootMicroS();
            uint64_t tempVal = osStats.frameStartTime + timeForFps;


            {
                for (int i = 0; i < osData.windows.getCount(); i++)
                {     
                    Window* window = osData.windows[i];
                    if (window->instance == NULL)
                        continue;
                    if (window->hidden)
                        continue;
                    if (window->instance->instanceType != InstanceType::Terminal)
                        continue;

                    TerminalInstance* terminal = (TerminalInstance*)window->instance;

                    if (terminal->tasks.getCount() != 0)
                    {
                        Task* task = terminal->tasks[0];
                        task->tempTime = 0;
                    }
                    else if (terminal->closeWindowAfterTask)
                    {
                        osData.osTasks.add(NewWindowCloseTask(window));
                    }
                }
            }

            bool startThing = true;
            while (PIT::TimeSinceBootMicroS() < tempVal || startThing)
            {
                startThing = false;
                //double endTime = PIT::TimeSinceBoot + 0.02;
                {
                    uint64_t tS = PIT::TimeSinceBootMicroS();
                    //for (int ax = 0; ax < 10; ax++)
                    {
                        //GlobalRenderer->Print("A");
                        for (int i = 0; i < osData.windows.getCount(); i++)
                        {     
                            //GlobalRenderer->Print("B");
                            
                            Window* window = osData.windows[i];
                            if (window->instance == NULL)
                                continue;
                            if (window->hidden)
                                continue;
                            if (window->instance->instanceType != InstanceType::Terminal)
                                continue;

                            TerminalInstance* terminal = (TerminalInstance*)window->instance;

                            if (terminal->tasks.getCount() != 0)
                            {
                                uint64_t tS2 = PIT::TimeSinceBootMicroS();
                                Task* task = terminal->tasks[0];
                                DoTask(task);
                                task->tempTime += PIT::TimeSinceBootMicroS() - tS2;
                                if (task->GetDone())
                                {
                                    terminal->tasks.removeFirst();
                                    FreeTask(task);
                                    //GlobalRenderer->Println("TASK DONE");
                                    terminal->PrintUserIfNeeded();
                                }
                                else
                                {
                                    
                                    //GlobalRenderer->Println("TASK NOT DONE");
                                }
                            }
                        }
                        //PIT::Sleep(10);
                        //asm("hlt");
                    }
                    totTaskTemp += PIT::TimeSinceBootMicroS() - tS;
                }
                

                {
                    uint64_t tS = PIT::TimeSinceBootMicroS();
                    if (osData.osTasks.getCount() > 0)
                    {
                        uint64_t tS2 = PIT::TimeSinceBootMicroS();
                        Task* task = osData.osTasks[0];
                        DoTask(task);
                        task->tempTime += PIT::TimeSinceBootMicroS() - tS2;
                        if (task->GetDone())
                        {
                            osData.osTasks.removeFirst();
                            FreeTask(task);
                        }
                    }
                    totOsTaskTemp += PIT::TimeSinceBootMicroS() - tS;
                }
            }

            {
                for (int i = 0; i < osData.windows.getCount(); i++)
                {     
                    Window* window = osData.windows[i];
                    if (window->instance == NULL)
                        continue;
                    if (window->instance->instanceType != InstanceType::Terminal)
                        continue;

                    TerminalInstance* terminal = (TerminalInstance*)window->instance;

                    if (terminal->tasks.getCount() != 0)
                    {
                        Task* task = terminal->tasks[0];
                        task->actTime = task->tempTime;
                    }
                }
            }

            osStats.totalIdleTime = PIT::TimeSinceBootMicroS() - tS1;
            osStats.totalTaskTime = totTaskTemp;
            osStats.totalOsTaskTime = totOsTaskTemp;
            RemoveFromStack();
        }




        //GlobalRenderer->Print("C");
        //asm("hlt");
        // {
        //     uint64_t tS = PIT::TimeSinceBootMicroS();
        //     uint64_t tempVal = osStats.frameStartTime + timeForFps;
        //     while (PIT::TimeSinceBootMicroS() < tempVal)
        //         asm("hlt");
        //     osStats.totalIdleTime = PIT::TimeSinceBootMicroS() - tS;
        // }

        // IO_CHECK();
        // osStats.testThing = port64Val;
        // if (keyboardWeird && !oldKeyboardWeird)
        // {
        //     oldKeyboardWeird = true;
            
        //     Window* msgWindow;
        //     {


        //         msgWindow = (Window*)_Malloc(sizeof(Window), "Warning Window");
        //         Size size = Size(800, 16*10);
        //         Position pos = Position(((GlobalRenderer->framebuffer->Width - size.width) / 2), ((GlobalRenderer->framebuffer->Height) / 5));
                
        //         if (msgWindow != NULL)
        //         {
        //             //GlobalRenderer->Println("BRUH 4.5", Colors.yellow);
        //             *(msgWindow) = Window(NULL, size, pos, "WARNING ABOUT YOUR PS/2 KEYBOARD", true, true, true);
        //             //GlobalRenderer->Println("BRUH 4.6", Colors.yellow);
        //             osData.windows.add(msgWindow);
        //             //GlobalRenderer->Println("BRUH 4.7", Colors.yellow);

        //             activeWindow = msgWindow;
        //             //osData.mainTerminalWindow = msgWindow;
        //             msgWindow->moveToFront = true;
        //         }
        //     }
        //     // it crashes between 4 and 5, probably while trying to allocate memory since it used all the memory
        //     //GlobalRenderer->Println("BRUH 5", Colors.yellow);
            
        //     if (msgWindow != NULL)
        //     {
        //         //GlobalRenderer->Println("BRUH 5.1", Colors.yellow);
        //         //GlobalRenderer->Print("Win x: {}", to_string(crashWindow->size.width), Colors.yellow);
        //         //GlobalRenderer->Println(", y: {}", to_string(crashWindow->size.height), Colors.yellow);
        //         msgWindow->renderer->Clear(Colors.black);
        //         //GlobalRenderer->Println("BRUH 5.2", Colors.yellow);
        //         msgWindow->renderer->Println("------------------------------------------------", Colors.bred);
        //         msgWindow->renderer->Println("WARNING: Your PS/2 Keyboard is having issues!", Colors.bred);
        //         msgWindow->renderer->Println("------------------------------------------------", Colors.bred);
        //         msgWindow->renderer->Println();
        //         //GlobalRenderer->Println("BRUH 5.3", Colors.yellow);
        //         msgWindow->renderer->Println("It should still work but it will probably cause issues if used with the mouse at the same time.", Colors.yellow);
        //         msgWindow->renderer->Println("The issue should be fixable with a restart.", Colors.yellow);
        //         msgWindow->renderer->Println();

        //     }

        //     msgWindow->hidden = false;
        //     msgWindow->oldHidden = true;


            

        // }

        osStats.frameEndTime = PIT::TimeSinceBootMicroS();
        osStats.totalFrameTime = osStats.frameEndTime - osStats.frameStartTime;
    }
}





void RecoverDed()
{
    //osData.osTasks.clear();

    for (int i = 0; i < osData.windows.getCount(); i++)
    {
        osData.windows.elementAt(i)->hidden = true;
        osData.windows.elementAt(i)->oldHidden = true;
    }

    if (osData.activeCrashWindow != NULL)
    {
        osData.activeCrashWindow->hidden = false;
        osData.activeCrashWindow->oldHidden = true;
    }

    if (osData.preCrashWindow != NULL)
    {
        osData.preCrashWindow->hidden = true;
        osData.preCrashWindow->oldHidden = true;
        //osData.osTasks.add(NewWindowCloseTask(osData.preCrashWindow));
        if (osData.preCrashWindow->instance != NULL && osData.preCrashWindow->instance->instanceType == InstanceType::Terminal)
        {
            TerminalInstance* terminal = (TerminalInstance*)osData.preCrashWindow->instance;
            while (terminal->tasks.getCount() > 0)
            {
                Task* tsk = terminal->tasks.elementAt(0);
                FreeTask(tsk);
                terminal->tasks.removeFirst();
            }
            NewTerminalInstance* bruh2 = ((NewTerminalInstance*)terminal->newTermInstance);
            if (bruh2 != NULL)
            {
                bruh2->Println("\n\nCurrent Tasks forcibly stopped, due them possibly having crashed the OS!", Colors.bred);
                terminal->PrintUserIfNeeded();
            }
        }
    }

    MStackData::stackPointer = 0;

    RenderLoop();
}









uint8_t port64Val = 0;
bool keyboardWeird = false;
bool oldKeyboardWeird = false;

void IO_CHECK()
{
    return;
    uint8_t t = inb(0x64);
    if (t == 0x1C)
        return;
    port64Val = t;

    uint64_t now = PIT::TimeSinceBootMS();
    if (port64Val == 0x1D && (now - osStats.lastKeyboardCall > 4000))
    {
        port64Val = inb(0x60);
        keyboardWeird = true;
        
        uint8_t real = TranslateScancode2(port64Val);
        //HandleKeyboard(real);
        
        if (KeyboardScancodeState[real])
            HandleKeyboard(real | (0b10000000));
        else
            HandleKeyboard(real & (~0b10000000));

        

        // io_wait();
        // outb(0x60, 0xF0);
        // io_wait();
        // outb(0x60, 0x01);
        // io_wait();
        // inb(0x60);
        // io_wait();
        //outb(0x60, 0x1C);
        //io_wait();
        //PIC_EndMaster();
        //PIC_EndSlave();
    }
    else if ((port64Val & 0b1 == 1) && (now - osStats.lastMouseCall > 2000))
    {
        uint8_t b = inb(0x60);
        //HandlePS2Mouse(b);
    }
}






//extern "C" void _start(BootInfo* bootInfo)
void boot(BootInfo* bootInfo)
{  
    osData.booting = false;
    osData.maxNonFatalCrashCount = 5;
    MStackData::stackPointer = 0;
    for (int i = 0; i < 1000; i++)
        MStackData::stackArr[i] = MStack();

    osData.enableStackTrace = RECORD_STACK_TRACE;
    AddToStack();
    osData.crashCount = 0;
    //
    //while(true);
    osData.booting = true;
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;
    //AddToStack();
    osData.osTasks = List<Task*>(4);


    osData.kernelInfo = &kernelInfo;
    osData.exit = false;

    Window* debugTerminalWindow = osData.debugTerminalWindow;
    //Window* realMainWindow = osData.realMainWindow;
    //Window* realMainWindow2 = osData.realMainWindow2;
    
    osData.drawBackground = true;
    
    
    GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->DrawImage(bootInfo->bootImage, 0, 0, 1, 1);

    osData.mouseSensitivity = 1.0;


    // CRASH OR FREEZE ON BOOT IMAGE
    // float x = 1;
    // GlobalRenderer->Clear(Colors.black);
    // GlobalRenderer->Println("LESS GO!");
    // while (true);


    

    if (PIT::TicksSinceBoot != 0)
    {
        uint64_t endTime = PIT::TimeSinceBootMS() + 1000;
        while (PIT::TimeSinceBootMS() < endTime && osData.booting)
            asm("hlt");
        osData.booting = false;
    }
    else
    {
        GlobalRenderer->CursorPosition.x = 0;
        GlobalRenderer->CursorPosition.y = 0;
        
        GlobalRenderer->color = Colors.bred;
        GlobalRenderer->Println("ERROR: Interrupts are not working properly!");
        GlobalRenderer->color = Colors.yellow;
        GlobalRenderer->Println("Please reboot the system.");

        while (true)
            asm("hlt");
    }
    
    GlobalRenderer->Clear(Colors.black);

//    while(true);

    //GlobalRenderer->Clear(Colors.black);
    
    // {
    //     int xdiff = 20;
    
    //     GlobalRenderer->Println("Filecount: {}\n", to_string(bootInfo->windowIconZIP->fileCount), Colors.yellow);
    //     int ydiff = GlobalRenderer->CursorPosition.y + 10;
    //     for (int i = 0; i < bootInfo->windowIconZIP->fileCount; i++)
    //     {
    //         kernelFiles::ImageFile* image = kernelFiles::ConvertFileToImage(&bootInfo->windowIconZIP->files[i]);
    //         GlobalRenderer->DrawImage(image, xdiff, ydiff, 1, 1);
    //         //GlobalRenderer->DrawImage(image, xdiff + image->xOff, ydiff + image->yOff + 20, 1, 1);
    //         xdiff += image->width + 20;
    //     }
    //     // {
    //     //     kernelFiles::ImageFile* image = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));
    //     //     if (image != NULL)
    //     //         GlobalRenderer->DrawImage(image, xdiff, ydiff, 2, 2);
    //     // }
    // }
    
    // while (true);

    // if (bootInfo->testImage != NULL)
    // {
    //     dispVar vars[] = {dispVar(bootInfo->testImage->width), dispVar(bootInfo->testImage->height)};
    //     GlobalRenderer->Println("Image: {0}x{1}", vars);
    // }
    // else
    //     GlobalRenderer->Println("No Image found!");

    //GlobalRenderer->DrawImage(bootInfo->testImage, 100, 10);
    

    //while(true);



    //CopyFrameBuffer(osData.realMainWindow->framebuffer, osData.realMainWindow2->framebuffer);

    Window* mainWindow;
    {
        mainWindow = (Window*)_Malloc(sizeof(Window), "Main window");
        TerminalInstance* terminal = (TerminalInstance*)_Malloc(sizeof(TerminalInstance), "Terminal Instance for main window");
        *terminal = TerminalInstance(&adminUser);
        // NewTerminalInstance* terminal = (NewTerminalInstance*)malloc(sizeof(NewTerminalInstance), "New Terminal Instance for main window");
        // *terminal = NewTerminalInstance(&adminUser, mainWindow);

        *(mainWindow) = Window((DefaultInstance*)terminal, Size(600, 500), Position(5, 30), "Main Window", true, true, true);
        osData.windows.add(mainWindow);
        terminal->SetWindow(mainWindow);



        activeWindow = mainWindow;
        osData.mainTerminalWindow = mainWindow;
    }


    AddToStack();
    GuiInstance* testGui;
    GuiComponentStuff::BoxComponent* box;
    GuiComponentStuff::RectangleComponent* testRect;
    {
        Window* window = (Window*)_Malloc(sizeof(Window), "GUI Window");
        GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "GUI Instance");
        *gui = GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(50, 50), Position(500, 100), "Testing GUI Window", true, true, true);
        osData.windows.add(window);
        window->hidden = true;
        gui->Init();

        testGui = gui;
        // gui->Clear();
        // gui->WriteText("This \\B1100FFis a \\FFF00FFtest!");
        // gui->WriteText("Oh ma go\nsh");
        //osData.windows[1]->renderer->Println("Hello, world!");
        //KeyboardPrintStart(window);

        

        {
            GuiComponentStuff::ComponentSize s = GuiComponentStuff::ComponentSize(60, 20);
            
            s.IsXFixed = false;
            s.ScaledX = 0.5;
            
            
            testRect = new GuiComponentStuff::RectangleComponent(Colors.purple, s, testGui->screen);
            testRect->position = GuiComponentStuff::Position(100, 20);
        }

    

        {
            box = new GuiComponentStuff::BoxComponent(
                testGui->screen, GuiComponentStuff::ComponentSize(240, 240), Colors.tblack
            );
            testGui->screen->children->add(box);
            box->position = GuiComponentStuff::Position(20, 30);

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.dgray, GuiComponentStuff::ComponentSize(180, 180), box);
                t->position = GuiComponentStuff::Position(0, 40);
                box->children->add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.white, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(40, 80);
                box->children->add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.white, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(120, 80);
                box->children->add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.orange, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(80, 120);
                box->children->add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.bred, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(20, 140);
                box->children->add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.bred, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(140, 140);
                box->children->add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.bred, GuiComponentStuff::ComponentSize(100, 20), box);
                t->position = GuiComponentStuff::Position(40, 160);
                box->children->add(t);
            }
        }

        


        {
            GuiComponentStuff::TextComponent* txt = new GuiComponentStuff::TextComponent(testGui->screen, Colors.black, Colors.white, "Hello!\nThis is an amazing test.", 
            GuiComponentStuff::Position(200, 90));
            txt->id = 993344;
            testGui->screen->children->add(txt);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("CLICK\nME\nPLS", 
            Colors.black, Colors.dgray, Colors.gray, 
            Colors.bgreen, Colors.yellow, Colors.bred, 
            GuiComponentStuff::ComponentSize(150, 80),
            GuiComponentStuff::Position(210, 160), testGui->screen
            );
            btn->mouseClickedCallBack = TestClickHandler;
            btn->keyHitCallBack = TestKeyHandler;
            //btn->stickToDefaultColor = true;
            btn->id = 995544;
            
            testGui->screen->children->add(btn);
        }

        {
            GuiComponentStuff::TextFieldComponent* txtField = new GuiComponentStuff::TextFieldComponent(
            Colors.white,
            Colors.black,
            GuiComponentStuff::ComponentSize(150, 80),
            GuiComponentStuff::Position(100, 260), testGui->screen
            );
            //btn->mouseClickedCallBack = TestClickHandler;
            //btn->keyHitCallBack = TestKeyHandler;
            //btn->stickToDefaultColor = true;
            
            testGui->screen->children->add(txtField);
        }

        testGui->screen->children->add(testRect);


        {
            uint64_t bleh = 0;
            *((int*)&bleh) = 1;
            testGui->SetBaseComponentAttribute(993344, GuiInstanceBaseAttributeType::POSITION_Y, bleh);
        }

        {
            uint64_t bleh = 0;
            *((uint32_t*)&bleh) = Colors.red;
            testGui->SetSpecificComponentAttribute(995544, 10, bleh);
        }

        {
            uint64_t bleh = 0;
            *((double*)&bleh) = 0.3;
            testGui->SetBaseComponentAttribute(995544, GuiInstanceBaseAttributeType::SIZE_SCALED_Y, bleh);
        }

        {
            uint64_t bleh = 0;
            *((bool*)&bleh) = false;
            testGui->SetBaseComponentAttribute(995544, GuiInstanceBaseAttributeType::SIZE_IS_FIXED_Y, bleh);
        }
    }
    RemoveFromStack();
    


    
    osData.windows.add(osData.debugTerminalWindow);



    debugTerminalWindow->Log("Kernel Initialised Successfully!");

    for (int i = 0; i < 9; i++)
        debugTerminalWindow->Log("<STAT>");
    debugTerminalWindow->renderer->CursorPosition.y = 16 * 16;

    ((TerminalInstance*)mainWindow->instance)->Cls();
    ((TerminalInstance*)mainWindow->instance)->KeyboardPrintStart();
    //mainWindow->Render();







    osData.wantedFps = 150;


    //PIT::Sleep(4000);

    // int Ax = 0;
    // int Ay = 0;

    // int Az = Ax/Ay;

    RenderLoop();




    GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->color = Colors.white;
    GlobalRenderer->Println("Goodbye.");
    PIT::Sleep(1000);
    GlobalRenderer->Clear(Colors.black);


    RemoveFromStack();
    return;
}














#include "kernel.h"

void bootTest(Framebuffer fb, ACPI::RSDP2* rsdp, PSF1_FONT* psf1_font, MaslOsAssetStruct* assets, void* freeMemStart, void* extraMemStart, uint64_t freeMemSize, void* kernelStart, uint64_t kernelSize, void* kernelStartV)
{
    
    BootInfo tempBootInfo;
    tempBootInfo.framebuffer = &fb;
    tempBootInfo.rsdp = rsdp;

    tempBootInfo.psf1_font = psf1_font;

    tempBootInfo.testImage = assets->testImage;
    tempBootInfo.bootImage = assets->bootImage;
    tempBootInfo.MButton = assets->MButton;
    tempBootInfo.MButtonS = assets->MButtonS;
    tempBootInfo.bgImage = assets->bgImage;
    tempBootInfo.maabZIP = assets->maabZIP;

    tempBootInfo.mouseZIP = assets->mouseZIP;
    tempBootInfo.windowButtonZIP = assets->windowButtonZIP;
    tempBootInfo.windowIconsZIP = assets->windowIconsZIP;

    tempBootInfo.mMapStart = freeMemStart;
    tempBootInfo.m2MapStart = extraMemStart;
    tempBootInfo.mMapSize = freeMemSize;
    
    tempBootInfo.kernelStart = kernelStart;
    tempBootInfo.kernelSize = kernelSize;
    tempBootInfo.kernelStartV = kernelStartV;

    for (int y = 0; y < 100; y++)
        for (int x = 500; x < 600; x++)
            *(uint32_t*)((uint64_t)fb.BaseAddress + 4 * (x + y * fb.PixelsPerScanLine)) = (Colors.yellow * ((x + y + 1) % 2)) + (Colors.green * ((x + y) % 2));

    //while (true);

    boot(&tempBootInfo);
    return;
}