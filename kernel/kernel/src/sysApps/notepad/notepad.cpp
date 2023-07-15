#include "notepad.h"
#include "../../cStdLib/cstrTools.h"
#include "../../memory/heap.h"
#include "../../osData/osData.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"
#include "../openFileExplorer/openFileExplorer.h"
#include "../saveFileExplorer/saveFileExplorer.h"


namespace SysApps
{
    Notepad::Notepad()
    {
        lastSavePath = NULL;//StrCopy("");
        btnTaskState = NotePadButtonTaskState::None;

        //window =

        window = (Window*)_Malloc(sizeof(Window), "Notepad Window");
        GuiInstance* gui = (GuiInstance*)_Malloc(sizeof(GuiInstance), "Notepad GUI");
        *gui = GuiInstance(window);
        *(window) = Window((DefaultInstance*)gui, Size(400, 300), Position(100, 100), "Notepad", true, true, true);
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
        //lastClickedComp = NULL;

        guiInstance->CreateComponentWithId(1010, GuiComponentStuff::ComponentType::TEXTFIELD);
        textComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1010);
        textComp->position.x = 0;
        textComp->position.y = 22;
        textComp->AdvancedKeyHitCallBackHelp = (void*)this;
        textComp->AdvancedKeyHitCallBack = (bool(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::KeyHitEventInfo))(void*)&PathTypeCallBack;

        guiInstance->CreateComponentWithId(1020, GuiComponentStuff::ComponentType::BUTTON);
        loadBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1020);
        loadBtn->position.x = 0;
        loadBtn->position.y = 0;
        _Free(loadBtn->textComp->text);
        loadBtn->textComp->text = StrCopy("Load");
        loadBtn->size.FixedX = 50;
        loadBtn->size.FixedY = 20;
        loadBtn->OnMouseClickHelp = (void*)this;
        loadBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnLoadClick;

        guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::BUTTON);
        saveBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1021);
        saveBtn->position.x = 52;
        saveBtn->position.y = 0;
        _Free(saveBtn->textComp->text);
        saveBtn->textComp->text = StrCopy("Save");
        saveBtn->size.FixedX = 50;
        saveBtn->size.FixedY = 20;
        saveBtn->OnMouseClickHelp = (void*)this;
        saveBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnSaveClick;

        guiInstance->CreateComponentWithId(1022, GuiComponentStuff::ComponentType::BUTTON);
        saveAsBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1022);
        saveAsBtn->position.x = 104;
        saveAsBtn->position.y = 0;
        _Free(saveAsBtn->textComp->text);
        saveAsBtn->textComp->text = StrCopy("Save As");
        saveAsBtn->size.FixedX = 64;
        saveAsBtn->size.FixedY = 20;
        saveAsBtn->OnMouseClickHelp = (void*)this;
        saveAsBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnSaveAsClick;


        // guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::TEXTFIELD);
        // pathComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
        // pathComp->position.x = 5*8 + 4;
        // pathComp->position.y = 0;

        // pathComp->AdvancedKeyHitCallBackHelp = (void*)this;
        // pathComp->AdvancedKeyHitCallBack = (bool(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::KeyHitEventInfo))(void*)&PathTypeCallBack;
        // _Free(pathComp->textComp->text);
        // pathComp->textComp->text = StrCopy(path);

        UpdateSizes();

        //Reload();

    }

    void Notepad::UpdateSizes()
    {
        int w = window->size.width;
        int h = window->size.height;
        if (w < 50)
            w = 50;
        if (h < 50)
            h = 50;

        textComp->size.FixedX = w;
        textComp->size.FixedY = h - 22;

        // fileListComp->size.FixedX = w;
        // fileListComp->size.FixedY = h - 25;
        // fileListComp->backgroundColor = Colors.white;
        // pathComp->size.FixedX = w - pathComp->position.x;
        // pathComp->size.FixedY = 16;
    }



    bool Notepad::PathTypeCallBack(GuiComponentStuff::TextFieldComponent* comp, GuiComponentStuff::KeyHitEventInfo event)
    {
        // if (event.Chr == '\n')
        // {
        //     //GlobalRenderer->Clear(Colors.orange);
        //     SetPath(pathComp->textComp->text);
        //     Reload();
        //     return false;
        // }
        return true;
    }

    // void Explorer::ClickCallBack(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::MouseClickEventInfo event)
    // {
    //     GlobalRenderer->Clear(Colors.blue);
    // }

    void Notepad::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);
        Free();
    }

    void Notepad::OnExternalWindowResize(Window* window)
    {
        UpdateSizes();
    }

    void Notepad::Free()
    {
        AddToStack();
        if (lastSavePath != NULL)
        {
            _Free(lastSavePath);
            lastSavePath = NULL;
        }
        

        _Free(this);
        RemoveFromStack();
    }

    void Notepad::OnLoadClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {        
        OpenFileExplorer* exp = new OpenFileExplorer();
        
        guiInstance->OnWaitTaskDoneHelp = (void*)this;
        guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
        guiInstance->waitTask = exp->dataTask;

        btnTaskState = NotePadButtonTaskState::Load;
    }

    void Notepad::OnSaveClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        if (lastSavePath == NULL)
        {
            SaveFileExplorer* exp = new SaveFileExplorer();
            
            guiInstance->OnWaitTaskDoneHelp = (void*)this;
            guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
            guiInstance->waitTask = exp->dataTask;

            btnTaskState = NotePadButtonTaskState::Save;
        }
        else
        {
            SaveInto(StrCopy(lastSavePath));
        }
    }

    void Notepad::OnSaveAsClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    {
        SaveFileExplorer* exp = new SaveFileExplorer();
        
        guiInstance->OnWaitTaskDoneHelp = (void*)this;
        guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
        guiInstance->waitTask = exp->dataTask;

        btnTaskState = NotePadButtonTaskState::Save;
    }

    void Notepad::SaveInto(const char* path)
    {
        if (path == NULL)
            return;

        if (lastSavePath != NULL)
        {
            _Free(lastSavePath);
            lastSavePath = NULL;
        }
        lastSavePath = StrCopy(path);

        FS_STUFF::WriteFileToFullPath(path, (char*)textComp->textComp->text, StrLen(textComp->textComp->text), false);
    }

    void Notepad::LoadFrom(const char* path)
    {
        if (lastSavePath != NULL)
        {
            _Free(lastSavePath);
            lastSavePath = NULL;
        }
        lastSavePath = StrCopy(path);
        //const char* fData = StrCopy(path);

        // LOAD

        char* fData = NULL;
        int fDataLen = 0;

        if (FS_STUFF::ReadFileFromFullPath(path, &fData, &fDataLen))
        {
            char* nData = (char*)_Malloc(fDataLen + 1);
            nData[fDataLen] = 0;
            _memcpy(fData, nData, fDataLen);
            _Free(fData);

            _Free(textComp->textComp->text);
            textComp->textComp->text = (const char*)nData;
        }
        else
        {
            // _Free(textComp->textComp->text);
            // textComp->textComp->text = StrCopy("");
            if (lastSavePath != NULL)
            {
                _Free(lastSavePath);
                lastSavePath = NULL;
            }
        }
    }
    
    void Notepad::OnTaskDone(Task* task)
    {
        NotePadButtonTaskState state = btnTaskState;
        btnTaskState = NotePadButtonTaskState::None;

        if (state == NotePadButtonTaskState::Load)
        {
            TaskSimpleData* tsk = (TaskSimpleData*)task;
            if (tsk->data != NULL)
            {
                const char* path = (const char*)tsk->data;

                LoadFrom(path);

                _Free(path);
            }
            else
            {
                // _Free(textComp->textComp->text);
                // textComp->textComp->text = StrCopy("");
                if (lastSavePath != NULL)
                {
                    _Free(lastSavePath);
                    lastSavePath = NULL;
                }
            }

            return;
        }
        else if (state == NotePadButtonTaskState::Save)
        {
            TaskSimpleData* tsk = (TaskSimpleData*)task;
            if (tsk->data != NULL)
            {
                const char* path = (const char*)tsk->data;

                SaveInto(path);

                _Free(path);
            }

            return;
        }
    }

    // void Explorer::Reload()
    // {
    //     AddToStack();
    //     UpdateSizes();

    //     ClearLists();

    //     const char* drive = FS_STUFF::GetDriveNameFromFullPath(path);
    //     const char* dir = FS_STUFF::GetFolderPathFromFullPath(path);
    //     FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
        
    //     if (fileListComp->children->getCount() > 0)
    //     {
    //         while (fileListComp->children->getCount() > 0)
    //         {
    //             GuiComponentStuff::BaseComponent* comp = fileListComp->children->elementAt(0);
    //             uint64_t coolId = RND::lehmer64();
    //             comp->id = coolId;
    //             guiInstance->DeleteComponentWithId(coolId, true);
    //         }
    //     }


    //     int cutOff = 0;
    //     if (dir != NULL)
    //         cutOff = StrLen(dir) + 1;
    //     if (dir != NULL && StrEquals(dir, ""))
    //     {
    //         _Free(dir);
    //         dir = StrCopy("/");
    //         cutOff = 0;
    //     }
    //     // GlobalRenderer->Clear(Colors.black);
    //     // GlobalRenderer->Println("PATH: \"{}\"", path, Colors.white);
    //     // GlobalRenderer->Println("Drive: \"{}\"", drive, Colors.white);
    //     // GlobalRenderer->Println("Dir: \"{}\"", dir, Colors.white);
    //     // GlobalRenderer->Println("Interface: {}",  to_string((uint64_t)fsInterface), Colors.white);
    //     // //while (true);


    //     if ( fsInterface != NULL && drive != NULL && dir != NULL)
    //     {
    //         // GlobalRenderer->Clear(Colors.black);
    //         // GlobalRenderer->Println("PATH: \"{}\"", path, Colors.white);
    //         // GlobalRenderer->Println("Drive: \"{}\"", drive, Colors.white);
    //         // GlobalRenderer->Println("Dir: \"{}\"", dir, Colors.white);
    //         // GlobalRenderer->Println("Interface: {}",  ConvertHexToString((uint64_t)fsInterface), Colors.yellow);

    //         uint64_t tempCount = 0;
    //         const char** dataList;
    //         int _y = 0;
            
    //         dataList = fsInterface->GetFolders(dir, &tempCount);
    //         //GlobalRenderer->Println("Folders: {}", to_string(tempCount), Colors.white);
    //         if (dataList != NULL)
    //         {
    //             for (int i = 0; i < tempCount; i++)
    //             {
    //                 //GlobalRenderer->Println(" - File: \"{}\"", dataList[i], Colors.white);
    //                 uint64_t coolId = RND::lehmer64();
    //                 //guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::TEXT, 1022);
    //                 guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
    //                 GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

    //                 btnComp->OnMouseClickHelp = (void*)this;
    //                 btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnFolderClick;

    //                 GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
    //                 _Free(textComp->text);
    //                 const char* tempo = StrSubstr(dataList[i], cutOff);
    //                 textComp->text = StrCombine("Folder: ", tempo);
    //                 btnComp->size.FixedY = 16;
    //                 btnComp->size.FixedX = StrLen(textComp->text) * 8;
                    
    //                 folderCompsYes.add(btnComp);
    //                 folderPathsYes.add(tempo);

    //                 //_Free(tempo);
    //                 btnComp->position.x = 0;
    //                 btnComp->position.y = _y;
    //                 _y += 16;
    //                 _Free(dataList[i]);
    //             }   
    //             _Free(dataList);
    //         }


    //         dataList = fsInterface->GetFiles(dir, &tempCount);
    //         //GlobalRenderer->Println("Files: {}", to_string(tempCount), Colors.white);
    //         if (dataList != NULL)
    //         {
    //             for (int i = 0; i < tempCount; i++)
    //             {
    //                 //GlobalRenderer->Println(" - File: \"{}\"", dataList[i], Colors.white);
    //                 uint64_t coolId = RND::lehmer64();
    //                 //guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::TEXT, 1022);
    //                 guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
    //                 GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

    //                 btnComp->OnMouseClickHelp = (void*)this;
    //                 btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnFileClick;

    //                 GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
    //                 _Free(textComp->text);
    //                 const char* tempo = StrSubstr(dataList[i], cutOff);
    //                 textComp->text = StrCombine("File: ", tempo);
    //                 btnComp->size.FixedY = 16;
    //                 btnComp->size.FixedX = StrLen(textComp->text) * 8;
                    
    //                 fileCompsYes.add(btnComp);
    //                 const char* tBLEH = StrCombine(drive, ":");
    //                 filePathsYes.add(StrCombine(tBLEH, dataList[i]));
    //                 _Free(tBLEH);

    //                 _Free(tempo);
    //                 btnComp->position.x = 0;
    //                 btnComp->position.y = _y;
    //                 _y += 16;
    //                 _Free(dataList[i]);
    //             }
    //             _Free(dataList);
    //         }


    //         //while (true);
    //     }
    //     else
    //     {
    //         int _y = 0;
    //         for (int i = 0; i < osData.diskInterfaces.getCount(); i++)
    //         {
    //             DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[i];
    //             if (diskInterface->partitionInterface == NULL)
    //                 continue;
    //             PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
    //             for (int i2 = 0; i2 < partInterface->partitionList.getCount(); i2++)
    //             {
    //                 PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[i2];
    //                 if (!partInfo->hidden && partInfo->type == PartitionInterface::PartitionType::Normal)
    //                 {
    //                     //Println(window, " - Drive: \"{}\"", partInfo->driveName, Colors.bgreen);
    //                     uint64_t coolId = RND::lehmer64();

    //                     guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
    //                     GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

    //                     btnComp->OnMouseClickHelp = (void*)this;
    //                     btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnDriveClick;

    //                     GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
    //                     driveCompsYes.add(btnComp);
    //                     drivePathsYes.add(StrCombine(partInfo->driveName, ":"));

    //                     // guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::TEXT, 1022);
    //                     // GuiComponentStuff::TextComponent* textComp = (GuiComponentStuff::TextComponent*)guiInstance->GetComponentFromId(coolId);
    //                     _Free(textComp->text);
    //                     textComp->text = StrCombine("Drive: ", partInfo->driveName);
    //                     // textComp->position.x = 0;
    //                     // textComp->position.y = _y;
    //                     btnComp->size.FixedY = 16;
    //                     btnComp->size.FixedX = StrLen(textComp->text) * 8;
                        
    //                     btnComp->position.x = 0;
    //                     btnComp->position.y = _y;



    //                     _y += 16;
    //                 }
    //             }
    //         }
    //     }

    //     guiInstance->Render();
    //     fileListComp->Render(GuiComponentStuff::Field(GuiComponentStuff::Position(), GuiComponentStuff::Position(fileListComp->size.FixedX, fileListComp->size.FixedY)));

    //     if (drive != NULL)
    //         _Free(drive);
    //     if (dir != NULL)
    //         _Free(dir);
    //     RemoveFromStack();
    // }
    
    
    // void Explorer::OnGoUpClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
    // {

    // }



}