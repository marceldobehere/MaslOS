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
        path = StrCopy("");
        OnFileClickCallback = NULL;
        OnFileClickHelp = NULL;

        //window =

        window = (Window*)_Malloc(sizeof(Window), "Explorer Window");
        GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "Explorer GUI");
        *gui = GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(400, 300), Position(100, 100), "Explorer", true, true, true);
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
        lastClickedComp = NULL;

        guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::TEXTFIELD);
        pathComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
        pathComp->position.x = 5*8 + 4;
        pathComp->position.y = 0;

        pathComp->AdvancedKeyHitCallBackHelp = (void*)this;
        pathComp->AdvancedKeyHitCallBack = (bool(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::KeyHitEventInfo))(void*)&PathTypeCallBack;
        _Free(pathComp->textComp->text);
        pathComp->textComp->text = StrCopy(path);

        guiInstance->CreateComponentWithId(1023, GuiComponentStuff::ComponentType::BUTTON);
        goUpBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1023);
        goUpBtn->position.x = 0;
        goUpBtn->position.y = 0;
        _Free(goUpBtn->textComp->text);
        goUpBtn->textComp->text = StrCopy("Go Up");
        goUpBtn->size.FixedY = 16;
        goUpBtn->size.FixedX = 5*8;
        goUpBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnGoUpClick;
        goUpBtn->OnMouseClickHelp = this;


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
        pathComp->size.FixedX = w - pathComp->position.x;
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
        AddToStack();
        _Free(path);
        
        ClearLists();
        folderCompsYes.free();
        folderPathsYes.free();
        driveCompsYes.free();
        drivePathsYes.free();
        fileCompsYes.free();
        filePathsYes.free();

        _Free(this);
        RemoveFromStack();
    }

    void Explorer::ClearLists()
    {
        for (int i = 0; i < folderPathsYes.getCount(); i++)
            _Free((void*)folderPathsYes.elementAt(i)); 
        for (int i = 0; i < drivePathsYes.getCount(); i++)
            _Free((void*)drivePathsYes.elementAt(i)); 
        for (int i = 0; i < filePathsYes.getCount(); i++)
            _Free((void*)filePathsYes.elementAt(i));

        folderCompsYes.clear();
        folderPathsYes.clear();
        driveCompsYes.clear();
        drivePathsYes.clear();
        fileCompsYes.clear();
        filePathsYes.clear();
    }

    void Explorer::Reload()
    {
        AddToStack();
        UpdateSizes();

        ClearLists();

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
                    
                    folderCompsYes.add(btnComp);
                    folderPathsYes.add(tempo);

                    //_Free(tempo);
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
                    
                    fileCompsYes.add(btnComp);
                    const char* tBLEH = StrCombine(drive, ":");
                    filePathsYes.add(StrCombine(tBLEH, dataList[i]));
                    _Free(tBLEH);

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

                        guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
                        GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

                        btnComp->OnMouseClickHelp = (void*)this;
                        btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnDriveClick;

                        GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
                        driveCompsYes.add(btnComp);
                        drivePathsYes.add(StrCombine(partInfo->driveName, ":"));

                        // guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::TEXT, 1022);
                        // GuiComponentStuff::TextComponent* textComp = (GuiComponentStuff::TextComponent*)guiInstance->GetComponentFromId(coolId);
                        _Free(textComp->text);
                        textComp->text = StrCombine("Drive: ", partInfo->driveName);
                        // textComp->position.x = 0;
                        // textComp->position.y = _y;
                        btnComp->size.FixedY = 16;
                        btnComp->size.FixedX = StrLen(textComp->text) * 8;
                        
                        btnComp->position.x = 0;
                        btnComp->position.y = _y;



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
        int indx = folderCompsYes.getIndexOf(btn);
        if (indx == -1)
            return;
        const char* pathThing = folderPathsYes[indx];

        const char* temp2 = StrCombine(path, pathThing);
        _Free(path);
        path = StrCombine(temp2, "/");
        _Free(temp2);

        _Free(pathComp->textComp->text);
        pathComp->textComp->text = StrCopy(path);
        Reload();
    }
    void Explorer::OnFileClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        int indx = fileCompsYes.getIndexOf(btn);
        if (indx == -1)
            return;
        const char* pathThing = filePathsYes[indx];

        if (OnFileClickCallback != NULL)
            OnFileClickCallback(OnFileClickHelp, pathThing);
        else
            FS_STUFF::OpenFile(pathThing);
    }

    void Explorer::OnDriveClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        int indx = driveCompsYes.getIndexOf(btn);
        if (indx == -1)
            return;
        const char* pathThing = drivePathsYes[indx];
        _Free(path);
        path = StrCopy(pathThing);

        _Free(pathComp->textComp->text);
        pathComp->textComp->text = StrCopy(path);
        Reload();
    }
    void Explorer::OnGoUpClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        const char* drive = FS_STUFF::GetDriveNameFromFullPath(path);
        const char* dir = FS_STUFF::GetFolderPathFromFullPath(path);
        
        // GlobalRenderer->Clear(Colors.black);
        // GlobalRenderer->Println("DRIVE:  \"{}\"", drive, Colors.yellow);
        // GlobalRenderer->Println("DIR:    \"{}\"", dir, Colors.yellow);
        // GlobalRenderer->Println("PATH:   \"{}\"", path, Colors.yellow);
        


        if (drive != NULL && dir != NULL)
        {
            const char* cool1 = StrCombine(drive, ":", dir, "/");
            //GlobalRenderer->Println("COOL1:  \"{}\"", cool1, Colors.yellow);
            const char* cool2 = StrCombine(drive, ":");
            //GlobalRenderer->Println("COOL2:  \"{}\"", cool2, Colors.yellow);
            if (StrEquals(cool2, path))
            {
                _Free(path);
                path = StrCopy("");
            }
            else if (StrEquals(cool1, path))
            {
                // GO UP
                int lstIndex = StrLastIndexOf(path, '/', 1);
                // test:abc/def/ 
                //          ^

                if (lstIndex != -1)
                {
                    const char* nPath = StrSubstr(path, 0, lstIndex + 1);
                    _Free(path);
                    path = nPath;
                }
                else
                {
                    lstIndex = StrLastIndexOf(path, ':');
                    if (lstIndex != -1)
                    {
                        const char* nPath = StrSubstr(path, 0, lstIndex + 1);
                        _Free(path);
                        path = nPath;
                    }
                    else
                    {
                        const char* nPath = StrCopy("");
                        _Free(path);
                        path = nPath;
                    }
                }
                // GlobalRenderer->Println("NPATH:  \"{}\"", path, Colors.yellow);

                // while (true);
            
            }
            else
            {
                _Free(path);
                path = StrCopy(cool1);
            }
            _Free(cool1);
            _Free(cool2);
        }
        

        // while (true)
        //     ;

        if (drive != NULL)
            _Free(drive);
        if (dir != NULL)
            _Free(dir); 

        _Free(pathComp->textComp->text);
        pathComp->textComp->text = StrCopy(path);
        Reload();
    }



}