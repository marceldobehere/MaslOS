#include "kernelUtil.h"

extern "C" void _start(BootInfo* bootInfo)
{  
    //while(true);
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    osData.kernelInfo = &kernelInfo;
    osData.exit = false;
    osData.windows = List<Window*>();

    // if (bootInfo->testImage != NULL)
    // {
    //     dispVar vars[] = {dispVar(bootInfo->testImage->width), dispVar(bootInfo->testImage->height)};
    //     GlobalRenderer->Println("Image: {0}x{1}", vars);
    // }
    // else
    //     GlobalRenderer->Println("No Image found!");

    //GlobalRenderer->DrawImage(bootInfo->testImage, 100, 10);
    

    //while(true);


    Window* realMainWindow;
    {
        realMainWindow = (Window*)malloc(sizeof(Window));
        *(realMainWindow) = Window(NULL, Size(GlobalRenderer->framebuffer->Width, GlobalRenderer->framebuffer->Height), Position(0, 0), GlobalRenderer, "Real Main Window");
        osData.realMainWindow = realMainWindow;
    }

    Window* mainWindow;
    {
        mainWindow = (Window*)malloc(sizeof(Window));
        TerminalInstance* terminal = (TerminalInstance*)malloc(sizeof(TerminalInstance));
        *terminal = TerminalInstance(&adminUser);
        *(mainWindow) = Window((DefaultInstance*)terminal, Size(600, 500), Position(5, 30), realMainWindow->renderer, "Main Window");
        osData.windows.add(mainWindow);

        activeWindow = mainWindow;
    }

    {
        Window* window = (Window*)malloc(sizeof(Window));
        TerminalInstance* terminal = (TerminalInstance*)malloc(sizeof(TerminalInstance));
        *terminal = TerminalInstance(&guestUser);
        *(window) = Window((DefaultInstance*)terminal, Size(400, 360), Position(700, 60), realMainWindow->renderer, "Testing Window");
        osData.windows.add(window);
    }
 
    
    //osData.windows[1]->renderer->Clear(Colors.blue);
    // osData.windows[1]->renderer->color = Colors.white;

    osData.windows[1]->renderer->Cls();  
    osData.windows[1]->renderer->DrawImage(bootInfo->testImage, 150, 0);
    //osData.windows[1]->renderer->Println("Hello, world!");
    KeyboardPrintStart(osData.windows[1]);
    
    

    //activeWindow = osData.windows[1];


    activeWindow->renderer->Cls();
    activeWindow->renderer->Println("Kernel Initialised Successfully!", Colors.yellow);
    KeyboardPrintStart(mainWindow);
    mainWindow->Render();

    while(!osData.exit)
    {
        realMainWindow->renderer->Clear(Colors.dblue);

        if (activeWindow != NULL)
        {
            if (activeWindow->moveToFront)
            {
                activeWindow->moveToFront = false;
                int index = osData.windows.getIndexOf(activeWindow);
                if (index != -1)
                {
                    osData.windows.removeAt(index);
                    osData.windows.add(activeWindow);
                }
            }
        }

        //DrawMousePointer1(realMainWindow->framebuffer);
        for (int i = 0; i < osData.windows.getCount(); i++)
        {            
            Window* window = osData.windows[i];
            window->position = window->newPosition;
            if (window->size != window->newSize)
            {
                window->size = window->newSize;
            }
            window->Render();
        }
        DrawMousePointer2(realMainWindow->framebuffer);

        realMainWindow->Render();
        



        PIT::Sleep(50);
        //asm("hlt");
    }

    GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->color = Colors.white;
    GlobalRenderer->Println("Goodbye.");
    PIT::Sleep(1000);
    GlobalRenderer->Clear(Colors.black);
}



/*



    // GlobalRenderer->Print("Memory Size: ");
    // GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    // GlobalRenderer->Println(" Bytes.");
    // GlobalRenderer->Println();


    // GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // GlobalRenderer->Println("");

        //GlobalRenderer->Println("ADDR Window:   {}", ConvertHexToString((uint64_t)window), Colors.bgreen);
        //GlobalRenderer->Println("ADDR Terminal: {}", ConvertHexToString((uint64_t)terminal), Colors.bgreen);

    // {
    //     Window* window = (Window*)malloc(sizeof(Window));
    //     TerminalInstance* terminal = (TerminalInstance*)malloc(sizeof(TerminalInstance));
    //     *terminal = TerminalInstance(&guestUser);
    //     *(window) = Window((DefaultInstance*)terminal, Size(480, 360), Position(100, 20), GlobalRenderer->framebuffer);
    //     osData.windows.add(window);

    //     //GlobalRenderer->Println("ADDR Window:   {}", ConvertHexToString((uint64_t)window), Colors.bgreen);
    //     //GlobalRenderer->Println("ADDR Terminal: {}", ConvertHexToString((uint64_t)terminal), Colors.bgreen);
    // }


    //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    //GlobalRenderer->Println("");
    //GlobalRenderer->Println("");

    // {
    //     dispVar vars[] = 
    //     {
    //         dispVar((uint64_t)GlobalRenderer->framebuffer->Width), 
    //         dispVar((uint64_t)GlobalRenderer->framebuffer->PixelsPerScanLine),
    //         dispVar((uint64_t)GlobalRenderer->framebuffer->Height), 
    //         dispVar((uint64_t)(GlobalRenderer->framebuffer->Width*GlobalRenderer->framebuffer->Height)), 
    //         dispVar((uint64_t)GlobalRenderer->framebuffer->BufferSize), 
    //     };
    //     GlobalRenderer->Println("Width:  {0}, PPS: {1}.", vars);
    //     GlobalRenderer->Println("Height: {2}.", vars);
    //     GlobalRenderer->Println("H * W:  {3}.", vars);
    //     GlobalRenderer->Println("Size:   {4}.", vars);
    //     GlobalRenderer->Println();
    // }
    
    // {
    //     Window* window = osData.windows[0];
    //     dispVar vars[] = 
    //     {
    //         dispVar((uint64_t)window->framebuffer->Width), 
    //         dispVar((uint64_t)window->framebuffer->PixelsPerScanLine),
    //         dispVar((uint64_t)window->framebuffer->Height), 
    //         dispVar((uint64_t)(window->framebuffer->Width * window->framebuffer->Height)), 
    //         dispVar((uint64_t)window->framebuffer->BufferSize), 
    //     };
    //     GlobalRenderer->Println("Width:  {0}, PPS: {1}.", vars);
    //     GlobalRenderer->Println("Height: {2}.", vars);
    //     GlobalRenderer->Println("H * W:  {3}.", vars);
    //     GlobalRenderer->Println("Size:   {4}.", vars);
    //     GlobalRenderer->Println();
    // }

    // osData.windows[0]->renderer->Clear(Colors.blue);
    // osData.windows[0]->renderer->color = Colors.white;
    // osData.windows[0]->renderer->Println("Hello, world!");
    // osData.windows[0]->Render();

    //osData.windows[1]->Render();

    // osData.windows[2]->renderer->Clear(Colors.green);
    // osData.windows[2]->renderer->color = Colors.white;
    // osData.windows[2]->renderer->Println("Hello, world!");
    // //osData.windows[2]->Render();
*/



/*

    //GlobalRenderer->Println("TEST: {}", to_string(to_int("1024")), Colors.bblue);
    //GlobalRenderer->Println("TEST: {}", to_string(to_int("-1024")), Colors.bblue);

    {
        dispVar vars[] = {dispVar("test"), dispVar(2), dispVar(0)};
        GlobalRenderer->Println("This is a {0}! Also {2}+2={1}.", vars);
    }

    {
        dispVar vars[] = {dispVar("cool thing"), dispVar(4), dispVar(2)};
        GlobalRenderer->Println("This is a {0}! Also {2}+2={1}.", vars);
    }

*/

/*
    //PIT::SetDivisor(65535);

    //GlobalRenderer->delChar(0, 0);
 
    //Panic("Panic go brrrt"); 
    //asm("int $0x0e");

*/

/*
    GlobalRenderer->Println("TIME: {} s", to_string(PIT::TimeSinceBoot), Colors.bred);
    GlobalRenderer->Println("TIME: {} ms", to_string((int)(PIT::TimeSinceBoot*1000)), Colors.bred);
    GlobalRenderer->Println("DIV:  {}", to_string(PIT::Divisor), Colors.bred);
    GlobalRenderer->Println("FREQ: {} Hz", to_string(PIT::freq), Colors.bred);

    GlobalRenderer->Println();

    for (int i = 0; i < 20; i++)
    {
        GlobalRenderer->Print("hoi! ");
        PIT::Sleep(100);
    }

    GlobalRenderer->Println();
    GlobalRenderer->Println();

    GlobalRenderer->Println("TIME: {} s", to_string(PIT::TimeSinceBoot), Colors.bred);
    GlobalRenderer->Println("TIME: {} ms", to_string((int)(PIT::TimeSinceBoot*1000)), Colors.bred);
    GlobalRenderer->Println("DIV:  {}", to_string(PIT::Divisor), Colors.bred);
    GlobalRenderer->Println("FREQ: {} Hz", to_string(PIT::freq), Colors.bred);

*/


/*

    GlobalRenderer->Println("TEST: {}", to_string(0.0), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(0.123), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(0.12345), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(10.0), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(10.123), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(10.12345), Colors.bred);
    GlobalRenderer->Println();
    GlobalRenderer->Println("TEST: {}", to_string(-0.0), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-0.123), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-0.12345), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-10.0), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-10.123), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-10.12345), Colors.bred);
    GlobalRenderer->Println();

    GlobalRenderer->Println("TEST: {}", to_string(0.0, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(0.123, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(0.12345, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(10.0, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(10.123, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(10.12345, 4), Colors.bred);
    GlobalRenderer->Println();
    GlobalRenderer->Println("TEST: {}", to_string(-0.0, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-0.123, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-0.12345, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-10.0, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-10.123, 4), Colors.bred);
    GlobalRenderer->Println("TEST: {}", to_string(-10.12345, 4), Colors.bred);
    GlobalRenderer->Println();
*/


/*





    Array<int> arr = Array<int>(10);

    arr[0] = 5; 
    GlobalRenderer->Println("ARR 1:");
    GlobalRenderer->Println("Size:  {}", to_string(arr.getSize()), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[0]), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[2]), Colors.silver);
    GlobalRenderer->Println();


    arr.free();

    List<int> list = List<int>();
    GlobalRenderer->Println("Size:  {}", to_string(list.getCount()), Colors.silver);
    GlobalRenderer->Println("Cap:   {}", to_string(list.getCapacity()), Colors.silver);
    
    GlobalRenderer->Println("Items:", Colors.silver);
    for (uint64_t i = 0; i < list.getCount(); i++)
        GlobalRenderer->Println(" - {}", to_string(list[i]), Colors.silver);
    GlobalRenderer->Println();

    list.add(10);
    list.add(20);
    list.add(5);
    list.add(8);
    list.add(99);
    list.add(1000);
    list.add(1);
    list.add(151);
    list.add(7);


    GlobalRenderer->Println("Size:  {}", to_string(list.getCount()), Colors.silver);
    GlobalRenderer->Println("Cap:   {}", to_string(list.getCapacity()), Colors.silver);

    GlobalRenderer->Println("Items:", Colors.silver);
    for (uint64_t i = 0; i < list.getCount(); i++)
        GlobalRenderer->Println(" - {}", to_string(list[i]), Colors.silver);
    GlobalRenderer->Println();

    list[3] = 1000000;


    list.removeAt(4);



    GlobalRenderer->Println("Size:  {}", to_string(list.getCount()), Colors.silver);
    GlobalRenderer->Println("Cap:   {}", to_string(list.getCapacity()), Colors.silver);

    GlobalRenderer->Println("Items:", Colors.silver);
    for (uint64_t i = 0; i < list.getCount(); i++)
        GlobalRenderer->Println(" - {}", to_string(list[i]), Colors.silver);
    GlobalRenderer->Println();
    GlobalRenderer->Println();

    List<int> list2 = list.clone();
    list.clear();

    GlobalRenderer->Println("Size:  {}", to_string(list.getCount()), Colors.silver);
    GlobalRenderer->Println("Cap:   {}", to_string(list.getCapacity()), Colors.silver);

    GlobalRenderer->Println("Items:", Colors.silver);
    for (uint64_t i = 0; i < list.getCount(); i++)
        GlobalRenderer->Println(" - {}", to_string(list[i]), Colors.silver);
    GlobalRenderer->Println();
    GlobalRenderer->Println();


    GlobalRenderer->Println("Size:  {}", to_string(list2.getCount()), Colors.silver);
    GlobalRenderer->Println("Cap:   {}", to_string(list2.getCapacity()), Colors.silver);

    GlobalRenderer->Println("Items:", Colors.silver);
    for (uint64_t i = 0; i < list2.getCount(); i++)
        GlobalRenderer->Println(" - {}", to_string(list2[i]), Colors.silver);
    GlobalRenderer->Println();


*/


/*

    Array<int> arr = Array<int>(10);

    arr[0] = 5;
    GlobalRenderer->Println("ARR 1:");
    GlobalRenderer->Println("Size:  {}", to_string(arr.size), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[0]), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[2]), Colors.silver);
    GlobalRenderer->Println("Freed: {}", to_string(arr.freed), Colors.silver);
    GlobalRenderer->Println();

    // arr.free();
    
    // GlobalRenderer->Println("Size:  {}", to_string(arr.size), Colors.silver);
    // GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[0]), Colors.silver);
    // GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[2]), Colors.silver);
    // GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[11]), Colors.silver);
    // GlobalRenderer->Println("Freed: {}", to_string(arr.freed), Colors.silver);

    Array<int> arr2 = arr.clone();

    arr[0] = 15;
    GlobalRenderer->Println("ARR 1:");
    GlobalRenderer->Println("Size:  {}", to_string(arr.size), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[0]), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[2]), Colors.silver);
    GlobalRenderer->Println("Freed: {}", to_string(arr.freed), Colors.silver);

    GlobalRenderer->Println("ARR 2:");
    GlobalRenderer->Println("Size:  {}", to_string(arr2.size), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr2[0]), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr2[2]), Colors.silver);
    GlobalRenderer->Println("Freed: {}", to_string(arr2.freed), Colors.silver);
    GlobalRenderer->Println();

    arr.free();

    GlobalRenderer->Println("ARR 1:");
    GlobalRenderer->Println("Size:  {}", to_string(arr.size), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[0]), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr[2]), Colors.silver);
    GlobalRenderer->Println("Freed: {}", to_string(arr.freed), Colors.silver);

    GlobalRenderer->Println("ARR 2:");
    GlobalRenderer->Println("Size:  {}", to_string(arr2.size), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr2[0]), Colors.silver);
    GlobalRenderer->Println("Value: {}", to_string((uint64_t)arr2[2]), Colors.silver);
    GlobalRenderer->Println("Freed: {}", to_string(arr2.freed), Colors.silver);
    GlobalRenderer->Println();
*/


/*

    // GlobalRenderer->Print("Memory Size: ");
    // GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    // GlobalRenderer->Println(" Bytes.");
    // GlobalRenderer->Println();


    // GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // GlobalRenderer->Println("");

    // int* arr[100];// = (int*)GlobalAllocator->RequestPage();
    // for (int i = 0; i < 100; i++)
    //     arr[i] = (int*)GlobalAllocator->RequestPage();

    // GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // GlobalRenderer->Println("");

    // arr[0][1] = 100;


    // for (int i = 0; i < 100; i++)
    //     GlobalAllocator->FreePage(arr[i]);

    // GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // GlobalRenderer->Println("");
 
    // GlobalRenderer->Println("ADDR OF VRAM:       {}", ConvertHexToString((uint64_t)GlobalRenderer->framebuffer->BaseAddress), Colors.bgreen);
    // GlobalRenderer->Println("ADDR OF NEW DATA 1: {}", ConvertHexToString((uint64_t)heapStart), Colors.silver);
    // GlobalRenderer->Println("ADDR OF NEW DATA:   {}", ConvertHexToString((uint64_t)malloc(0x100)), Colors.silver);
    // GlobalRenderer->Println("ADDR OF NEW DATA:   {}", ConvertHexToString((uint64_t)malloc(0x100)), Colors.silver);
    // GlobalRenderer->Println("ADDR OF NEW DATA:   {}", ConvertHexToString((uint64_t)malloc(0x100)), Colors.silver);
    // GlobalRenderer->Println("");

    //GlobalRenderer->Println("ADDR OF VRAM:       {}", ConvertHexToString((uint64_t)GlobalRenderer->framebuffer->BaseAddress), Colors.bgreen);
    //GlobalRenderer->Println("ADDR OF NEW DATA 1: {}", ConvertHexToString((uint64_t)heapStart), Colors.silver);
    //GlobalRenderer->Println("ADDR OF NEW DATA:   {}", ConvertHexToString((uint64_t)malloc(0x10)), Colors.silver);
    //GlobalRenderer->Println("ADDR OF NEW DATA:   {}", ConvertHexToString((uint64_t)malloc(0x20)), Colors.silver);
    //GlobalRenderer->Println("ADDR OF NEW DATA:   {}", ConvertHexToString((uint64_t)malloc(0x30)), Colors.silver);
    //GlobalRenderer->Println("ADDR OF NEW DATA:   {}", ConvertHexToString((uint64_t)malloc(0x40)), Colors.silver);
    //GlobalRenderer->Println("");

    //GlobalRenderer->Println("Username: {}", adminUser.userName);
    //GlobalRenderer->Println("Username: {}", currentUser->userName);

    // GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // GlobalRenderer->Println("Free Page Count: {} pages.", to_string(GlobalAllocator->GetFreePageCount()), Colors.bgreen);

    // for (int i = 0; i < 5; i++)
    // {
    //     //GlobalAllocator->RequestPage(); //works
    //     GlobalRenderer->Println("ADDR OF NEW DATA: {}", ConvertHexToString((uint64_t)malloc(0x1000)), Colors.silver); // makes the page count function crash
    //     GlobalRenderer->Println("Free Page Count: {} pages.", to_string(GlobalAllocator->GetFreePageCount()), Colors.bgreen);
    //     //GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    //     //GlobalRenderer->Println("");
    // }

    // GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // GlobalRenderer->Println("Free Page Count: {} pages.", to_string(GlobalAllocator->GetFreePageCount()), Colors.bgreen);
    // GlobalRenderer->Println("Done.");

*/


/*
   //GlobalRenderer->Println("ADDR OF NEW DATA: {}", ConvertHexToString((uint64_t)malloc(0x100)), Colors.silver);

    GlobalRenderer->Println();

    GlobalRenderer->Print("Memory Size: ");
    GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    GlobalRenderer->Println(" Bytes.");
    GlobalRenderer->Println();

    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");

    int* arr = (int*)GlobalAllocator->RequestPage();

    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");

    arr[0] = 100;

    GlobalAllocator->FreePage(arr);

    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");


    GlobalRenderer->Println("ADDR OF NEW DATA: {}", ConvertHexToString((uint64_t)malloc(0x100)), Colors.silver);

    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");
*/

/*
    GlobalRenderer->Print("Memory Size: ");
    GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    GlobalRenderer->Println(" Bytes.");
    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");

    int* arr = (int*)GlobalAllocator->RequestPage();

    GlobalRenderer->Print("Memory Size: ");
    GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    GlobalRenderer->Println(" Bytes.");
    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");

    arr[0] = 100;

    GlobalAllocator->FreePage(arr);

    GlobalRenderer->Print("Memory Size: ");
    GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    GlobalRenderer->Println(" Bytes.");
    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");

*/
 
/*
    GlobalRenderer->Print("Memory Size: ");
    GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    GlobalRenderer->Println(" Bytes.");
    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator.GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");

    int* arr = (int*)GlobalAllocator.RequestPage();

    GlobalRenderer->Print("Memory Size: ");
    GlobalRenderer->Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    GlobalRenderer->Println(" Bytes.");
    GlobalRenderer->Println("Free: {} Bytes.", to_string(GlobalAllocator.GetFreeRAM()), Colors.bgreen);
    GlobalRenderer->Println("");
*/




/*
    GlobalRenderer->Println("Test 1: {}", to_string((uint64_t)100));
    GlobalRenderer->Println("Test 2: {}", to_string(100.5, 2));
    GlobalRenderer->Clear(Colors.orange);
    
    
    asm("int $0x0e");
    
    for (unsigned int y = 10; y < bootInfo->framebuffer->Height / 4; y++)
    {
        for (unsigned int x = 0; x < bootInfo->framebuffer->Width / 2 * 4; x+=4)
        {
            *(unsigned int*)(x + (y * (bootInfo->framebuffer->PixelsPerScanLine * 4) + bootInfo->framebuffer->BaseAddress)) = 0xff00ffff;
        }
    }
 
    temp.Print("Memory Size: ");
    temp.Print(to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)));
    temp.Println(" Bytes.");
    temp.Println("");


    // {
    //     const char* a[] = {to_string(-7.5, 1), to_string((int64_t)-10000)};
    //     temp.Println("The values are: {} and {}.", (const char**)a);
    // }

    temp.Println("New Page Map loaded!");

    pageTableManager->MapMemory((void*)0x600000000, (void*)0x80000);

    uint64_t* test = (uint64_t*)0x600000000;

    *test = 26;

    temp.Println("Testing virtual Memory Adress: {}", to_string(*test));



    //temp.Println("Hello, \\CFF00FFWorld!\\CFFFFFF");
    PageMapIndexer pageIndexer = PageMapIndexer(0x1000 * 52 + 0x50000 * 7);

    temp.Println("P_i: {}", to_string(pageIndexer.P_i));
    temp.Println("PT_i: {}", to_string(pageIndexer.PT_i));
    temp.Println("PD_i: {}", to_string(pageIndexer.PD_i));
    temp.Println("PDP_i: {}", to_string(pageIndexer.PDP_i));
    temp.Println("Size: {} Pages.", to_string(kernelPages));

    temp.Println("Free RAM:     {} KB", to_string(newAllocator.GetFreeRAM() / 1024));
    temp.Println("Used RAM:     {} KB", to_string(newAllocator.GetUsedRAM() / 1024));
    temp.Println("Reserved RAM: {} KB", to_string(newAllocator.GetReservedRAM() / 1024));
    temp.Println();
    temp.Println("Total RAM:    {} KB", to_string((newAllocator.GetFreeRAM() + newAllocator.GetUsedRAM() + newAllocator.GetReservedRAM()) / 1024));
    temp.Println();


    for (int t = 0; t < 20; t++)
    {
        void* adress = newAllocator.RequestPage();
        temp.Println("Received Page address: {}", ConvertHexToString((uint64_t)adress));
    }


    temp.Println("Total RAM:    {} KB", to_string(GetMemorySize(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize)/ 1024));
    temp.Println();
    temp.Println("-----------------------------");
    temp.Println();
    //PrintEFIMemData(bootInfo->mMap, (bootInfo->mMapSize / bootInfo->mMapDescSize), bootInfo->mMapDescSize, &temp);

    uint8_t testbuffer[20];
    Bitmap testBitMap;
    testBitMap.Buffer = &testbuffer[0];
    // testBitMap.Set(0, true); 
    // testBitMap.Set(2, true); 
    // testBitMap.Set(3, true);  
    // testBitMap.Set(5, true); 
    // testBitMap.Set(11, true);  
    // testBitMap.Set(12, true); 
    // testBitMap.Set(14, true);
    // testBitMap.Set(16, true);

    for (uint64_t i = 0; i < 20; i ++)
    {
        testBitMap.Set(i, false);
    }

    for (uint64_t i = 0; i < 20; i++)
    {
        temp.Print(to_string(i));
        temp.Print(": ");
        temp.color = BasicRenderer::Colors::red;
        if (testBitMap[i]) 
            temp.color = BasicRenderer::Colors::green;
        temp.Println("#");
        temp.color = BasicRenderer::Colors::white;
    }

    for (uint64_t i = 0; i < 20; i += 1 + (i % 3))
    {
        testBitMap.Set(i, true);
    }
    temp.Println("");

    for (uint64_t i = 0; i < 20; i++)
    {
        temp.Print(to_string(i));
        temp.Print(": ");
        temp.color = BasicRenderer::Colors::red;
        if (testBitMap[i])
            temp.color = BasicRenderer::Colors::green;
        temp.Println("#");
        temp.color = BasicRenderer::Colors::white;
    }
	unsigned int BBP = 4;

    for (unsigned int y = 10; y < framebuffer->Height / 4; y++)
    {
        for (unsigned int x = 0; x < framebuffer->Width / 2 * BBP; x+=BBP)
        {
            *(unsigned int*)(x + (y * (framebuffer->PixelsPerScanLine * BBP) + framebuffer->BaseAddress)) = 0xff00ffff;
        }
    }
    //putChar(framebuffer, psf1_font, 0xffff0000, 'A', 0, 0);

     //putStr(framebuffer, psf1_font, 0xffff00ff, "Hello, World!", 16, 16);

    temp.printStr("Hello, World!\n\r");

    temp.printStr(ConvertHexToString((uint64_t)0x002244668811AACC));
    temp.printStr("\n\r");
    temp.printStr(ConvertHexToString((uint32_t)0x0022AA66));
    temp.printStr("\n\r"); 
    temp.printStr(ConvertHexToString((uint16_t)0xA056));
    temp.printStr("\n\r");
    temp.printStr(ConvertHexToString((uint8_t)0x0A));
    temp.printStr("\n\r"); 
    temp.printStr(to_string((uint64_t)125));
    temp.printStr("\n\r");   
    temp.printStr(to_string((int64_t)-125)); 
    temp.printStr("\n\r");
    temp.printStr(to_string((int64_t)125));  
    temp.printStr("\n\r");
    temp.printStr(to_string(10.623, 1));
    temp.printStr("\n\r");
    temp.printStr(to_string(-10.615, 2));
    temp.printStr("\n\r");
    temp.printStr(to_string(-10.615, 4));
    temp.printStr("\n\r");
    temp.printStr(to_string(-10.615, 0));
    temp.printStr("\n\r");
    temp.printStr(to_string(510.623, 3));
    temp.printStr("\n\r");

    

    if (ConvertStringToHex("FF0AFF", temp) == 0x00FF0AFF)
        temp.printStr("String conversion works!\n\r");
    else 
        temp.printStr("String conversion doesn't work.\n\r");

    temp.printStr(to_string((uint64_t)125));
    temp.printStr("\n\r");
    temp.printStr(to_string((int64_t)-125));
    temp.printStr("\n\r");
    

    //printStr(framebuffer, psf1_font, 0xffff00ff, "Hello,");
    //printStr(framebuffer, psf1_font, 0xffff0000, " World!\n\r");
    //printStr(framebuffer, psf1_font, 0xff00ff00, "Hoi!");
    //printStr(framebuffer, psf1_font, 0xff0000ff, "\nHello, World!\n\r");

    //printStr(framebuffer, psf1_font, 0xffff00ffff, "Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi Hoi ");







*/