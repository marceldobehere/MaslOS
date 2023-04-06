#include "imgTest.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"


namespace SysApps
{
    ImageTest::ImageTest(const char* path)
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

        window->oldHidden = true;
        window->hidden = false;
        osData.windowsToGetActive.add(window);

        guiInstance = gui;

        guiInstance->screen->backgroundColor = Colors.white;

        guiInstance->CreateComponentWithId(1010, GuiComponentStuff::ComponentType::IMAGE_RECT);
        GuiComponentStuff::ImageRectangleComponent* imgComp = (GuiComponentStuff::ImageRectangleComponent*)guiInstance->GetComponentFromId(1010);

        imgComp->position.x = 0;
        imgComp->position.y = 0;

        imgComp->size.FixedX = 50;
        imgComp->size.FixedY = 50;
        window->newSize = Size(50, 50);


        imgComp->size.ScaledX = 1;
        imgComp->size.ScaledY = 1;
        imgComp->size.IsXFixed = false;
        imgComp->size.IsYFixed = false;

        _Free(imgComp->imagePath);
        imgComp->imagePath = StrCopy(path);

        imgComp->GetImageFromPath(imgComp->imagePath);

        if (imgComp->image != NULL)
        {
            // imgComp->size.FixedX = imgComp->image->width;
            // imgComp->size.FixedY = imgComp->image->height;
            // imgComp->size.IsXFixed = true;
            // imgComp->size.IsYFixed = true;
            window->newSize = Size(imgComp->image->width, imgComp->image->height);
        }

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

    void ImageTest::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void ImageTest::Free()
    {
        AddToStack();

        //drivePathsYes.free();

        _Free(this);
        RemoveFromStack();
    }

}