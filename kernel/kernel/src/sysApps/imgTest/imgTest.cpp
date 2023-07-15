#include "imgTest.h"
#include "../../cStdLib/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"
#include "../openFileExplorer/openFileExplorer.h"


namespace SysApps
{
    ImageViewer::ImageViewer(const char* path)
    {
        AddToStack();
        //path = StrCopy("");

        //window =

        window = (Window*)_Malloc(sizeof(Window), "Image Test Window");
        GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "Image Test GUI");
        *gui = GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(400, 300), Position(100, 100), "Image Viewer", true, true, true);
        osData.windows.insertAt(window, 0);
        window->hidden = true;
        //window->resizeable = false;
        gui->Init();

        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;
        window->OnResizeHelp = (void*)this;
        window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;

        window->oldHidden = true;
        window->hidden = false;
        osData.windowsToGetActive.add(window);

        guiInstance = gui;

        guiInstance->screen->backgroundColor = Colors.white;

        guiInstance->CreateComponentWithId(1010, GuiComponentStuff::ComponentType::IMAGE_RECT);
        testImg1 = (GuiComponentStuff::ImageRectangleComponent*)guiInstance->GetComponentFromId(1010);

        testImg1->position.x = 0;
        testImg1->position.y = 20;

        testImg1->size.FixedX = 100;
        testImg1->size.FixedY = 100;
        window->newSize = Size(100, 120);

        _Free(testImg1->imagePath);
        testImg1->imagePath = StrCopy(path);

        testImg1->GetImageFromPath(testImg1->imagePath);

        if (testImg1->image != NULL)
        {
            testImg1->size.FixedX = testImg1->image->width;
            testImg1->size.FixedY = testImg1->image->height;
            testImg1->size.IsXFixed = true;
            testImg1->size.IsYFixed = true;
            window->newSize = Size(testImg1->image->width, testImg1->image->height + 20);
        }

        guiInstance->CreateComponentWithId(1011, GuiComponentStuff::ComponentType::BUTTON);
        openBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1011);
        openBtn->position.x = 0;
        openBtn->position.y = 0;
        _Free(openBtn->textComp->text);
        openBtn->textComp->text = StrCopy("Open");
        openBtn->size.FixedX = 50;
        openBtn->size.FixedY = 20;
        openBtn->OnMouseClickHelp = (void*)this;
        openBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnOpenClick;

        // guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::TEXTFIELD);
        // pathComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
        // pathComp->position.x = 5*8 + 4;
        // pathComp->position.y = 0;

        // pathComp->AdvancedKeyHitCallBackHelp = (void*)this;
        // pathComp->AdvancedKeyHitCallBack = (bool(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::KeyHitEventInfo))(void*)&PathTypeCallBack;
        // _Free(pathComp->textComp->text);
        // pathComp->textComp->text = StrCopy(path);
        RemoveFromStack();
    }

    void ImageViewer::OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        OpenFileExplorer* exp = new OpenFileExplorer();
        
        guiInstance->OnWaitTaskDoneHelp = (void*)this;
        guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
        guiInstance->waitTask = exp->dataTask;
    }

    void ImageViewer::OnTaskDone(Task* task)
    {
        TaskSimpleData* tsk = (TaskSimpleData*)task;
        if (tsk->data != NULL)
        {
            const char* path = (const char*)tsk->data;

            _Free(testImg1->imagePath);
            testImg1->imagePath = StrCopy(path);

            testImg1->GetImageFromPath(testImg1->imagePath);

            testImg1->size.FixedX = 100;
            testImg1->size.FixedY = 100;
            window->newSize = Size(100, 120);

            if (testImg1->image != NULL)
            {
                testImg1->size.FixedX = testImg1->image->width;
                testImg1->size.FixedY = testImg1->image->height;
                testImg1->size.IsXFixed = true;
                testImg1->size.IsYFixed = true;
                window->newSize = Size(testImg1->image->width, testImg1->image->height + 20);
            }

            _Free(path);
        }
    }

    void ImageViewer::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void ImageViewer::OnExternalWindowResize(Window* window)
    {
        int h = window->size.height;
        if (h < 20)
            h = 20;
        testImg1->size.FixedY = h - 20;
        testImg1->size.FixedX = window->size.width;
    }

    void ImageViewer::Free()
    {
        AddToStack();

        //drivePathsYes.free();

        _Free(this);
        RemoveFromStack();
    }

}