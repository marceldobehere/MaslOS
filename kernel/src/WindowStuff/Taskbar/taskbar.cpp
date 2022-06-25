#include "taskbar.h"
#include "../../OSDATA/osdata.h"

namespace Taskbar
{
    Framebuffer* taskbarBuffer;
    kernelFiles::ImageFile* MButton;
    kernelFiles::ImageFile* MButtonS;

    void InitTaskbar(kernelFiles::ImageFile* mButton, kernelFiles::ImageFile* mButtonS)
    {
        MButton = mButton;
        MButtonS = mButtonS;
        taskbarBuffer = osData.windowPointerThing->taskbar;
    }

    void RenderTaskbar()
    {
        {
            uint32_t* endAddr = (uint32_t*)((uint64_t)taskbarBuffer->BaseAddress + taskbarBuffer->BufferSize);
            uint32_t col = osData.windowPointerThing->defaultTaskbarBackgroundColor;
            for (uint32_t* addr = (uint32_t*)taskbarBuffer->BaseAddress; addr < endAddr; addr++)
                *addr = col;
        }
        int64_t ypos = osData.windowPointerThing->virtualScreenBuffer->Height - taskbarBuffer->Height;

        if (MousePosition.x >= 0 && MousePosition.x < 40 && MousePosition.y >= ypos && MousePosition.y < ypos + 40)
            GlobDrawImage(MButtonS, 0, 0, 1, 1, taskbarBuffer);
        else
            GlobDrawImage(MButton, 0, 0, 1, 1, taskbarBuffer);
        
        //GlobDrawImage(currentMouseImage, 10, 10, 1, 1, taskbarBuffer);





        osData.windowPointerThing->UpdatePointerRect(0, ypos, osData.windowPointerThing->virtualScreenBuffer->Width - 1, ypos + taskbarBuffer->Height - 1);
    }



    
}