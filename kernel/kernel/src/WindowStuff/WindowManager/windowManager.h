#pragma once

#include "../Window/window.h"
//#include "../../osData/osData.h"
#include "../../rendering/Cols.h"

namespace WindowManager
{
    Window *getWindowAtMousePosition();
    Window *getWindowAtMousePosition(int distance);

    enum WindowAttributeType : int32_t
    {
        BLEH
    };

    Window* GetWindowById(int64_t id);
    bool DeleteWindowWithId(int64_t id);
    bool CreateWindowWithId(int64_t id);
    uint64_t GetWindowAttributeWithId(int64_t id, WindowAttributeType type);
    bool SetWindowAttributeWithId(int64_t id, WindowAttributeType type, uint64_t value);
    


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
        TERMINAL = 3,
        WARNING = 4,
        CRASH = 5,
        GUI_PGM = 6,
        TESTO_PGM = 7;

    } windowIconEnum;

    static const int countOfWindowIcons = 8;

    static const char* windowIconNames[countOfWindowIcons] = {
        "debug.mbif",
        "generic.mbif",
        "task_manager.mbif",
        "terminal.mbif",
        "warning.mbif",
        "crash.mbif",
        "gui.mbif",
        "testo_pgm.mbif"
    };



    extern Window* currentActionWindow;
    extern uint8_t testInterlace;

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
        void Resize(Framebuffer* actualScreenBuffer);

        // uint32_t* GetPixelAt(int x, int y);
        void DrawBGRect(int x1, int y1, int x2, int y2);
        void DrawTaskbarRect(int x1, int y1, int x2, int y2);
        void UpdatePointerRect(int x1, int y1, int x2, int y2);
        void UpdateWindowRect(Window *window);
        void RenderWindow(Window *window);
        //inline uint64_t RenderActualSquare(int _x1, int _y1, int _x2, int _y2);
        void RenderWindowRect(Window *window, int x1, int y1, int x2, int y2);
        void RenderWindows();
        void Clear(bool resetGlobal);
        void Render();
        void UpdateWindowBorder(Window *window);
    
        uint64_t RenderActualSquare(int _x1, int _y1, int _x2, int _y2);
    };
}