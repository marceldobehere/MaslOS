#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"

namespace SysApps
{
    enum NotePadButtonTaskState
    {
        None,
        Save,
        Load
    };
    class Notepad
    {
        private:
        Window* window;
        GuiInstance* guiInstance;
        NotePadButtonTaskState btnTaskState;


        // GuiComponentStuff::BaseComponent* lastClickedComp;
        // GuiComponentStuff::BoxComponent* fileListComp;
        // GuiComponentStuff::TextFieldComponent* pathComp;
        // GuiComponentStuff::ButtonComponent* goUpBtn;
    
        GuiComponentStuff::ButtonComponent* loadBtn;   
        GuiComponentStuff::ButtonComponent* saveBtn;
        GuiComponentStuff::ButtonComponent* saveAsBtn;
        GuiComponentStuff::TextFieldComponent* textComp;


        // List<void*> folderCompsYes = List<void*>(4);
        // List<const char*> folderPathsYes = List<const char*>(4);
        // List<void*> driveCompsYes = List<void*>(4);
        // List<const char*> drivePathsYes = List<const char*>(4);
        // List<void*> fileCompsYes = List<void*>(4);
        // List<const char*> filePathsYes = List<const char*>(4);
        
        const char* lastSavePath;
        int ScrollY;


        public:
        Notepad();
        void UpdateSizes();
        //void Reload();

        //void ClickCallBack(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo event);
        bool PathTypeCallBack(GuiComponentStuff::TextFieldComponent* comp, GuiComponentStuff::KeyHitEventInfo event);

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);

        void OnSaveClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnSaveAsClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnLoadClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);

        void OnTaskDone(Task* task);
        void SaveInto(const char* path);
        void LoadFrom(const char* path);

        // void OnFolderClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        // void OnFileClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        // void OnDriveClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        // void OnGoUpClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);

        void Free();
        //void ClearLists();
    };



}