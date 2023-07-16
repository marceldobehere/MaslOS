#pragma once
#include "../../WindowStuff/Window/window.h"

namespace SysApps
{

    class Magnifier
    {
        private:
        Window* window;

        public:
        Magnifier();
        int zoom = 100;
        bool keyPressed;

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);
        void OnExternalWindowFrame(Window* window);
        void Render();
        void HandleKeys();
        void UpdateTitle();

        void Free();
    };



}