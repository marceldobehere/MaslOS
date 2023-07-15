#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/imageRect/imageRectangleComponent.h"
#include "../../audio/audio.h"

namespace SysApps
{

    class MusicPlayer
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        //GuiComponentStuff::ImageRectangleComponent* testImg1;
        GuiComponentStuff::ButtonComponent* openBtn;
        GuiComponentStuff::ButtonComponent* playBtn;
        GuiComponentStuff::TextComponent* pathText;
        GuiComponentStuff::TextComponent* timeText;

        Audio::BasicAudioSource* musicSource;
        char* musicFileData;
        int musicFileLen;
        const int waveDataOffset = 44;
        int musicFilePos;
        bool paused;

        public:
        const char* musicPath;
        MusicPlayer(const char* path);

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);
        void OnExternalWindowFrame(Window* window);

        void OnBufferFinish(Audio::BasicAudioDestination* dest);
        void HandleMusic();

        void OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnPlayClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnTaskDone(Task* task);
        void LoadFileBuff(void* tsk);
        void LoadFile(const char* path);


        void Free();
    };



}