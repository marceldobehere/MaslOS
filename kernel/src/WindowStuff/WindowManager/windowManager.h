#pragma once

#include "../../customClasses/list.h"
//#include "../Window/window.h"
//#include "../../OSDATA/osdata.h"
#include "../../Rendering/Cols.h"

namespace WindowManager
{
    Window *getWindowAtMousePosition();
    Window *getWindowAtMousePosition(int distance);

    static struct _windowButtonIconEnum
    {
        static const int 
        CLOSE_N = 0,
        CLOSE_S = 1,
        CLOSE_H = 2,

        MIN_N = 3,
        MIN_S = 4,
        MIN_H = 5,

        HIDE_N = 6,
        HIDE_S = 7,
        HIDE_H = 8;

    } windowButtonIconEnum;

    static const int countOfButtonIcons = 9;

    static const char* const windowButtonIconNames[countOfButtonIcons] = {
        "close_n.mbif", "close_s.mbif", "close_h.mbif", 
        "min_n.mbif",   "min_s.mbif",   "min_h.mbif", 
        "hide_n.mbif",  "hide_s.mbif",  "hide_h.mbif"
    };


    static struct _windowIconEnum
    {
        static const int 
        DEBUG = 0,
        DEFAULT = 1,
        TASK_MANAGER = 2,
        TERMINAL = 3;

    } windowIconEnum;

    static const int countOfWindowIcons = 4;

    static const char* const windowIconNames[countOfWindowIcons] = {
        "debug.mbif",
        "generic.mbif",
        "task_manager.mbif",
        "terminal.mbif"
    };



    extern Window* currentActionWindow;


    extern kernelFiles::ImageFile* windowButtonIcons[];

    extern kernelFiles::ImageFile* internalWindowIcons[];

    class WindowPointerBufferThing
    {
    private:
    public:

        //static const char** windowIconNames = {"", ""};
        
        uint64_t fps;
        uint32_t defaultColor = Colors.black;
        uint32_t defaultBackgroundColor = defaultColor;
        Framebuffer *actualScreenBuffer;
        Framebuffer *copyOfScreenBuffer;
        Framebuffer *background;
        Framebuffer *taskbar;
        PointerFramebuffer *virtualScreenBuffer;
        PointerFramebuffer *copyOfVirtualBuffer;

        WindowPointerBufferThing(Framebuffer *actualScreenBuffer, Framebuffer *background, uint32_t backgroundColor);

        // uint32_t* GetPixelAt(int x, int y);
        void DrawBGandTaskbarRect(int x1, int y1, int x2, int y2);
        void UpdatePointerRect(int x1, int y1, int x2, int y2);
        void UpdateWindowRect(Window *window);
        void RenderWindow(Window *window);
        void RenderWindowRect(Window *window, int x1, int y1, int x2, int y2);
        void RenderWindows();
        void Clear();
        void Render();
        void UpdateWindowBorder(Window *window);
    };
}