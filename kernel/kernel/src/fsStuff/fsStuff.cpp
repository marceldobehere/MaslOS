#include "fsStuff.h"

#include <stdint.h>
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../kernelStuff/stuff/cstr.h"
#include "../cmdParsing/cstrTools.h"
#include "../OSDATA/osdata.h"
#include "../tasks/enterHandler/taskEnterHandler.h"

#include "../sysApps/imgTest/imgTest.h"
#include "../sysApps/notepad/notepad.h"
#include "../musicTest/musicTest.h"

namespace FS_STUFF
{
    char* GetDriveNameFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        AddToStack();
        int fIndex = -1;
        int len = StrLen(path);
        RemoveFromStack();
        
        AddToStack();
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        RemoveFromStack();

        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        int nLen = fIndex;
        char* name = (char*)_Malloc(nLen + 1, "drive name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }

    char* GetFilePathFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        int fIndex = -1;
        int len = StrLen(path);
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        // AB :  XYZ (6)
        // 01 2 345
        // 6 - (2 + 1) => 3

        int nLen = len - (fIndex + 1);
        char* name = (char*)_Malloc(nLen + 1, "path name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[fIndex + 1 + i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }
    
    char* GetFolderPathFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;

        AddToStack();

        int fIndex = -1;
        int len = StrLen(path);
        for (int i = 0; i < len; i++)
            if (path[i] == ':')
            {
                fIndex = i;
                break;
            }
        if (fIndex < 1)
        {
            RemoveFromStack();
            return NULL;
        }
        
        int lSI = -1;
        for (int i = len - 1; i > 0; i--)
            if (path[i] == '/')
            {
                lSI = i;
                break;
            }


        char* name;


        // AB :  XYZ (6)
        // 01 2 345
        // 6 - (2 + 1) => 3



        if (lSI == -1)
        {
            lSI = fIndex + 1;
        }

        int nLen = lSI - (fIndex + 1);
        name = (char*)_Malloc(nLen + 1, "path name malloc");
        for (int i = 0; i < nLen; i++)
            name[i] = path[fIndex + 1 + i];
        name[nLen] = 0;

        RemoveFromStack();
        return name;
    }

    FilesystemInterface::FileInfo* GetFileInfoFromFullPath(const char* path)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
        if (fsInterface == NULL)
            return NULL;
        
        char* relPath = FS_STUFF::GetFilePathFromFullPath(path);
        if (relPath == NULL)
            return NULL;

        if (!fsInterface->FileExists(relPath))
        {
            _Free(relPath);
            return NULL;
        }

        FilesystemInterface::FileInfo* inf = fsInterface->GetFileInfo(relPath);

        _Free(relPath);
        return inf;       
    }

    FilesystemInterface::GenericFilesystemInterface* GetFsInterfaceFromFullPath(const char* path)
    {
        if (path == NULL)
            return NULL;


        AddToStack();

        char* drive = GetDriveNameFromFullPath(path);
        if (drive == NULL)
        {
            RemoveFromStack();
            return NULL;
        }
        
        FilesystemInterface::GenericFilesystemInterface* foundFsInterface = NULL;
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
                    if (!StrEquals(partInfo->driveName, drive))
                        continue;

                    if (partInfo->fsInterface != NULL)
                    {
                        foundFsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInfo->fsInterface;
                        break;
                    }
                }
            }
        }
        _Free(drive);
        if (foundFsInterface == NULL)
        {
            RemoveFromStack();
            return NULL;
        }

        RemoveFromStack();
        return foundFsInterface;
    }
    


    bool ReadFileFromFullPath(const char* path, char** resBuffer, int* resBufferLen)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
        if (fsInterface == NULL)
            return false;
        
        char* relPath = FS_STUFF::GetFilePathFromFullPath(path);
        if (relPath == NULL)
            return false;

        if (!fsInterface->FileExists(relPath))
        {
            _Free(relPath);
            return false;
        }

        char* buf = NULL;
        fsInterface->ReadFile(relPath, (void**)(&buf));

        if (buf == NULL)
        {
            _Free(relPath);
            return false;
        }

        int fSize = fsInterface->GetFileInfo(relPath)->sizeInBytes;

        *resBuffer = buf;
        *resBufferLen = fSize;

        _Free(relPath);
        return true;        
    }

    bool FileExists(const char* path)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
        if (fsInterface == NULL)
            return false;
        
        char* relPath = FS_STUFF::GetFilePathFromFullPath(path);
        if (relPath == NULL)
            return false;

        bool res = fsInterface->FileExists(relPath);

        _Free(relPath);
        return res;
    }

    bool CreateFileIfNotExist(const char* path)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
        if (fsInterface == NULL || path == NULL)
            return false;
        
        char* relPath = FS_STUFF::GetFilePathFromFullPath(path);
        if (relPath == NULL)
            return false;

        if (!fsInterface->FileExists(relPath))
        {
            fsInterface->CreateFile(relPath);
            _Free(relPath);
            return true;
        }
        _Free(relPath);
        return false;
    }

    bool WriteFileToFullPath(const char* path, char* buffer, int bufferLen, bool createIfNotExists)
    {
        FilesystemInterface::GenericFilesystemInterface* fsInterface = FS_STUFF::GetFsInterfaceFromFullPath(path);
        if (fsInterface == NULL || buffer == NULL || path == NULL)
            return false;
        
        char* relPath = FS_STUFF::GetFilePathFromFullPath(path);
        if (relPath == NULL)
            return false;

        if (!fsInterface->FileExists(relPath))
        {
            if (createIfNotExists)
            {
                fsInterface->CreateFile(relPath);
            }
            else
            {
                _Free(relPath);
                return false;
            }
        }


        const char* res = fsInterface->WriteFile(relPath, bufferLen, (void*) buffer);

        _Free(relPath);
        return true;    
    }

    TaskReadFile* ReadFileTask(const char* path)
    {
        if (path == NULL)
            return NULL;

        return NewReadFileTask(path);
    }    
    
    TaskWriteFile* WriteFileTask(const char* path, char* buffer, int bufferLen, bool createIfNotExists)
    {
        if (path == NULL)
            return NULL;

        if (!FileExists(path))
        {
            if (createIfNotExists)
                CreateFileIfNotExist(path);
            else
                return NULL;
        }

        return NewWriteFileTask(path, buffer, bufferLen);
    }

    bool OpenFile(const char* path)
    {
        if (path == NULL)
            return false;

        if (StrEndsWith(path, ".mbif"))
        {
            new SysApps::ImageViewer(path);
            return true;
        }
        if (StrEndsWith(path, ".txt"))
        {
            SysApps::Notepad* notepad = new SysApps::Notepad();
            notepad->LoadFrom(path);
            return true;
        }
        if (StrEndsWith(path, ".maab"))
        {
            const char* t = StrCombine("maab \"", path);
            const char* t2= StrCombine(t, "\"");
            _Free(t);
            RunTerminalCommand(t2, "MAAB TEST", false, false);
            _Free(t2);
            return true;
        }
        if (StrEndsWith(path, ".mbaf"))
        {
            int totalLen = 0;
            char* buf = NULL;
            ReadFileFromFullPath(path, &buf, &totalLen);
            if (buf != NULL && totalLen > 8)
            {
                Music::rawAudioInUse = true;
                int bitRate = ((int*)buf)[1];
                int div = PIT::BaseFrequency / bitRate;
                Music::rawAudioDiv = div;
                Music::currentRawAudio->clear();
                for (long offset = 8; offset < totalLen; offset++)
                    Music::currentRawAudio->add(buf[offset]);
                Music::rawAudioInUse = false;
                return true;
            }
            return false;
        }
        return false;
    }



    void RunTerminalCommand(const char* terminalCommand, const char* terminalWindowTitle, bool hideTerminalWindow, bool autoCloseTerminalWindow)
    {
        Window* mainWindow = (Window*)_Malloc(sizeof(Window), "App Window");
        TerminalInstance* terminal = (TerminalInstance*)_Malloc(sizeof(TerminalInstance), "App Terminal");
        *terminal = TerminalInstance(&guestUser);
        *(mainWindow) = Window((DefaultInstance*)terminal, Size(500, 500), Position(50, 50), terminalWindowTitle, true, true, true);
        mainWindow->hidden = hideTerminalWindow;
        mainWindow->oldHidden = !hideTerminalWindow;
        
        osData.windows.add(mainWindow);
        terminal->SetWindow(mainWindow);
        terminal->closeWindowAfterTask = autoCloseTerminalWindow;
        ((TerminalInstance*)mainWindow->instance)->Cls();
        //KeyboardPrintStart(mainWindow);
        //((TerminalInstance*)mainWindow->instance)->KeyboardPrintStart();
        if (!hideTerminalWindow)
            osData.windowsToGetActive.add(mainWindow);

        //((NewTerminalInstance*)terminal->newTermInstance)->Println(BLEHUS_CMD);
        {
            int i = 0;
            for (; terminalCommand[i] != 0; i++)
                terminal->terminalInput[i] = terminalCommand[i];
            terminal->terminalInput[i] = 0;
            terminal->userlen = i;
        }
        terminal->tasks.add(NewEnterTask(terminal));
    }
}

