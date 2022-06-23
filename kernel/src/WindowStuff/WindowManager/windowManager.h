#pragma once

#include "../../customClasses/list.h"
//#include "../Window/window.h"  
//#include "../../OSDATA/osdata.h" 
#include "../../Cols.h"

namespace WindowManager
{
    Window* getWindowAtMousePosition();
    Window* getWindowAtMousePosition(int distance);

    class WindowPointerBufferThing
    {
        private:


        public:
            double fps;
            uint32_t defaultColor = Colors.black;
            uint32_t defaultBackgroundColor = defaultColor;
            Framebuffer* actualScreenBuffer;
            Framebuffer* copyOfScreenBuffer;
            Framebuffer* background;
            PointerFramebuffer* virtualScreenBuffer;
            PointerFramebuffer* copyOfVirtualBuffer;

            WindowPointerBufferThing(Framebuffer* actualScreenBuffer, Framebuffer* background, uint32_t backgroundColor);

            //uint32_t* GetPixelAt(int x, int y);
            void UpdatePointerRect(int x1, int y1, int x2, int y2);
            void RenderWindow(Window* window);
            void RenderWindowRect(Window* window, int x1, int y1, int x2, int y2);
            void RenderWindows();
            void Clear();
            void Render();
            void UpdateWindowBorder(Window* window);

    };  
}