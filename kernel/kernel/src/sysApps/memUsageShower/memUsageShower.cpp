#include "memUsageShower.h"
#include "../../cStdLib/cStdLib.h"
#include "../../memory/heap.h"
#include "../../osData/osData.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"
#include "../openFileExplorer/openFileExplorer.h"
#include "../../devices/serial/serial.h"


namespace SysApps
{


    MemUsageShower::MemUsageShower()
    {
        AddToStack();
        //path = StrCopy("");

        //window =

        window = (Window*)_Malloc(sizeof(Window), "Mem Usage Shower Window");
        GuiInstance* gui = new GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(300, 200), Position(100, 100), "RAM Usage", true, true, true);
        osData.windows.InsertAt(window, 0);
        window->hidden = true;
        //window->resizeable = false;
        gui->Init();

        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;
        window->OnResizeHelp = (void*)this;
        window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;
        window->OnPartRenderHelp = (void*)this;
        window->OnPartRender = (void(*)(void*, Window*))(void*)&OnExternalWindowFrame;


        window->oldHidden = true;
        window->hidden = false;
        osData.windowsToGetActive.Enqueue(window);

        guiInstance = gui;

        guiInstance->screen->backgroundColor = Colors.white;




        guiInstance->CreateComponentWithId(1400, GuiComponentStuff::ComponentType::CANVAS);
        canvas = (GuiComponentStuff::CanvasComponent*)guiInstance->GetComponentFromId(1400);
        canvas->position.x = 0;
        canvas->position.y = 0;
        canvas->size.IsXFixed = false;
        canvas->size.ScaledX = 1;
        canvas->size.IsYFixed = false;
        canvas->size.ScaledY = 1;
        canvas->bgColor = Colors.white;
        
        canvas->Clear();
        canvas->CheckUpdates();

        canvas->DrawText(0, 0, Colors.black, 1, "Hoi!");
        canvas->DrawText(8, 16, Colors.brown, 2, "Hello, world!");

        lastTimeMS = PIT::TimeSinceBootMS();
        
        wantedListCount = 30;
        memUsageList = new List<int64_t>(wantedListCount);
        for (int i = 0; i < wantedListCount; i++)
            memUsageList->Add(usedHeapAmount);
        
        DoRender();
        RemoveFromStack();
    }

    void MemUsageShower::DoRender()
    {
        //Serial::Writeln("> START FRAME");
        canvas->Clear();

        if (memUsageList->GetCount() < 2)
            return;

        int canvasW = canvas->renderer->componentFrameBuffer->Width;
        int canvasH = canvas->renderer->componentFrameBuffer->Height;

        if (canvasW < 1 || canvasH < 1)
            return;
        
        int64_t max = memUsageList->ElementAt(0);
        int64_t min = memUsageList->ElementAt(0);
        for (int i = 1; i < memUsageList->GetCount(); i++)
        {
            int64_t val = memUsageList->ElementAt(i);
            if (val > max)
                max = val;
            if (val < min)
                min = val;
        }

        

        // Draw graph of ram usage over time using the canvas
        
        int64_t stepDiv = 0x400;
        //Serial::Writeln("> STEP DIV: {}", to_string(stepDiv));


        max = (max * 110) / 100;
        min = (min * 90) / 100;
        max = (max * stepDiv + (stepDiv/2)) / stepDiv;
        min = (min * stepDiv) / stepDiv;
        if (min < 0)
            min = 0;

        int maxMin = max - min;
        if (maxMin == 0)
            return;

        int lX, lY, lX2;
        {
            int64_t val = memUsageList->ElementAt(0);
            lX = 0;
            lX2 = 0;
            lY = ((val - min) * canvasH) / maxMin;
            lY = canvasH - lY - 1;
        }

        for (int i = 0; i < wantedListCount; i++)
        {
            int64_t val = memUsageList->ElementAt(i);
            int x = (i * canvasW) / (wantedListCount - 1);
            int y = ((val - min) * canvasH) / maxMin;
            y = canvasH - y - 1;
            if (y != lY || i == wantedListCount - 1)
            {
                if (lX != lX2)
                    canvas->DrawLine(lX, lY, lX2, lY, Colors.bblue, 1);

                if (y != lY)
                    canvas->DrawLine(lX2, lY, x, y, Colors.bred, 1);
                else
                    canvas->DrawLine(lX2, lY, x, y, Colors.bblue, 1);

                lX = x;
                lY = y;
            }

            lX2 = x;
        }


        // stepDiv = 0x400;
        // while (maxMin > stepDiv)
        //     stepDiv <<= 4;
        // stepDiv >>= 5;
        // if (stepDiv < 1)
        //     stepDiv = 1;

        // // draw lines with dividor
        // //Serial::Writeln("> Drawing Lines");
        // for (int y = min; y < max; y += stepDiv)
        // {
        //     int y2 = (y / stepDiv) * stepDiv;
        //     int y3 = ((y2 - min) * canvasH) / maxMin;
        //     y3 = canvasH - y3 - 1;
        //     //Serial::Writeln("  > Y: {}", to_string(y3));
        //     canvas->DrawLine(0, y3, canvasW - 1, y3, Colors.bgray, 1);
        // }
        // //Serial::Writeln();

        // write text with min, avg and max values for reference
        // at bottom, middle, top

        //Serial::Writeln("> WRITE TEXT");
        // MAX
        {
            int tX = 0;
            int tY = 0; // top
            const char* temp;

            temp = "Max: ";
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);
            
            temp = to_string(max / 0x1000);
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);

            temp = " KB";
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);
        }

        // AVG
        {
            int tX = 0;
            int tY = canvasH / 2 + 16; // middle below the line hopefully
            const char* temp;

            temp = "Avg: ";
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);

            int64_t avg = 0;
            for (int i = 0; i < wantedListCount; i++)
                avg += memUsageList->ElementAt(i);
            avg /= wantedListCount;

            temp = to_string(avg / 0x1000);
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);

            temp = " KB";
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);
        }

        // MIN
        {
            int tX = 0;
            int tY = canvasH - 16; // bottom
            const char* temp;

            temp = "Min: ";
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);

            temp = to_string(min / 0x1000);
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);

            temp = " KB";
            canvas->DrawText(tX, tY, Colors.black, 1, temp);
            tX += 8 * StrLen(temp);
        }

        //Serial::Writeln("> END FRAME");
        canvas->UpdateCanvas();
    }

    void MemUsageShower::OnExternalWindowFrame(Window* window)
    {
        int64_t currTimeMS = PIT::TimeSinceBootMS();
        if (currTimeMS < lastTimeMS + 500)
            return;
        lastTimeMS = currTimeMS;

        while (memUsageList->GetCount() - 1 < wantedListCount)
            memUsageList->Add(usedHeapAmount);
        while (memUsageList->GetCount() > wantedListCount)
            memUsageList->RemoveAt(0);

        if (memUsageList->GetCount() < 1)
            return;

        DoRender();
    }

   
    void MemUsageShower::OnExternalWindowClose(Window* window)
    {
        Free();
    }

    void MemUsageShower::OnExternalWindowResize(Window* window)
    {
        int h = window->size.height;
        if (h < 40)
            h = 40;
        int w = window->size.width;
        if (w < 100)
            w = 100;
        window->newSize = Size(w, h);
        DoRender();
    }

    void MemUsageShower::Free()
    {
        AddToStack();

        if (memUsageList != NULL)
        {
            memUsageList->Free();
            _Free(memUsageList);
            memUsageList = NULL;
        }

        _Free(this);
        RemoveFromStack();
    }

}