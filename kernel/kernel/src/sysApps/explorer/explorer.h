#pragma once
#include "../../WindowStuff/Window/window.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiInstance.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/box/boxComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/button/buttonComponent.h"

#include "../../cStdLib/list/list_guiBtn.h"

namespace SysApps
{
    class Explorer
    {
        public:
        Window* window;
        GuiInstance* guiInstance;
        GuiComponentStuff::BaseComponent* lastClickedComp;
        GuiComponentStuff::BoxComponent* fileListComp;
        GuiComponentStuff::TextFieldComponent* pathComp;
        GuiComponentStuff::ButtonComponent* goUpBtn;
        List<GuiComponentStuff::ButtonComponent*> folderCompsYes = List<GuiComponentStuff::ButtonComponent*>(4);
        List<const char*> folderPathsYes = List<const char*>(4);
        List<GuiComponentStuff::ButtonComponent*> driveCompsYes = List<GuiComponentStuff::ButtonComponent*>(4);
        List<const char*> drivePathsYes = List<const char*>(4);
        List<GuiComponentStuff::ButtonComponent*> fileCompsYes = List<GuiComponentStuff::ButtonComponent*>(4);
        List<const char*> filePathsYes = List<const char*>(4);
        
        const char* path;
        int ScrollY;


        public:

        void* OnFileClickHelp;
        void (*OnFileClickCallback)(void* bruh, const char* path);

        Explorer();
        void UpdateSizes();
        const char* GetPath();
        void SetPath(const char* path);
        void Reload();

        void ClickCallBack(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo event);
        bool PathTypeCallBack(GuiComponentStuff::TextFieldComponent* comp, GuiComponentStuff::KeyHitEventInfo event);

        void OnExternalWindowClose(Window* window);
        void OnExternalWindowResize(Window* window);

        void OnFolderClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnFileClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnDriveClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
        void OnGoUpClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);

        void Free();
        void ClearLists();
    };



}