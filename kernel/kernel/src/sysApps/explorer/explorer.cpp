#include "explorer.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"
#include "../../WindowStuff/SubInstances/guiInstance/guiStuff/components/textField/textFieldComponent.h"

namespace SysApps
{
    Explorer::Explorer()
    {
        path = StrCopy("/");

        //window =

        Window* window = (Window*)_Malloc(sizeof(Window), "Explorer Window");
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
        GuiComponentStuff::TextFieldComponent* txtSearchField = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
        txtSearchField->position.x = 0;
        txtSearchField->position.y = 0;

        txtSearchField->size.FixedX = 500;
        txtSearchField->size.FixedY = 16;
        
        
        
        Reload();

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

    }
    
}