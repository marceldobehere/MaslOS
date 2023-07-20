#include "paint.h"
#include "../../cStdLib/cStdLib.h"
#include "../../memory/heap.h"
#include "../../osData/osData.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"
#include "../openFileExplorer/openFileExplorer.h"
#include "../../devices/serial/serial.h"


namespace SysApps
{

    static const uint32_t DefaultMSColors[28]
    { //0xAARRGGBB
        0xFF000000, // Black
        0xFF7F7F7F, // Dark Gray
        0xFF7E0102, // Dark Red
        0xFF7E8000, // Dark Yellow
        0xFF027F03, // Dark Green
        0xFF00807E, // Dark Light Blue
        0xFF040083, // Dark Blue
        0xFF7D0284, // Dark Magenta
        0xFF807E41,// Dark Lime
        0xFF024241, // Dark Cyan
        0xFF0380FE, // Dark Light Blue (2)
        0xFF013F83, // Dark Light Purple Blue
        0xFF4000FB, // Middle Dark Blue
        0xFF813D00, // Brown

        0xFFFFFFFF, // White
        0xFFBFBFBF, // Gray
        0xFFFA0201, // Red
        0xFFFBFF04, // Yellow
        0xFF04FF0B, // Green
        0xFF03FDFB, // Light Blue
        0xFF0403F7, // Blue
        0xFFFD01FC, // Magenta
        0xFFFFFB84, // Lime
        0xFF05FD7E, // Cyan
        0xFF7CFFFF, // Light Blue (2)
        0xFF7F7EFE, // Light Purple Blue
        0xFFFC0181, // Pink
        0xFFFE8040, // Orange
    };

    Paint::Paint()
    {
        AddToStack();
        //path = StrCopy("");

        //window =

        window = (Window*)_Malloc(sizeof(Window), "Paint Window");
        GuiInstance* gui = new GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(280, 400), Position(100, 100), "Paint", true, true, true);
        osData.windows.insertAt(window, 0);
        window->hidden = true;
        //window->resizeable = false;
        gui->Init();

        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;
        window->OnResizeHelp = (void*)this;
        window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;
        window->OnPartRenderHelp = (void*)this;
        window->OnPartRender = (void(*)(void*, Window*))(void*)&OnExternalWindowFrame;


        window->oldHidden = true;
        window->hidden = false;
        osData.windowsToGetActive.add(window);

        guiInstance = gui;

        guiInstance->screen->backgroundColor = Colors.white;

        oldMouseLeftState = false;
        oldMouseRightState = false;
        currMouseLeftState = false;
        currMouseRightState = false;
        currentColor = Colors.black;
        curSize = 2;


        guiInstance->CreateComponentWithId(1100, GuiComponentStuff::ComponentType::BOX);
        menuBox = (GuiComponentStuff::BoxComponent*)guiInstance->GetComponentFromId(1100);
        menuBox->position.x = 0;
        menuBox->position.y = 0;
        menuBox->size.IsXFixed = false;
        menuBox->size.ScaledX = 1;
        menuBox->size.IsYFixed = true;
        menuBox->size.FixedY = 20;
        menuBox->backgroundColor = Colors.bgray;

        guiInstance->CreateComponentWithIdAndParent(1101, GuiComponentStuff::ComponentType::BUTTON, menuBox->id);
        openBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1101);
        openBtn->position.x = 0;
        openBtn->position.y = 0;
        openBtn->size.IsXFixed = true;
        openBtn->size.FixedX = 50;
        openBtn->size.IsYFixed = true;
        openBtn->size.FixedY = 20;
        
        _Free(openBtn->textComp->text);
        openBtn->textComp->text = StrCopy("Open");

        openBtn->OnMouseClickHelp = (void*)this;
        openBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnOpenClick;


        guiInstance->CreateComponentWithIdAndParent(1102, GuiComponentStuff::ComponentType::BUTTON, menuBox->id);
        saveBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1102);
        saveBtn->position.x = 50;
        saveBtn->position.y = 0;
        saveBtn->size.IsXFixed = true;
        saveBtn->size.FixedX = 50;
        saveBtn->size.IsYFixed = true;
        saveBtn->size.FixedY = 20;

        _Free(saveBtn->textComp->text);
        saveBtn->textComp->text = StrCopy("Save");

        saveBtn->OnMouseClickHelp = (void*)this;
        saveBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnSaveClick;


        guiInstance->CreateComponentWithIdAndParent(1103, GuiComponentStuff::ComponentType::BUTTON, menuBox->id);
        saveAsBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1103);
        saveAsBtn->position.x = 100;
        saveAsBtn->position.y = 0;
        saveAsBtn->size.IsXFixed = true;
        saveAsBtn->size.FixedX = 74;
        saveAsBtn->size.IsYFixed = true;
        saveAsBtn->size.FixedY = 20;

        _Free(saveAsBtn->textComp->text);
        saveAsBtn->textComp->text = StrCopy("Save As");

        saveAsBtn->OnMouseClickHelp = (void*)this;
        saveAsBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnSaveAsClick;


        guiInstance->CreateComponentWithIdAndParent(1104, GuiComponentStuff::ComponentType::BUTTON, menuBox->id);
        clearBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1104);
        clearBtn->position.x = 174;
        clearBtn->position.y = 0;
        clearBtn->size.IsXFixed = true;
        clearBtn->size.FixedX = 50;
        clearBtn->size.IsYFixed = true;
        clearBtn->size.FixedY = 20;

        _Free(clearBtn->textComp->text);
        clearBtn->textComp->text = StrCopy("Clear");

        clearBtn->OnMouseClickHelp = (void*)this;
        clearBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnClearClick;


        guiInstance->CreateComponentWithId(1200, GuiComponentStuff::ComponentType::BOX);
        colorBox = (GuiComponentStuff::BoxComponent*)guiInstance->GetComponentFromId(1200);
        colorBox->position.x = 0;
        colorBox->position.y = 20;
        // colorBox->size.IsXFixed = true;
        // colorBox->size.FixedX = 320;
        colorBox->size.IsXFixed = false;
        colorBox->size.ScaledX = 1;
        colorBox->size.IsYFixed = true;
        colorBox->size.FixedY = 40;
        colorBox->backgroundColor = Colors.bgray;


        AddToStack();
        // add colors here
        int startIdForColorButtons = 1300;
        lastCol = NULL;
        for (int y = 0; y < 2; y++)
        {
            for (int x = 0; x < 14; x++)
            {
                //Serial::Writeln("Adding color button: {}", to_string(startIdForColorButtons));
                if (!guiInstance->CreateComponentWithIdAndParent(startIdForColorButtons, GuiComponentStuff::ComponentType::BUTTON, colorBox->id))
                {
                    x--;
                    continue;
                }

                GuiComponentStuff::ButtonComponent* colorBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(startIdForColorButtons);
                

            
                colorBtn->position.x = x * 20;
                colorBtn->position.y = y * 20;
                colorBtn->size.IsXFixed = true;
                colorBtn->size.FixedX = 20;
                colorBtn->size.IsYFixed = true;
                colorBtn->size.FixedY = 20;

                if (lastCol == NULL)
                {
                    lastCol = colorBtn;
                    lastCol->position.x += 2;
                    lastCol->position.y += 2;
                    lastCol->size.FixedX -= 4;
                    lastCol->size.FixedY -= 4;
                }

                colorBtn->bgColDef    = DefaultMSColors[x + y * 14];
                colorBtn->bgColHover = colorBtn->bgColDef | 0xFF000000;
                colorBtn->bgColClick  = colorBtn->bgColDef | 0xFF000000;

                colorBtn->OnMouseClickHelp = (void*)this;
                colorBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnColorClick;

                startIdForColorButtons++;
            }
        }
        RemoveFromStack();

        guiInstance->CreateComponentWithId(1400, GuiComponentStuff::ComponentType::CANVAS);
        canvas = (GuiComponentStuff::CanvasComponent*)guiInstance->GetComponentFromId(1400);
        canvas->position.x = 0;
        canvas->position.y = 60;
        canvas->size.IsXFixed = false;
        canvas->size.ScaledX = 1;
        canvas->size.IsYFixed = true;
        canvas->size.FixedY = window->size.height - canvas->position.y;
        canvas->bgColor = Colors.white;
        canvasPosition = canvas->GetAbsoluteComponentPosition();
        canvas->OnMouseClickHelp = (void*)this;
        canvas->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnCanvasClick;

        canvas->Clear();
        {
            MPoint aPos = MPoint(
                MousePosition.x - window->position.x - canvasPosition.x,
                MousePosition.y - window->position.y - canvasPosition.y
            ); 
            oldMousePos = aPos;
        }
        canvas->CheckUpdates();

        // canvas->DrawText(0, 0, Colors.black, 1, "Hoi!");
        // canvas->DrawText(8, 16, Colors.brown, 2, "Hello, world!");


        RemoveFromStack();
    }

    void Paint::OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        // OpenFileExplorer* exp = new OpenFileExplorer();
        
        // guiInstance->OnWaitTaskDoneHelp = (void*)this;
        // guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
        // guiInstance->waitTask = exp->dataTask;
    }
    
    void Paint::OnSaveClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {

    }
    void Paint::OnSaveAsClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {

    }
    void Paint::OnClearClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        canvas->Clear();
    }

    void Paint::OnColorClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        if (lastCol != NULL)
        {
            lastCol->position.x -= 2;
            lastCol->position.y -= 2;
            lastCol->size.FixedX += 4;
            lastCol->size.FixedY += 4;
        }

        {
            lastCol = btn;
            lastCol->position.x += 2;
            lastCol->position.y += 2;
            lastCol->size.FixedX -= 4;
            lastCol->size.FixedY -= 4;
        }

        currentColor = btn->bgColDef;
    }

    void Paint::OnCanvasClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        currMouseLeftState = true;
    }

    void Paint::OnExternalWindowFrame(Window* window)
    {
        MPoint aPos = MPoint(
                MousePosition.x - window->position.x - canvasPosition.x,
                MousePosition.y - window->position.y - canvasPosition.y
            ); 

        GuiComponentStuff::ComponentSize tSize = canvas->GetActualComponentSize();
        bool inRange = aPos.x >= 0 && aPos.y >= 0 &&
            aPos.x < tSize.FixedX && 
            aPos.y < tSize.FixedY;

        if (window == activeWindow && inRange)
            guiInstance->screen->tempSelectedComponent = canvas;

        if (activeWindow != window ||
            canvas != guiInstance->screen->selectedComponent)
            return;

        currMouseLeftState = currMouseLeftState && MouseClickState[0];
        
        if (currMouseLeftState)
        {
            if (!oldMouseLeftState)
                oldMousePos = aPos;

            if (inRange)
            {
                if (oldMouseLeftState && oldMousePos != aPos)
                    canvas->DrawLine(oldMousePos.x, oldMousePos.y, aPos.x, aPos.y, currentColor, curSize);
                else if (!oldMouseLeftState)
                    canvas->DrawBlob(aPos.x, aPos.y, currentColor, curSize);
            }
            oldMousePos = aPos;
        }
        oldMouseLeftState = currMouseLeftState;
    }

   

    void Paint::OnTaskDone(Task* task)
    {
        // TaskSimpleData* tsk = (TaskSimpleData*)task;
        // if (tsk->data != NULL)
        // {
        //     const char* path = (const char*)tsk->data;

        //     _Free(testImg1->imagePath);
        //     testImg1->imagePath = StrCopy(path);

        //     testImg1->GetImageFromPath(testImg1->imagePath);

        //     testImg1->size.FixedX = 100;
        //     testImg1->size.FixedY = 100;
        //     window->newSize = Size(100, 120);

        //     if (testImg1->image != NULL)
        //     {
        //         testImg1->size.FixedX = testImg1->image->width;
        //         testImg1->size.FixedY = testImg1->image->height;
        //         testImg1->size.IsXFixed = true;
        //         testImg1->size.IsYFixed = true;
        //         window->newSize = Size(testImg1->image->width, testImg1->image->height + 20);
        //     }

        //     _Free(path);
        // }
    }

    void Paint::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void Paint::OnExternalWindowResize(Window* window)
    {
        int h = window->size.height;
        if (h < 40)
            h = 40;
        int w = window->size.width;
        if (w < 100)
            w = 100;
        window->newSize = Size(w, h);

        canvas->size.FixedY = h - canvas->position.y;

        
        // testImg1->size.FixedY = h - 20;
        // testImg1->size.FixedX = window->size.width;
    }

    void Paint::Free()
    {
        AddToStack();

        //drivePathsYes.free();

        _Free(this);
        RemoveFromStack();
    }

}