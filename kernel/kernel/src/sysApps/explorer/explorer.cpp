#include "explorer.h"
#include "../../cmdParsing/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"


namespace SysApps
{
    Explorer::Explorer()
    {
        path = StrCopy("bruh:");

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

        window->OnResizeHelp = (void*)this;
        window->OnResize = (void(*)(void*, Window*))(void*)&OnExternalWindowResize;

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
        _Free(pathComp->textComp->text);
        pathComp->textComp->text = StrCopy(path);

        guiInstance->CreateComponentWithId(1022, GuiComponentStuff::ComponentType::BOX);
        fileListComp = (GuiComponentStuff::BoxComponent*)guiInstance->GetComponentFromId(1022);
        fileListComp->position.x = 0;
        fileListComp->position.y = 20;

        UpdateSizes();

        Reload();

    }

    void Explorer::UpdateSizes()
    {
        int w = window->size.width;
        int h = window->size.height;
        if (w < 50)
            w = 50;
        if (h < 50)
            h = 50;

        fileListComp->size.FixedX = w;
        fileListComp->size.FixedY = h - 25;
        fileListComp->backgroundColor = Colors.white;
        pathComp->size.FixedX = w;
        pathComp->size.FixedY = 16;
    }

    const char* Explorer::GetPath()
    {
        return StrCopy(path);
    }
    void Explorer::SetPath(const char* path)
    {
        _Free(this->path);
        this->path = StrCopy(path);
    }

    bool Explorer::PathTypeCallBack(GuiComponentStuff::TextFieldComponent* comp, GuiComponentStuff::KeyHitEventInfo event)
    {
        if (event.Chr == '\n')
        {
            //GlobalRenderer->Clear(Colors.orange);
            SetPath(pathComp->textComp->text);
            Reload();
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
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void Explorer::OnExternalWindowResize(Window* window)
    {
        UpdateSizes();
    }

    void Explorer::Free()
    {
        _Free(path);
        _Free(this);
    }

    void Explorer::Reload()
    {
        AddToStack();
        UpdateSizes();

        const char* drive = FS_STUFF::GetDriveNameFromFullPath(path);
        const char* dir = FS_STUFF::GetFolderPathFromFullPath(path);
        FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
        
        if (fileListComp->children->getCount() > 0)
        {
            while (fileListComp->children->getCount() > 0)
            {
                GuiComponentStuff::BaseComponent* comp = fileListComp->children->elementAt(0);
                uint64_t coolId = RND::lehmer64();
                comp->id = coolId;
                guiInstance->DeleteComponentWithId(coolId, true);
            }
        }


        int cutOff = 0;
        if (dir != NULL)
            cutOff = StrLen(dir) + 1;
        if (dir != NULL && StrEquals(dir, ""))
        {
            _Free(dir);
            dir = StrCopy("/");
            cutOff = 0;
        }
        // GlobalRenderer->Clear(Colors.black);
        // GlobalRenderer->Println("PATH: \"{}\"", path, Colors.white);
        // GlobalRenderer->Println("Drive: \"{}\"", drive, Colors.white);
        // GlobalRenderer->Println("Dir: \"{}\"", dir, Colors.white);
        // GlobalRenderer->Println("Interface: {}",  to_string((uint64_t)fsInterface), Colors.white);
        // //while (true);


        if ( fsInterface != NULL && drive != NULL && dir != NULL)
        {
            // GlobalRenderer->Clear(Colors.black);
            // GlobalRenderer->Println("PATH: \"{}\"", path, Colors.white);
            // GlobalRenderer->Println("Drive: \"{}\"", drive, Colors.white);
            // GlobalRenderer->Println("Dir: \"{}\"", dir, Colors.white);
            // GlobalRenderer->Println("Interface: {}",  ConvertHexToString((uint64_t)fsInterface), Colors.yellow);

            uint64_t tempCount = 0;
            const char** dataList;
            int _y = 0;
            
            dataList = fsInterface->GetFolders(dir, &tempCount);
            //GlobalRenderer->Println("Folders: {}", to_string(tempCount), Colors.white);
            if (dataList != NULL)
            {
                for (int i = 0; i < tempCount; i++)
                {
                    //GlobalRenderer->Println(" - File: \"{}\"", dataList[i], Colors.white);
                    uint64_t coolId = RND::lehmer64();
                    //guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::TEXT, 1022);
                    guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
                    GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

                    btnComp->OnMouseClickHelp = (void*)this;
                    btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnFolderClick;

                    GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
                    _Free(textComp->text);
                    const char* tempo = StrSubstr(dataList[i], cutOff);
                    textComp->text = StrCombine("Folder: ", tempo);
                    btnComp->size.FixedY = 16;
                    btnComp->size.FixedX = StrLen(textComp->text) * 8;
                    
                    _Free(tempo);
                    btnComp->position.x = 0;
                    btnComp->position.y = _y;
                    _y += 16;
                    _Free(dataList[i]);
                }   
                _Free(dataList);
            }


            dataList = fsInterface->GetFiles(dir, &tempCount);
            //GlobalRenderer->Println("Files: {}", to_string(tempCount), Colors.white);
            if (dataList != NULL)
            {
                for (int i = 0; i < tempCount; i++)
                {
                    //GlobalRenderer->Println(" - File: \"{}\"", dataList[i], Colors.white);
                    uint64_t coolId = RND::lehmer64();
                    //guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::TEXT, 1022);
                    guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
                    GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

                    btnComp->OnMouseClickHelp = (void*)this;
                    btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnFileClick;

                    GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
                    _Free(textComp->text);
                    const char* tempo = StrSubstr(dataList[i], cutOff);
                    textComp->text = StrCombine("File: ", tempo);
                    btnComp->size.FixedY = 16;
                    btnComp->size.FixedX = StrLen(textComp->text) * 8;
                    
                    _Free(tempo);
                    btnComp->position.x = 0;
                    btnComp->position.y = _y;
                    _y += 16;
                    _Free(dataList[i]);
                }
                _Free(dataList);
            }


            //while (true);
        }
        else
        {
            int _y = 0;
            for (int i = 0; i < osData.diskInterfaces.getCount(); i++)
            {
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[i];
                if (diskInterface->partitionInterface == NULL)
                    continue;
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                for (int i2 = 0; i2 < partInterface->partitionList.getCount(); i2++)
                {
                    PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[i2];
                    if (!partInfo->hidden && partInfo->type == PartitionInterface::PartitionType::Normal)
                    {
                        //Println(window, " - Drive: \"{}\"", partInfo->driveName, Colors.bgreen);
                        uint64_t coolId = RND::lehmer64();
                        guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::TEXT, 1022);
                        GuiComponentStuff::TextComponent* textComp = (GuiComponentStuff::TextComponent*)guiInstance->GetComponentFromId(coolId);
                        _Free(textComp->text);
                        textComp->text = StrCombine("Drive: ", partInfo->driveName);
                        textComp->position.x = 0;
                        textComp->position.y = _y;
                        _y += 16;
                    }
                }
            }
        }

        guiInstance->Render();
        fileListComp->Render(GuiComponentStuff::Field(GuiComponentStuff::Position(), GuiComponentStuff::Position(fileListComp->size.FixedX, fileListComp->size.FixedY)));

        if (drive != NULL)
            _Free(drive);
        if (dir != NULL)
            _Free(dir);
        RemoveFromStack();
    }
    
    void Explorer::OnFolderClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        const char* temp2 = StrCombine(path, "maab");
        _Free(path);
        path = StrCombine(temp2, "/");
        _Free(temp2);

        _Free(pathComp->textComp->text);
        pathComp->textComp->text = StrCopy(path);
        Reload();
    }
    void Explorer::OnFileClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        window->renderer->Clear(Colors.blue);
    }

}