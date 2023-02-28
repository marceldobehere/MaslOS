#include "explorer.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"

namespace SysApps
{
    Explorer::Explorer()
    {
        path = StrCopy("/");

        //window =

        window = (Window*)_Malloc(sizeof(Window), "Explorer Window");
        GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "Explorer GUI");
        *gui = GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(400, 300), Position(100, 100), "Explorer", true, true, true);
        osData.windows.insertAt(window, 0);
        window->hidden = true;
        gui->Init();

        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;

        window->moveToFront = true;
        window->oldHidden = true;
        window->hidden = false;
        activeWindow = window;

        guiInstance = gui;
        lastClickedComp = NULL;

        guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::TEXTFIELD);
        pathComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
        pathComp->position.x = 0;
        pathComp->position.y = 0;

        pathComp->AdvancedKeyHitCallBackHelp = (void*)this;
        pathComp->AdvancedKeyHitCallBack = (bool(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::KeyHitEventInfo))(void*)&PathTypeCallBack;

        guiInstance->CreateComponentWithId(1022, GuiComponentStuff::ComponentType::BOX);
        fileListComp = (GuiComponentStuff::BoxComponent*)guiInstance->GetComponentFromId(1022);
        fileListComp->position.x = 0;
        fileListComp->position.y = 20;

        UpdateSizes();

        //Reload();

    }

    void Explorer::UpdateSizes()
    {
        fileListComp->size.FixedX = window->size.width;
        fileListComp->size.FixedY = window->size.height - 25;
        fileListComp->backgroundColor = Colors.white;
        pathComp->size.FixedX = window->size.width;
        pathComp->size.FixedY = 16;
    }

    const char* Explorer::GetPath()
    {
        return StrCopy(path);
    }
    void Explorer::SetPath(const char* path)
    {
        _Free(path);
        this->path = StrCopy(path);
    }

    bool Explorer::PathTypeCallBack(GuiComponentStuff::TextFieldComponent* comp, GuiComponentStuff::KeyHitEventInfo event)
    {
        if (event.Chr == '\n')
        {
            GlobalRenderer->Clear(Colors.orange);
            return false;
        }
        return true;
    }

    void Explorer::ClickCallBack(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo event)
    {
        GlobalRenderer->Clear(Colors.blue);
    }

    void Explorer::OnExternalWindowClose(Window* window)
    {
        GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void Explorer::Free()
    {
        _Free(path);
        _Free(this);
    }

    void Explorer::Reload()
    {
        UpdateSizes();

    }
    
}