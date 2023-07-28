#include "magnifier.h"
#include "../../cStdLib/cStdLib.h"


namespace SysApps
{
    Magnifier::Magnifier()
    {
        AddToStack();
        window = (Window*)_Malloc(sizeof(Window), "Magnifier Window");
        zoom = 200;
        keyPressed = false;
        
        *(window) = Window(NULL, Size(400, 300), Position(100, 100), "Magnifier", true, true, true);
        osData.windows.InsertAt(window, 0);
        window->hidden = true;


        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;
        window->OnResizeHelp = (void*)this;
        window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;
        window->OnPartRenderHelp = (void*)this;
        window->OnPartRender = (void(*)(void*, Window*))(void*)&OnExternalWindowFrame;

        window->oldHidden = true;
        window->hidden = false;
        osData.windowsToGetActive.Enqueue(window);

        UpdateTitle();
        Render();
       
        RemoveFromStack();
    }

   

    void Magnifier::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void Magnifier::OnExternalWindowResize(Window* window)
    {
        int h = window->size.height;
        if (h < 40)
            h = 40;
        int w = window->size.width;
        if (w < 40)
            w = 40;
        window->newSize = Size(w, h);
    }

    #define KEY_PLUS 0x0D
    #define KEY_MINUS 0x0C
    #define KEY_CTRL Control

    void Magnifier::OnExternalWindowFrame(Window* window)
    {
        AddToStack();
        HandleKeys();
        Render();
        RemoveFromStack();
    }

    static const int zoomstep = 25;

    void Magnifier::UpdateTitle()
    {
        _Free(window->title);
        window->title = StrAppend("Magnifier - Zoom: ", to_string(zoom), false);
        window->title = StrAppend(window->title, "%", true);
        osData.windowPointerThing->UpdateWindowBorder(window);
    }

    void Magnifier::HandleKeys()
    {
        if (activeWindow != window)
            return;
        
        if (!keyPressed)
        {
            // check for ctrl and +
            if (KeyboardScancodeState[KEY_CTRL] && KeyboardScancodeState[KEY_PLUS]) // maybe 117
            {
                keyPressed = true;
                zoom += zoomstep;
                if (zoom > 1600)
                    zoom = 1600;
                UpdateTitle();
            }
            // check for ctrl and -
            else if (KeyboardScancodeState[KEY_CTRL] && KeyboardScancodeState[KEY_MINUS])
            {
                keyPressed = true;
                zoom -= zoomstep;
                if (zoom < zoomstep)
                    zoom = zoomstep;
                UpdateTitle();
            }
        }

        if (keyPressed)
        {
            if (!KeyboardScancodeState[KEY_PLUS] && 
                !KeyboardScancodeState[KEY_MINUS])
                keyPressed = false;
        }
    }

    const static uint32_t OutOfBoundsCols[16] = 
    {
        Colors.black,
        Colors.bblue, // LEFT
        Colors.bgreen, // DOWN
        Colors.cyan, // LEFT + DOWN
        Colors.yellow, // RIGHT
        Colors.black,
        Colors.lime, // RIGHT + DOWN
        Colors.black, 
        Colors.bred, // UP
        Colors.pink, // UP + LEFT
        Colors.black,
        Colors.black,
        Colors.orange, // UP + RIGHT
        Colors.black,
        Colors.black,
        Colors.black
    };

    void Magnifier::Render()
    {
        PointerFramebuffer* scr = osData.windowPointerThing->virtualScreenBuffer;
        //Framebuffer* scr = osData.windowPointerThing->copyOfScreenBuffer;
        Framebuffer* win = window->framebuffer;
        int sW = scr->Width;
        int sH = scr->Height;
        int wW = win->Width;
        int wH = win->Height;
        int zoomVal = zoom * 2;

        MPoint mp = MousePosition;
        for (int tY = 0; tY < wH; tY++)
        {
            for (int tX = 0; tX < wW; tX++)
            {
                int x = ((mp.x * zoomVal) + (tX * 2 - wW) * 100) / zoomVal;
                int y = ((mp.y * zoomVal) + (tY * 2 - wH) * 100) / zoomVal;

                uint32_t col = Colors.black;
                if (x >= 0 && x < sW && y >= 0 && y < sH)
                    col = *((uint32_t**)scr->BaseAddress)[y * sW + x];
                else
                {
                    int indx = 0;
                    if (x < 0)
                        indx |= 0b0001;
                    if (x >= sW)
                        indx |= 0b0100;
                    if (y < 0)
                        indx |= 0b1000;
                    if (y >= sH)
                        indx |= 0b0010;
                    if (indx < 0 || indx > 16)
                        indx = 0;
                    col = OutOfBoundsCols[indx];
                }
                
                ((uint32_t*)win->BaseAddress)[tY * win->PixelsPerScanLine + tX] = col;
            }
        }
    }

    void Magnifier::Free()
    {
        AddToStack();
        _Free(this);
        RemoveFromStack();
    }

}