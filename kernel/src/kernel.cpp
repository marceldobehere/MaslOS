#include "kernelUtil.h"


extern "C" void _start(BootInfo* bootInfo)
{  
    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;


    GlobalRenderer->Println("Kernel Initialised Successfully!", Colors.yellow);

    //GlobalRenderer->delChar(0, 0);

    //Panic("Panic go brrrt"); 
    //asm("int $0x0e");
   
    while(true)
    {
        //ProcessMousePacket();
    }
    
    while(true); 

}
 

 

 

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