#pragma once
#include "../../WindowStuff/Window/window.h"
// #include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
// #include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
// #include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"

namespace SysApps
{
    class Tetris
    {
        private:
        Window* window;
        
        const char* path;
        int ScrollY;
        uint64_t lastSleep;


        public:
        Tetris();

        void Reload();

        // void ClickCallBack(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo event);
        // bool PathTypeCallBack(GuiComponentStuff::TextFieldComponent* comp, GuiComponentStuff::KeyHitEventInfo event);

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);
        void OnExternalWindowRender(Window* window);
        void Free();
    };


    // https://github.com/Gregwar/ASCII-Tetris/blob/master/tetris.h

    struct tetris;
    void tetris_cleanup_io();
    void tetris_signal_quit(int);
    void tetris_set_ioconfig();
    void tetris_init(struct tetris *t,int w,int h);
    void tetris_clean(struct tetris *t);
    void tetris_print(struct tetris *t);
    
    // void tetris_run(int width, int height);

    void tetris_run_init(int width, int height);
    void tetris_run_loop();
    void tetris_run_end();


    void tetris_new_block(struct tetris *t);
    void tetris_new_block(struct tetris *t);
    void tetris_print_block(struct tetris *t);
    void tetris_rotate(struct tetris *t);
    void tetris_gravity(struct tetris *t);
    void tetris_fall(struct tetris *t, int l);
    void tetris_check_lines(struct tetris *t);
    int tetris_level(struct tetris *t);

}