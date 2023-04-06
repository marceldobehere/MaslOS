#include "openFileExplorer.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"
#include "../../tasks/closeWindow/taskWindowClose.h"



namespace SysApps
{
    OpenFileExplorer::OpenFileExplorer()
    {
        explorer = new Explorer();
        window = explorer->window;
        _Free(window->title);
        window->title = StrCopy("Select File to open");
        explorer->OnFileClickHelp = (void*)this;
        explorer->OnFileClickCallback = (void(*)(void*, const char*))(void*)&OnFileClick;


        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;

        dataTask = NewSimpleDataTask();

        //path = StrCopy("");

        //window =

        // window = (Window*)_Malloc(sizeof(Window), "Explorer Window");
        // GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "Explorer GUI");
        // *gui = GuiInstance(window);
        // *(window) = Window((DefaultInstance*)gui, Size(400, 300), Position(100, 100), "Explorer", true, true, true);
        // osData.windows.insertAt(window, 0);
        // window->hidden = true;
        // //window->resizeable = false;
        // gui->Init();

        // window->OnCloseHelp = (void*)this;
        // window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;

        // window->OnResizeHelp = (void*)this;
        // window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;

        // window->oldHidden = true;
        // window->hidden = false;
        // osData.windowsToGetActive.add(window);

        // guiInstance = gui;
        // lastClickedComp = NULL;

        // guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::TEXTFIELD);
        // pathComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
        // pathComp->position.x = 5*8 + 4;
        // pathComp->position.y = 0;

        // pathComp->AdvancedKeyHitCallBackHelp = (void*)this;
        // pathComp->AdvancedKeyHitCallBack = (bool(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::KeyHitEventInfo))(void*)&PathTypeCallBack;
        // _Free(pathComp->textComp->text);
        // pathComp->textComp->text = StrCopy(path);

        // guiInstance->CreateComponentWithId(1023, GuiComponentStuff::ComponentType::BUTTON);
        // goUpBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1023);
        // goUpBtn->position.x = 0;
        // goUpBtn->position.y = 0;
        // _Free(goUpBtn->textComp->text);
        // goUpBtn->textComp->text = StrCopy("Go Up");
        // goUpBtn->size.FixedY = 16;
        // goUpBtn->size.FixedX = 5*8;
        // goUpBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnGoUpClick;
        // goUpBtn->OnMouseClickHelp = this;


        // guiInstance->CreateComponentWithId(1022, GuiComponentStuff::ComponentType::BOX);
        // fileListComp = (GuiComponentStuff::BoxComponent*)guiInstance->GetComponentFromId(1022);
        // fileListComp->position.x = 0;
        // fileListComp->position.y = 20;

        // UpdateSizes();

        // Reload();

    }


    void OpenFileExplorer::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);

        dataTask->data = NULL;
        dataTask->SetDone();

        Free();
    }

    void OpenFileExplorer::Free()
    {
        AddToStack();

        if (explorer != NULL)
            explorer->OnExternalWindowClose(window);
        explorer = NULL;
        
        _Free(this);
        RemoveFromStack();
    }


    void OpenFileExplorer::OnFileClick(const char* path)
    {
        AddToStack();

        dataTask->data = (void*)StrCopy(path);
        dataTask->SetDone();

        window->OnCloseHelp = (void*)explorer;
        window->OnClose = (void(*)(void*, Window*))(void*)&Explorer::OnExternalWindowClose;
        osData.osTasks.add(NewWindowCloseTask(window));

        _Free(this);
        RemoveFromStack();
    }




}