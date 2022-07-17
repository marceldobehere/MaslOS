#include "taskbar.h"
#include "../../OSDATA/osdata.h"


namespace Taskbar
{
    Framebuffer* taskbarBuffer;
    BasicRenderer* renderer;
    kernelFiles::ImageFile* MButton;
    kernelFiles::ImageFile* MButtonS;
    uint32_t backgroundColor;
    uint32_t defaultTabBackgroundColor;
    uint32_t selectedTabBackgroundColor;
    uint32_t defaultTabTextColor;
    uint32_t selectedTabTextColor;
    SyncedList<Window*>* taskWindowList;
    Window* activeTabWindow;

    int8_t Scounter = 0;

    void InitTaskbar(kernelFiles::ImageFile* mButton, kernelFiles::ImageFile* mButtonS)
    {
        AddToStack();
        MButton = mButton;
        MButtonS = mButtonS;
        taskbarBuffer = osData.windowPointerThing->taskbar;
        renderer = (BasicRenderer*)malloc(sizeof(BasicRenderer));
        *renderer = BasicRenderer(taskbarBuffer, GlobalRenderer->psf1_font);
        
        taskWindowList = (SyncedList<Window*>*)malloc(sizeof(SyncedList<Window*>));
        *taskWindowList = SyncedList<Window*>(&osData.windows);

        backgroundColor =            0xff001530;
        defaultTabBackgroundColor =  0xff002299;
        selectedTabBackgroundColor = 0xff0030BF;
        defaultTabTextColor =        0xffABABAB;
        selectedTabTextColor =       0xffFFFFFF;

        Scounter = 10;

        RemoveFromStack();
    }

    

    void RenderTaskbar()
    {
        Scounter++;
        if (Scounter % 3 == 0)
            return;
        else if (Scounter > 100)
            Scounter = 0;


        AddToStack();
        {
            uint32_t* endAddr = (uint32_t*)((uint64_t)taskbarBuffer->BaseAddress + taskbarBuffer->BufferSize);
            uint32_t col = backgroundColor;
            for (uint32_t* addr = (uint32_t*)taskbarBuffer->BaseAddress; addr < endAddr; addr++)
                *addr = col;
        }
        int64_t ypos = osData.windowPointerThing->virtualScreenBuffer->Height - taskbarBuffer->Height;

        if (MousePosition.x >= 0 && MousePosition.x < 40 && MousePosition.y >= ypos && MousePosition.y < ypos + 40)
            GlobDrawImage(MButtonS, 0, 0, 1, 1, taskbarBuffer);
        else
            GlobDrawImage(MButton, 0, 0, 1, 1, taskbarBuffer);
        
        //GlobDrawImage(currentMouseImage, 10, 10, 1, 1, taskbarBuffer);
        if (Scounter % 5 == 0)
            taskWindowList->sync();

        int64_t wCount = taskWindowList->getCount();
        int64_t size = 200;
        int64_t width = taskbarBuffer->Width;
        int64_t height = taskbarBuffer->Height;
        if (wCount != 0)
        {
            size = (width - 400) / wCount;
            if (size > 200)
                size = 200;
            if (size < 60)
                size = 60;
        }

        {
            int64_t x = 46;
            int iconSize = 24;
            int ydiff = ((height)-iconSize)/2;
            int textStart = iconSize + 12;
            Window* tempWindow = NULL;
            for (int i = 0; i < wCount; i++)
            {
                if (x + size > width)
                    break;

                Window* window = taskWindowList->elementAt(i);

                if (window == osData.debugTerminalWindow && !osData.showDebugterminal)
                    continue;


                bool hover = window == activeWindow;

                if (MousePosition.x >= x && MousePosition.x < (x + size) && MousePosition.y >= ypos && MousePosition.y < GlobalRenderer->framebuffer->Height)
                {
                    hover = true;
                    tempWindow = window;
                }

                if (hover)
                    renderer->Clear(x, 2, x + size, height - 3, selectedTabBackgroundColor); // Clear whole Rect
                else
                    renderer->Clear(x, 2, x + size, height - 3, defaultTabBackgroundColor); // Clear whole Rect
                
                bool drawIcon = (window->icon != NULL);
                if (drawIcon)
                    drawIcon = (window->icon->height == iconSize && window->icon->width == iconSize);

                if (drawIcon)
                    renderer->DrawImage(window->icon, x + 6, ydiff, 1, 1);
                else
                    renderer->Clear(x + 6, ydiff, x + 5 + iconSize, height - (ydiff+1), Colors.black); // Clear Icon Rect


                {
                    char* stitle = StrSubstr(window->title, 0, ((size-8) - textStart) / 8);

                    int strLen1 = ((size-8) - textStart) / 8;
                    int strLen2 = StrLen(window->title);

                    if (strLen1 < strLen2)
                    {
                        int amount = min(3, strLen1);
                        int index = strLen1 - 1;
                        for (int i = 0; i < amount; i++)
                        {
                            stitle[index] = '.';
                            index--;
                        }
                    }

                    if (hover)
                        renderer->color = selectedTabTextColor;
                    else
                        renderer->color = defaultTabTextColor;

                    renderer->putStr(stitle, x + textStart, (height/2)-6);
                    free((void*)stitle);
                }

                activeTabWindow = tempWindow;



                x += size + 6;
            }
        }


        osData.windowPointerThing->UpdatePointerRect(0, ypos, osData.windowPointerThing->virtualScreenBuffer->Width - 1, ypos + taskbarBuffer->Height - 1);
        RemoveFromStack();
    }


    // Window* GetItemAtMousePosition()
    // {
        


    // }

    
}