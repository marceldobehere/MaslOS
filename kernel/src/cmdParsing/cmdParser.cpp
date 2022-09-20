#include "cmdParser.h"
#include "../Rendering/BasicRenderer.h"
#include "cstrTools.h"
#include "../Rendering/Cols.h"
#include <stdint.h>
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "../OSDATA/osdata.h"
#include "../kernelStuff/other_IO/pit/pit.h"
#include "../tasks/sleep/taskSleep.h"
#include "../tasks/playBeep/playBeep.h"
#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/ram/ramDiskInterface.h"
#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/file/fileDiskInterface.h"
#include "../kernelStuff/Disk_Stuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../kernelStuff/Disk_Stuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"

void LogError(const char* msg, Window* window)
{
    AddToStack();
    window->renderer->Println(msg, Colors.bred);
    RemoveFromStack();
}

void LogError(const char* msg, const char* var, Window* window)
{
    AddToStack();
    window->renderer->Println(msg, var, Colors.bred);
    RemoveFromStack();
}

void LogInvalidArgumentCount(int expected, int found, Window* window)
{
    AddToStack();
    window->renderer->Print("Invalid Argument count. Expected {} but got ", to_string((uint64_t)expected), Colors.bred);
    window->renderer->Println("{} instead.", to_string((uint64_t)found), Colors.bred);
    RemoveFromStack();
}


void EditPartitionSetting(PartitionInterface::PartitionInfo* part, const char* param, const char* val, OSUser* user, Window* window)
{
    if (StrEquals(param, "hidden"))
    {
        if (StrEquals(val, "true") || StrEquals(val, "TRUE"))
        {
            part->hidden = true;
            window->renderer->Println("Parameter changed!");
        }
        else if (StrEquals(val, "false") || StrEquals(val, "FALSE"))
        {
            part->hidden = false;
            window->renderer->Println("Parameter changed!");
        }
        else
            LogError("Invalid Value for Paramter!", window);
    }
    else if (StrEquals(param, "name"))
    {
        free((void*)part->name);
        part->name = StrCopy(val);
        part->nameLen = StrLen(val);
        window->renderer->Println("Parameter changed!");
    }
    else if (StrEquals(param, "description"))
    {
        free((void*)part->description);
        part->description = StrCopy(val);
        part->descriptionLen = StrLen(val);
        window->renderer->Println("Parameter changed!");
    }
    else if (StrEquals(param, "drive name"))
    {
        free((void*)part->driveName);
        part->driveName = StrCopy(val);
        part->driveNameLen = StrLen(val);
        window->renderer->Println("Parameter changed!");
    }
    else
        LogError("Invalid Parameter!", window);
    
}

void ParseCommand(char* input, char* oldInput, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;

    //window->renderer->Println("This is test out!");
    if (StrEquals(input, "cls"))
    {
        window->renderer->Cls();
        RemoveFromStack();
        return;
    }

    if (StrEquals(input, "malloc"))
    {
        malloc(0x5000, "Test malloc command");
        RemoveFromStack();
        return;
    }

    if (StrEquals(input, "exit"))
    {
        window->renderer->Println("Exiting...");
        osData.exit = true;
        RemoveFromStack();
        return;
    }


    if (StrEquals(input, "crash"))
    {
        window->renderer->Println("Crashing...");
        asm("int $0x0e");
        RemoveFromStack();
        return;
    }
    //return;

    StringArrData* data = SplitLine(oldInput);

    if (StrEquals(data->data[0], "login") && terminal->mode == commandMode::enterPassword)
    {
        terminal->mode = commandMode::none;
        StringArrData* data2 = SplitLine(input);
        if (data->len == 2)
        {
            if (data2->len == 1)
                login(data->data[1], data2->data[0], user, window);
            else
                LogError("Password can only be one Argument long!", window);
        }
        
        free(data);
        free(data2);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "set") && terminal->mode == commandMode::enterPassword)
    {
        terminal->mode = commandMode::none;
        StringArrData* data2 = SplitLine(input);
        if (data->len == 2 || data->len == 3)
        {
            if (StrEquals(data->data[1], "password"))
            {
                if (data2->len == 1)
                {
                    (*user)->password = StrCopy(data2->data[0]);
                    //window->renderer->Println("Password is now \"{}\".", (*user)->password, Colors.yellow);
                }
                else
                    LogError("Password can only be one Argument long!", window);
            }
        }
        
        free(data);
        free(data2);
        RemoveFromStack();
        return;
    }

    free(data);
    data = SplitLine(input);

    if (data->len == 0)
    {
        free(data);
        RemoveFromStack();
        return;
    }

    // window->renderer->Println("Parts:");
    // for (int i = 0; i < data->len; i++)
    //     window->renderer->Println(" - \"{}\"", data->data[i], Colors.bgreen);

    if (StrEquals(data->data[0], "echo"))
    {
        if (data->len == 2)
            window->renderer->Println(data->data[1]);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "sleep"))
    {
        if (data->len == 2)
        {
            int64_t time = to_int(data->data[1]);
            if (time > 0)
            {
                window->renderer->Println("Sleeping for {} ms...", to_string(time), Colors.yellow);
                //PIT::Sleep((uint64_t)time);
                {

                    if (window->instance != NULL)
                    {
                        if (window->instance->instanceType == InstanceType::Terminal)
                        {
                            TerminalInstance* terminal = (TerminalInstance*)window->instance;

                            terminal->tasks.add(NewSleepTask(time));
                        }
                    }

                }
            }
            else
                LogError("You cannot sleep less than 0ms!", window);
        }
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "set"))
    {
        if (data->len == 3)
            SetCmd(data->data[1], data->data[2], user, window);
        else if (data->len == 2 && StrEquals(data->data[1], "password"))
            SetCmd(data->data[1], "", user, window);
        else
            LogInvalidArgumentCount(2, data->len-1, window);
        
        free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "get"))
    {
        if (data->len == 2)
            GetCmd(data->data[1], *user, window);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "substr"))
    {
        if (data->len == 3)
        {
            const char* dataa = StrSubstr(data->data[1], to_int(data->data[2]));
            window->renderer->Println("Result: {}", dataa);
            free((void*)dataa);
        }
        else
            LogInvalidArgumentCount(2, data->len-1, window);
        
        free(data);
        RemoveFromStack();
        return;
    }


    if (StrEquals(data->data[0], "login"))
    {
        if (data->len == 2)
            login(data->data[1], user, window);
        else
            LogInvalidArgumentCount(1, data->len-1, window);
        
        free(data);
        RemoveFromStack();
        return;
    }

    if (StrEquals(data->data[0], "beep"))
    {
        int onDur = to_int(data->data[1]);
        int offDur = to_int(data->data[2]);
        int totDur = to_int(data->data[3]);
        
        if (data->len == 4)
        {
            int size = to_int(data->data[3]);
            terminal->tasks.add(NewBeepTask(onDur, offDur, totDur));
            window->renderer->Println("Playing beep...");

            free(data);
            RemoveFromStack();
            return;
        }
        else
            LogInvalidArgumentCount(3, data->len-1, window);
    }
    
    if (StrEquals(data->data[0], "disk"))
    {
        // if (data->len == 3)
        // {
        //     if (StrEquals(data->data[1], "create"))
        //     {
        //         int size = to_int(data->data[2]);
        //         osData.diskInterfaces.add(new DiskInterface::RamDiskInterface(size));
        //         window->renderer->Println("Ram Disk with {} sectors created!", to_string(size), (*user)->colData.defaultTextColor);
        //     }
        //     else
        //         LogError("No valid arguments passed!", window);
        // }
        // else 
        if (data->len == 4)
        {
            if (StrEquals(data->data[1], "create"))
            {
                if (StrEquals(data->data[2], "ram disk"))
                {
                    int size = to_int(data->data[3]);
                    osData.diskInterfaces.add(new DiskInterface::RamDiskInterface(size));
                    window->renderer->Println("Ram Disk with {} sectors created!", to_string(size), (*user)->colData.defaultTextColor);
                }
                else
                    LogError("No valid arguments passed!", window);
            }
            else if (StrEquals(data->data[2], "read"))
            {
                int num = to_int(data->data[1]);
                int size = to_int(data->data[3]);

                uint8_t* buffer = (uint8_t*)malloc((((size - 1) / 512) + 1) * 512, "Malloc for disk read buffer");

                if (osData.diskInterfaces[num]->Read(0, ((size - 1) / 512) + 1, (void*)buffer))
                {
                    window->renderer->Println("Raw Data:");
                    for (int i = 0; i < size; i++)
                        window->renderer->Print(buffer[i]);
                    window->renderer->Println();
                }
                else
                {
                    LogError("Disk read failed!", window);
                }


                free(buffer);
                
            }
            else if (StrEquals(data->data[2], "get"))
            {
                int num = to_int(data->data[1]);

                if (StrEquals(data->data[3], "max sector count"))
                {
                    window->renderer->Println("Max Sector Count: {}", to_string((uint64_t)osData.diskInterfaces[num]->GetMaxSectorCount()), (*user)->colData.defaultTextColor);
                }
                else
                {
                    LogError("No valid arguments for get function passed!", window);
                }
            }
            else if (StrEquals(data->data[2], "partition"))
            {
                int num = to_int(data->data[1]);

                if (StrEquals(data->data[3], "create"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)new PartitionInterface::MRAPSPartitionInterface(diskInterface);

                    if (diskInterface->partitionInterface != NULL)
                        window->renderer->Println("Partition Interface creation Success!", (*user)->colData.defaultTextColor);
                    else
                        LogError("Partition Interface creation failed!", window);

                }
                else if (StrEquals(data->data[3], "init"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        const char* res = partInterface->InitAndSavePartitionTable();
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            window->renderer->Println("Partition Init Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Init failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else if (StrEquals(data->data[3], "save"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        const char* res = partInterface->SavePartitionTable();
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            window->renderer->Println("Partition Save Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Save failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else if (StrEquals(data->data[3], "load"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        const char* res = partInterface->LoadPartitionTable();
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            window->renderer->Println("Partition Load Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Load failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else if (StrEquals(data->data[3], "list"))
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[num];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface != NULL)
                    {
                        uint64_t partCount = partInterface->partitionList.getCount();
                        window->renderer->Println("Partition Count: {}", to_string(partCount), Colors.yellow);
                        window->renderer->Println("Partition Data:", Colors.yellow);
                        for (int i = 0; i < partCount; i++)
                        {   
                            PartitionInterface::PartitionInfo* info = partInterface->partitionList[i];
                            window->renderer->Println(" + Partition {}:", to_string(i), Colors.orange);
                            window->renderer->Println("    - Name:        \"{}\"", info->name, Colors.yellow);
                            window->renderer->Println("    - Description: \"{}\"", info->description, Colors.yellow);
                            window->renderer->Println("    - Drive Name:  \"{}\"", info->driveName, Colors.yellow);
                            window->renderer->Println("    - Size:         {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                            //window->renderer->Println("    - Location:     0x{}", ConvertHexToString(info->locationInBytes), Colors.yellow);
                            {
                                window->renderer->Print("    - Location:     0x{} ", ConvertHexToString(info->locationInBytes), Colors.yellow);
                                window->renderer->Print("({} - ", to_string(info->locationInBytes), Colors.yellow);
                                window->renderer->Print("{})", to_string(info->locationInBytes + info->sizeInBytes - 1), Colors.yellow);
                                window->renderer->Println();
                            }
                            window->renderer->Println("    - Owner:        0x{}", ConvertHexToString((uint64_t)info->owner), Colors.yellow);
                            window->renderer->Println("    - Type:         {}", PartitionInterface::PartitionTypeStr[(uint8_t)info->type], Colors.yellow);
                            window->renderer->Println("    - FS Type:      {}", FilesystemInterface::FilesystemInterfaceTypeStr[(uint8_t)info->fsType], Colors.yellow);
                            window->renderer->Println("    - FS Interface: 0x{}", ConvertHexToString((uint64_t)info->fsInterface), Colors.yellow);
                            window->renderer->Println("    - Hidden:       {}", info->hidden ? "True" : "False", Colors.yellow);
                        }
                    }
                    else
                        LogError("Drive has no Partition Manager!", window);
                }
                else
                {
                    LogError("No valid arguments for partition function passed!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 5)
        {
            if (StrEquals(data->data[2], "partition"))
            {
                int diskNum = to_int(data->data[1]);
                int partNum = to_int(data->data[4]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                {
                    if (StrEquals(data->data[3], "delete"))
                    {
                        const char* res = partInterface->DeletePartition(partInterface->partitionList[partNum]);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                            window->renderer->Println("Partition Deletion Success!", (*user)->colData.defaultTextColor);
                        else
                            LogError("Partition Deletion failed! Error: \"{}\"", res, window);

                    }
                    else if (StrEquals(data->data[3], "wipe"))
                    {
                        const char* res = partInterface->WipePartitionContents(partInterface->partitionList[partNum]);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                            window->renderer->Println("Partition Wipe Success!", (*user)->colData.defaultTextColor);
                        else
                            LogError("Partition Wipe failed! Error: \"{}\"", res, window);
                    }
                    else if (StrEquals(data->data[3], "show"))
                    {
                        PartitionInterface::PartitionInfo* info = partInterface->partitionList[partNum];
                        window->renderer->Println(" + Partition {}:", to_string(partNum), Colors.orange);
                        window->renderer->Println("    - Name:        \"{}\"", info->name, Colors.yellow);
                        window->renderer->Println("    - Description: \"{}\"", info->description, Colors.yellow);
                        window->renderer->Println("    - Drive Name:  \"{}\"", info->driveName, Colors.yellow);
                        window->renderer->Println("    - Size:         {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                        //window->renderer->Println("    - Location:     0x{}", ConvertHexToString(info->locationInBytes), Colors.yellow);
                        {
                            window->renderer->Print("    - Location:     0x{} ", ConvertHexToString(info->locationInBytes), Colors.yellow);
                            window->renderer->Print("({} - ", to_string(info->locationInBytes), Colors.yellow);
                            window->renderer->Print("{})", to_string(info->locationInBytes + info->sizeInBytes - 1), Colors.yellow);
                            window->renderer->Println();
                        }
                        window->renderer->Println("    - Owner:        0x{}", ConvertHexToString((uint64_t)info->owner), Colors.yellow);
                        window->renderer->Println("    - Type:         {}", PartitionInterface::PartitionTypeStr[(uint8_t)info->type], Colors.yellow);
                        window->renderer->Println("    - FS Type:      {}", FilesystemInterface::FilesystemInterfaceTypeStr[(uint8_t)info->fsType], Colors.yellow);
                        window->renderer->Println("    - FS Interface: 0x{}", ConvertHexToString((uint64_t)info->fsInterface), Colors.yellow);
                        window->renderer->Println("    - Hidden:       {}", info->hidden ? "True" : "False", Colors.yellow);
                    }
                    else
                        LogError("No valid arguments passed!", window);
                }
            }
            else if (StrEquals(data->data[2], "read"))
            {
                int num = to_int(data->data[1]);
                int start = to_int(data->data[3]);
                int count = to_int(data->data[4]);

                // Disk 0 read 0 100
                uint8_t* buffer = (uint8_t*)malloc(count, "Malloc for disk read buffer");

                if (osData.diskInterfaces[num]->ReadBytes(start, count, (void*)buffer))
                {
                    window->renderer->Println("Raw Data:");
                    for (int i = 0; i < count; i++)
                        window->renderer->Print(buffer[i]);
                    window->renderer->Println();
                }
                else
                {
                    LogError("Disk read failed!", window);
                }


                _free(buffer);
                
            }
            else if (StrEquals(data->data[2], "write"))
            {   
                int num = to_int(data->data[1]);
                int size = to_int(data->data[4]);
                
                uint8_t val = data->data[3][0];
                if (StrEquals(data->data[3], "NULL"))
                    val = 0;

                // disk 0 write A 100
                uint8_t* buffer = (uint8_t*)malloc((((size - 1) / 512) + 1) * 512, "Malloc for disk read buffer");
                _memset(buffer, 0, (((size - 1) / 512) + 1) * 512);
                _memset(buffer, val, size);

                if (osData.diskInterfaces[num]->Write(0, ((size - 1) / 512) + 1, (void*)buffer))
                {
                    window->renderer->Println("Disk write successful!");
                }
                else
                {
                    LogError("Disk write failed!", window);
                }


                _free(buffer);
            }
            else if (StrEquals(data->data[2], "write_cached"))
            {   
                int num = to_int(data->data[1]);
                int size = to_int(data->data[4]);
                
                uint8_t val = data->data[3][0];
                if (StrEquals(data->data[3], "NULL"))
                    val = 0;

                // disk 0 write A 100
                uint8_t* buffer = (uint8_t*)malloc((((size - 1) / 512) + 1) * 512, "Malloc for disk read buffer");
                _memset(buffer, 0, (((size - 1) / 512) + 1) * 512);

                if (!osData.diskInterfaces[num]->Read(0, ((size - 1) / 512) + 1, (void*)buffer))
                    LogError("Disk read failed!", window);
                {
                    _memset(buffer, val, size);

                    if (osData.diskInterfaces[num]->Write(0, ((size - 1) / 512) + 1, (void*)buffer))
                    {
                        window->renderer->Println("Disk write successful!");
                    }
                    else
                    {
                        LogError("Disk write failed!", window);
                    }
                }
                _free(buffer);
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 6)
        {
            if (StrEquals(data->data[1], "create"))
            {
                if (StrEquals(data->data[2], "file disk"))
                {
                    int64_t diskID = to_int(data->data[3]);
                    int64_t partID = to_int(data->data[4]);
                    const char* filename = data->data[5];

                    if (diskID < 0 || diskID >= osData.diskInterfaces.getCount())
                        LogError("Invalid Disk selected!", window);
                    else
                    {
                        DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskID];
                        PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                        if (partInterface == NULL)
                            LogError("Drive has no Partition Manager!", window);
                        else if (partID < 0 || partID >= partInterface->partitionList.getCount())
                            LogError("Invalid Partition selected!", window);
                        else
                        {
                            FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partID]->fsInterface;
                            if (fsInterface == NULL)
                                LogError("Partition has no Filesystem!", window);
                            else
                            {
                                if (!fsInterface->FileExists(filename))
                                    LogError("The File was not found!", window);
                                else
                                {
                                    osData.diskInterfaces.add(new DiskInterface::FileDiskInterface(filename, fsInterface));
                                    window->renderer->Println("File Disk from file \"{}\" created!", filename, (*user)->colData.defaultTextColor);
                                }
                            }
                        }
                    }




                    //int size = to_int(data->data[3]);
                    //osData.diskInterfaces.add(new DiskInterface::RamDiskInterface(size));
                    //window->renderer->Println("Ram Disk with {} sectors created!", to_string(size), (*user)->colData.defaultTextColor);
                }
                else
                    LogError("No valid arguments passed!", window);
            }
            else if (StrEquals(data->data[2], "partition"))
            {
                int diskNum = to_int(data->data[1]);
                if (StrEquals(data->data[4], "fs"))
                {
                    int partNum = to_int(data->data[3]);
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface == NULL)
                        LogError("Drive has no Partition Manager!", window);
                    else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                        LogError("Invalid Partition selected!", window);
                    else
                    {
                        if (StrEquals(data->data[5], "create"))
                        {
                            FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)new FilesystemInterface::MrafsFilesystemInterface(partInterface, partInterface->partitionList[partNum]);

                            if (fsInterface != NULL)
                                window->renderer->Println("Filesystem Interface Creation Success!");
                            else
                                LogError("Filesystem Interface Creation failed!", window);
                        }
                        else
                        {
                            FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                            if (fsInterface != NULL)
                            {
                                if (StrEquals(data->data[5], "init"))
                                {
                                    const char* res = fsInterface->InitAndSaveFSTable();
                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                        window->renderer->Println("Filesystem Interface Init Success!");
                                    else
                                        LogError("Filesystem Interface Init failed! Error: {}", res, window);
                                }
                                else if (StrEquals(data->data[5], "save"))
                                {
                                    const char* res = fsInterface->SaveFSTable();
                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                        window->renderer->Println("Filesystem Interface Save Success!");
                                    else
                                        LogError("Filesystem Interface Save failed! Error: {}", res, window);
                                }
                                else if (StrEquals(data->data[5], "load"))
                                {
                                    const char* res = fsInterface->LoadFSTable();
                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                        window->renderer->Println("Filesystem Interface Load Success!");
                                    else
                                        LogError("Filesystem Interface Load failed! Error: {}", res, window);
                                }
                                else if (StrEquals(data->data[5], "list"))
                                {
                                    AddToStack();
                                    const char* res = fsInterface->LoadFSTable();
                                    RemoveFromStack();
                                    if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    {
                                        //window->renderer->Println("Filesystem Interface Load Success!");
                                        
                                        if (fsInterface->InterfaceType == FilesystemInterface::FilesystemInterfaceType::Mrafs)
                                        {
                                            FilesystemInterface::MrafsFilesystemInterface* mrafsInterface = (FilesystemInterface::MrafsFilesystemInterface*)fsInterface;

                                            AddToStack();
                                            {
                                                uint64_t partCount = mrafsInterface->fsPartitionList.getCount();
                                                window->renderer->Println("Partition Count: {}", to_string(partCount), Colors.yellow);
                                                window->renderer->Println("Partition Data:", Colors.yellow);
                                                for (int i = 0; i < partCount; i++)
                                                {   
                                                    FilesystemInterface::MrafsFilesystemInterface::FSPartitionInfo* info = (FilesystemInterface::MrafsFilesystemInterface::FSPartitionInfo*)mrafsInterface->fsPartitionList[i];
                                                    window->renderer->Println(" + Partition {}:", to_string(i), Colors.orange);
                                                    window->renderer->Println("    - Free:        {}", info->free ? "true" : "false", Colors.yellow);
                                                    window->renderer->Println("    - Location:    0x{}", ConvertHexToString((uint64_t)info->locationInBytes), Colors.yellow);
                                                    window->renderer->Println("    - Size:        {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                                                }
                                            }
                                            RemoveFromStack();
                                            
                                            AddToStack();
                                            {
                                                uint64_t partCount = mrafsInterface->fsFileList.getCount();
                                                window->renderer->Println("File Count: {}", to_string(partCount), Colors.yellow);
                                                window->renderer->Println("File Data:", Colors.yellow);
                                                for (int i = 0; i < partCount; i++)
                                                {   
                                                    FilesystemInterface::FileInfo* info = mrafsInterface->fsFileList[i];
                                                    window->renderer->Println(" + File {}:", to_string(i), Colors.orange);
                                                    window->renderer->Println("    - Path:        \"{}\"", info->baseInfo->path, Colors.yellow);
                                                    window->renderer->Println("    - Hidden:      {}", info->baseInfo->hidden ? "true" : "false", Colors.yellow);
                                                    window->renderer->Println("    - System File: {}", info->baseInfo->systemFile ? "true" : "false", Colors.yellow);
                                                    window->renderer->Println("    - Readonly:    {}", info->baseInfo->writeProtected ? "true" : "false", Colors.yellow);

                                                    window->renderer->Println("    - Location:    0x{}", ConvertHexToString((uint64_t)info->locationInBytes), Colors.yellow);
                                                    window->renderer->Println("    - Size:        {} Bytes", to_string(info->sizeInBytes), Colors.yellow);
                                                }
                                            }
                                            RemoveFromStack();
                                            
                                            AddToStack();
                                            {
                                                uint64_t partCount = mrafsInterface->fsFolderList.getCount();
                                                window->renderer->Println("Folder Count: {}", to_string(partCount), Colors.yellow);
                                                window->renderer->Println("Folder Data:", Colors.yellow);
                                                for (int i = 0; i < partCount; i++)
                                                {   
                                                    FilesystemInterface::FolderInfo* info = mrafsInterface->fsFolderList[i];
                                                    window->renderer->Println(" + Folder {}:", to_string(i), Colors.orange);
                                                    window->renderer->Println("    - Path:        \"{}\"", info->baseInfo->path, Colors.yellow);
                                                    window->renderer->Println("    - Hidden:      {}", info->baseInfo->hidden ? "true" : "false", Colors.yellow);
                                                    window->renderer->Println("    - System File: {}", info->baseInfo->systemFile ? "true" : "false", Colors.yellow);
                                                    window->renderer->Println("    - Readonly:    {}", info->baseInfo->writeProtected ? "true" : "false", Colors.yellow);
                                                }
                                            }
                                            RemoveFromStack();
                                        }
                                        else
                                        {
                                            LogError("Filesystem Interface is not MRAFS! Error: {}", res, window);
                                        }
                                        

                                    }
                                    else
                                        LogError("Filesystem Interface Load failed! Error: {}", res, window);
                                }
                                else
                                    LogError("No valid arguments passed!", window);
                            }
                            else
                                LogError("Filesystem Interface not found!", window);
                        }
                    }
                }
                else
                {
                    int partNum = to_int(data->data[4]);
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    if (partInterface == NULL)
                        LogError("Drive has no Partition Manager!", window);
                    else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                        LogError("Invalid Partition selected!", window);
                    else
                    {
                        if (StrEquals(data->data[3], "resize"))
                        {
                            uint64_t newSize = to_int(data->data[5]);
                            const char* res = partInterface->ResizePartition(partInterface->partitionList[partNum], newSize);
                            if (res == PartitionInterface::CommandResult.SUCCESS)
                                window->renderer->Println("Partition Resize Success!", (*user)->colData.defaultTextColor);
                            else
                                LogError("Partition Resize failed! Error: \"{}\"", res, window);

                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                }
            }
            else if (StrEquals(data->data[2], "write"))
            {   
                int num = to_int(data->data[1]);
                int start = to_int(data->data[4]);
                int count = to_int(data->data[5]);
                
                uint8_t val = data->data[3][0];
                if (StrEquals(data->data[3], "NULL"))
                    val = 0;

                // disk 0 write A 10 52
                uint8_t* buffer = (uint8_t*)malloc(count, "Malloc for disk read buffer");
                _memset(buffer, val, count);

                if (osData.diskInterfaces[num]->WriteBytes(start, count, (void*)buffer))
                {
                    window->renderer->Println("Disk write successful!");
                }
                else
                {
                    LogError("Disk write failed!", window);
                }


                free(buffer);
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 7)
        {
            int diskNum = to_int(data->data[1]);
            if (StrEquals(data->data[4], "fs"))
            {
                int partNum = to_int(data->data[3]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                { 
                    FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                    if (fsInterface != NULL)
                    {
                        if (StrEquals(data->data[5], "read"))
                        {
                            uint8_t* buffer = NULL;
                            uint64_t byteCount = fsInterface->ReadFile(data->data[6], (void**)&buffer);
                            window->renderer->Println("Size: {} Bytes", to_string(byteCount), (*user)->colData.defaultTextColor);
                            window->renderer->Println("Raw Data:");
                            for (uint64_t i = 0; i < byteCount; i++)
                                window->renderer->Print(buffer[i]);
                            window->renderer->Println();
                            if (byteCount != 0)
                                free(buffer);
                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                    else
                        LogError("Filesystem Interface not found!", window);
                }
            }
            else if (StrEquals(data->data[2], "partition"))
            {
                int partNum = to_int(data->data[4]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                {
                    if (StrEquals(data->data[3], "create"))
                    {
                        int newSize = to_int(data->data[5]);
                        PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[partNum];
                        const char* res = partInterface->CreatePartition(partInfo, newSize);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            free((void*)partInfo->name);
                            partInfo->name = StrCopy(data->data[6]);
                            partInfo->nameLen = StrLen(data->data[6]);

                            window->renderer->Println("Partition Creation Success!", (*user)->colData.defaultTextColor);
                        }
                        else
                            LogError("Partition Creation failed! Error: \"{}\"", res, window);
                    }
                    else if (StrEquals(data->data[3], "edit"))
                    {
                        const char* param = data->data[5];
                        const char* val = data->data[6];
                        // EDIT PARAM VAL
                        EditPartitionSetting(partInterface->partitionList[partNum], param, val, *user, window);
                    }
                    else if (StrEquals(data->data[3], "read"))
                    {
                        uint64_t addr = to_int(data->data[5]);
                        uint64_t count = to_int(data->data[6]);
                        uint8_t* buffer = (uint8_t*)malloc(count, "Buffer For Partition Read Command");

                        const char* res = partInterface->ReadPartition(partInterface->partitionList[partNum], addr, count, buffer);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                        {
                            window->renderer->Println("Raw Data:");
                            for (int i = 0; i < count; i++)
                                window->renderer->Print(buffer[i]);
                            window->renderer->Println();
                        }
                        else
                            LogError("Partition Read failed! Error: \"{}\"", res, window);
                        
                        free(buffer);
                    }
                    else if (StrEquals(data->data[3], "write"))
                    {
                        uint64_t addr = to_int(data->data[6]);
                        const char* buffer = data->data[5];

                        const char* res = partInterface->WritePartition(partInterface->partitionList[partNum], addr, StrLen(buffer), (void*)buffer);
                        if (res == PartitionInterface::CommandResult.SUCCESS)
                            window->renderer->Println("Partition Write Success!");
                        else
                            LogError("Partition Write failed! Error: \"{}\"", res, window);
                    }
                    else
                        LogError("No valid arguments passed!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 8)
        {
            int diskNum = to_int(data->data[1]);
            if (StrEquals(data->data[4], "fs"))
            {
                int partNum = to_int(data->data[3]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                { 
                    FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                    if (fsInterface != NULL)
                    {
                        if (StrEquals(data->data[5], "create"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->CreateFile(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("File Creation Success!");
                                else
                                    LogError("File Creation failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                const char* res = fsInterface->CreateFolder(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("Folder Creation Success!");
                                else
                                    LogError("Folder Creation failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "delete"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->DeleteFile(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("File Deletion Success!");
                                else
                                    LogError("File Deletion failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                const char* res = fsInterface->DeleteFolder(data->data[7]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("Folder Deletion Success!");
                                else
                                    LogError("Folder Deletion failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "read"))
                        {
                            uint64_t byteCount = to_int(data->data[7]);
                            uint8_t* buffer = (uint8_t*)malloc(byteCount, "Malloc for File read");
                            const char* res = fsInterface->ReadFile(data->data[6], byteCount, buffer);
                            if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                            {
                                window->renderer->Println("Raw Data:");
                                for (uint64_t i = 0; i < byteCount; i++)
                                    window->renderer->Print(buffer[i]);
                                window->renderer->Println();
                            }
                            else
                                LogError("File Read failed! Error: \"{}\"", res, window);
                            free(buffer);
                        }
                        else if (StrEquals(data->data[5], "write"))
                        {
                            uint64_t byteCount = StrLen(data->data[7]);
                            const char* res = fsInterface->WriteFile(data->data[6], byteCount, data->data[7]);
                            if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                window->renderer->Println("File Write Success!");
                            else
                                LogError("File Write failed! Error: \"{}\"", res, window);
                        }
                        else if (StrEquals(data->data[5], "exists"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                if (fsInterface->FileExists(data->data[7]))
                                    window->renderer->Println("File exists!");
                                else
                                    window->renderer->Println("File doesn't exists!");
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                if (fsInterface->FolderExists(data->data[7]))
                                    window->renderer->Println("Folder exists!");
                                else
                                    window->renderer->Println("Folder doesn't exists!");
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "get"))
                        {
                            if (StrEquals(data->data[6], "files"))
                            {
                                uint64_t fCount = 0;
                                const char** files = fsInterface->GetFiles(data->data[7], &fCount);
                                window->renderer->Println("Files: (Count: {})", to_string(fCount), Colors.yellow);
                                for (int i = 0; i < fCount; i++)
                                {
                                    window->renderer->Println(" - \"{}\"", files[i], Colors.yellow);
                                    free((void*)files[i]);
                                }
                                if (fCount != 0)
                                    free(files);
                            }
                            else if (StrEquals(data->data[6], "folders"))
                            {
                                uint64_t fCount = 0;
                                const char** folders = fsInterface->GetFolders(data->data[7], &fCount);
                                window->renderer->Println("Folders: (Count: {})", to_string(fCount), Colors.yellow);
                                for (int i = 0; i < fCount; i++)
                                {
                                    window->renderer->Println(" - \"{}\"", folders[i], Colors.yellow);
                                    free((void*)folders[i]);
                                }
                                if (fCount != 0)
                                    free(folders);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                    else
                        LogError("Filesystem Interface not found!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else if (data->len == 9)
        {
            int diskNum = to_int(data->data[1]);
            if (StrEquals(data->data[4], "fs"))
            {
                int partNum = to_int(data->data[3]);
                DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[diskNum];
                PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                if (partInterface == NULL)
                    LogError("Drive has no Partition Manager!", window);
                else if (partNum < 0 || partNum >= partInterface->partitionList.getCount())
                    LogError("Invalid Partition selected!", window);
                else
                { 
                    FilesystemInterface::GenericFilesystemInterface* fsInterface = (FilesystemInterface::GenericFilesystemInterface*)partInterface->partitionList[partNum]->fsInterface;
                    if (fsInterface != NULL)
                    {
                        if (StrEquals(data->data[5], "create"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                int64_t size = to_int(data->data[8]);
                                const char* res = fsInterface->CreateFile(data->data[7], size);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("File with a size of {} bytes got created successfully!", data->data[8], (*user)->colData.defaultTextColor);
                                else
                                    LogError("File Creation failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "copy"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->CopyFile(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("File Copying Success!");
                                else
                                    LogError("File Copying failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                const char* res = fsInterface->CopyFolder(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("Folder Copying Success!");
                                else
                                    LogError("Folder Copying failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else if (StrEquals(data->data[5], "rename"))
                        {
                            if (StrEquals(data->data[6], "file"))
                            {
                                const char* res = fsInterface->RenameFile(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("File Renaming Success!");
                                else
                                    LogError("File Renaming failed! Error: \"{}\"", res, window);
                            }
                            else if (StrEquals(data->data[6], "folder"))
                            {
                                //osData.debugTerminalWindow->renderer->Cls();
                                //osData.debugTerminalWindow->Log("---Rename Folder Debug---");
                                const char* res = fsInterface->RenameFolder(data->data[7], data->data[8]);
                                if (res == FilesystemInterface::FSCommandResult.SUCCESS)
                                    window->renderer->Println("Folder Renaming Success!");
                                else
                                    LogError("Folder Renaming failed! Error: \"{}\"", res, window);
                            }
                            else
                                LogError("No valid arguments passed!", window);
                        }
                        else
                            LogError("No valid arguments passed!", window);
                    }
                    else
                        LogError("Filesystem Interface not found!", window);
                }
            }
            else
                LogError("No valid arguments passed!", window);
        }
        else
        {
            LogError("No valid arguments passed!", window);
        }
        
        free(data);
        RemoveFromStack();
        return;
    }


    LogError("Unknown command \"{}\"!", data->data[0], window);
    free(data);
    RemoveFromStack();
    return;
}


void login(const char* name, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;

    OSUser* usr = getUser(name);
    if (usr == 0)
    {
        LogError("User \"{}\" was not found!", name, window);
        RemoveFromStack();
        return;
    }

    if (StrEquals(usr->password, ""))
        *user = usr;
    else
    {
        window->renderer->Println("Please enter the password down below:");
        terminal->mode = commandMode::enterPassword;
    }
    RemoveFromStack();
}

void login(const char* name, const char* pass, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;

    terminal->mode = commandMode::none;

    OSUser* usr = getUser(name);
    if (usr == 0)
    {
        LogError("User \"{}\" was not found!", name, window);
        RemoveFromStack();
        return;
    }

    if (StrEquals(usr->password, pass))
        *user = usr;
    else
        LogError("Password is incorrect!", name, window); 
    RemoveFromStack();
}

void SetCmd(const char* name, const char* val, OSUser** user, Window* window)
{
    AddToStack();

    if (window->instance == NULL)
    {
        RemoveFromStack();
        return;
    }

    TerminalInstance* terminal = (TerminalInstance*)window->instance;

    if (StrEquals(name, "user color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            (*user)->colData.userColor = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "default color"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            window->renderer->color = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "username"))
    {
        (*user)->userName = StrCopy(val);
    }
    else if (StrEquals(name, "amfv") || StrEquals(name, "active memory flag value"))
    {
        activeMemFlagVal= to_int(val);
    }
    else if (StrEquals(name, "mnfcc") || StrEquals(name, "max non fatal crash count"))
    {
        osData.maxNonFatalCrashCount = to_int(val);
    }
    else if (StrEquals(name, "debug terminal"))
    {
        if (StrEquals(val, "on") || StrEquals(val, "shown"))
        {
            osData.showDebugterminal = true;
            osData.windowPointerThing->UpdatePointerRect(
                osData.debugTerminalWindow->position.x - 1, 
                osData.debugTerminalWindow->position.y - 23, 
                osData.debugTerminalWindow->position.x + osData.debugTerminalWindow->size.width, 
                osData.debugTerminalWindow->position.y + osData.debugTerminalWindow->size.height
                );
        }
        else if (StrEquals(val, "off") || StrEquals(val, "hidden"))
        {
            osData.showDebugterminal = false;
            osData.windowPointerThing->UpdatePointerRect(
                osData.debugTerminalWindow->position.x - 1, 
                osData.debugTerminalWindow->position.y - 23, 
                osData.debugTerminalWindow->position.x + osData.debugTerminalWindow->size.width, 
                osData.debugTerminalWindow->position.y + osData.debugTerminalWindow->size.height
                );
        }
        else
            LogError("value has to be \"on\", \"shown\", \"off\" or \"hidden\"!", window);
    }
    else if (StrEquals(name, "background image"))
    {
        osData.drawBackground = StrEquals(val, "true") || StrEquals(val, "on");
        if (!osData.drawBackground)
        {
            if (!(StrEquals(val, "false") || StrEquals(val, "off")))
                LogError("Value \"{}\" has to be \"true\", \"false\", \"on\" or \"off\"!", val, window); 
        }
    }
    else if (StrEquals(name, "mouse offset"))
    {
        mouseCycleSkip = to_int(val);
    }
    else if (StrEquals(name, "test interlace"))
    {
        int interlace = to_int(val);
        if (interlace < 1)
            LogError("Interlace cannot be less than 1", window);
        else if (interlace > 8)
            LogError("Interlace cannot be higher than 8", window);
        else
        {
            int inter = 1;
            for (int i = 0; i < interlace - 1; i++)
                inter *= 2;
            
            WindowManager::testInterlace = inter;
        }
    }
    else if (StrEquals(name, "mouse image"))
    {
        currentMouseImageName = StrCopy(val);
        if (kernelFiles::ZIP::GetFileFromFileName(mouseZIP, currentMouseImageName) == NULL)
            LogError("Mouse Image File doesn't exist! Setting to default...", window);
    }
    //mouseImage = kernelFiles::ConvertFileToImage(kernelFiles::ZIP::GetFileFromFileName(bootInfo->mouseZIP, "default.mbif"));
    else if (StrEquals(name, "password"))
    {
        terminal->mode = commandMode::enterPassword;
        window->renderer->Println("Please enter the new password down below:");
    }
    else if (StrEquals(name, "mouse color front"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            mouseColFront = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "mouse color back"))
    {
        ParsedColData data = ParseColor(val);
        if (data.parseSuccess)
            mouseColBack = data.col;
        else
            LogError("Color \"{}\" could not be Parsed!", val, window);
    }
    else if (StrEquals(name, "window resolution") || StrEquals(name, "window size"))
    {
        StringArrData* split = SplitLine(val);
        if (split->len == 2)
        {
            int x = to_int(split->data[0]);
            int y = to_int(split->data[1]);

            if (x >= 10 && y >= 10)
            {
                int x1 = window->position.x - 1;
                int y1 = window->position.y - 23;
                int sx1 = window->size.width + 3;
                int sy1 = window->size.height + 25;

                window->Resize(Size(x, y));

                int x2 = window->position.x - 1;
                int y2 = window->position.y - 23;
                int sx2 = window->size.width + 3;
                int sy2 = window->size.height + 25;

                {
                    osData.windowPointerThing->UpdatePointerRect(x1, y1, x2 + sx2, y2 + sy2);

                    osData.windowPointerThing->UpdatePointerRect(x2, y2 + sy2, x1 + sx1, y1 + sy1);

                    osData.windowPointerThing->UpdatePointerRect(x2 + sx2, y1, x1 + sx1, y2 + sy2);

                    osData.windowPointerThing->UpdatePointerRect(x1, y2, x2, y1 + sy1);

                    osData.windowPointerThing->RenderWindow(window);
                }

            }
            else
                LogError("Invalid size given!", window);
        }
        else
            LogInvalidArgumentCount(2, split->len, window);

        free((void*)split);
    }
    else
    {
        LogError("Parameter \"{}\" does not exist.", name, window);
    }
    RemoveFromStack();
}



void GetCmd(const char* name, OSUser* user, Window* window)
{
    AddToStack();
    if (StrEquals(name, "free ram"))
    {
        window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    }
    else if (StrEquals(name, "free pages"))
    {
        window->renderer->Println("Free Page Count: {} pages.", to_string(GlobalAllocator->GetFreePageCount()), Colors.bgreen);
    }
    else if (StrEquals(name, "window resolution") || StrEquals(name, "window size"))
    {
        dispVar vars[] = {dispVar(window->size.width), dispVar(window->size.height)};
        window->renderer->Println("Window Resolution: {0}x{1}.", vars);
    }
    else if (StrEquals(name, "screen resolution"))
    {
        dispVar vars[] = {dispVar((uint64_t)GlobalRenderer->framebuffer->Width), dispVar((uint64_t)GlobalRenderer->framebuffer->Height)};
        window->renderer->Println("Screen Resolution: {0}x{1}.", vars);
    }
    else if (StrEquals(name, "mnfcc") || StrEquals(name, "max non fatal crash count"))
    {
        window->renderer->Println("Max non-fatal crash count: {} crashes..", to_string(osData.maxNonFatalCrashCount), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "stack trace"))
    {
        PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer, window->renderer, user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "TSB S"))
    {
        window->renderer->Println("Time since boot: {} s.", to_string(PIT::TimeSinceBootS()), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "TSB MS"))
    {
        window->renderer->Println("Time since boot: {} ms.", to_string(PIT::TimeSinceBootMS()), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "disk count"))
    {
        window->renderer->Println("Disk Count: {}", to_string(osData.diskInterfaces.getCount()), user->colData.defaultTextColor);
    }
    else if (StrEquals(name, "heap stats"))
    {
        window->renderer->Println("Heap Statistics:", Colors.yellow);
        {
            uint64_t totalSegCount = 0, freeSegCount = 0, usedSegCount = 0, totalSegSpace = 0, freeSegSpace = 0, usedSegSpace = 0; 
            for (HeapSegHdr* current = (HeapSegHdr*) heapStart; current != NULL; current = current->next)
            {
                if (current->free)
                {
                    freeSegCount += 1;
                    freeSegSpace += current->length;
                }
                else
                {
                    usedSegCount += 1;
                    usedSegSpace += current->length;
                }

                totalSegCount += 1;
                totalSegSpace += current->length;
            }
            window->renderer->Println("> Total Amount of Heapsegments: {}", to_string(totalSegCount), Colors.yellow);
            window->renderer->Println("> Amount of Free Heapsegments:  {}", to_string(freeSegCount), Colors.yellow);
            window->renderer->Println("> Amount of Used Heapsegments:  {}", to_string(usedSegCount), Colors.yellow);
            window->renderer->Println();
            window->renderer->Println("> Total Heap Space:             {} Bytes", to_string(totalSegSpace), Colors.yellow);
            window->renderer->Println("> Free Heap Space:              {} Bytes", to_string(freeSegSpace), Colors.yellow);
            window->renderer->Println("> Used Heap Space:              {} Bytes", to_string(usedSegSpace), Colors.yellow);
        }
    }
    else if (StrEquals(name, "heap stats detail"))
    {
        window->renderer->Println("Heap Statistics (Detailed):", Colors.yellow);
        {
            uint64_t totalSegCount = 0, freeSegCount = 0, usedSegCount = 0, totalSegSpace = 0, freeSegSpace = 0, usedSegSpace = 0; 
            for (HeapSegHdr* current = (HeapSegHdr*) heapStart; current != NULL; current = current->next)
            {
                dispVar vars[] = {dispVar(totalSegCount), dispVar(current->length), dispVar(current->text), dispVar(current->activeMemFlagVal)};
                uint32_t col = Colors.gray;
                if (current->free)
                {
                    freeSegCount += 1;
                    freeSegSpace += current->length;
                    if (current->activeMemFlagVal == activeMemFlagVal && activeMemFlagVal != 0)
                        col = Colors.bgreen;
                    else
                        col = Colors.green;
                }
                else
                {
                    usedSegCount += 1;
                    usedSegSpace += current->length;
                    if (current->activeMemFlagVal == activeMemFlagVal && activeMemFlagVal != 0)
                        col = Colors.yellow;
                    else
                        col = Colors.bred;
                }
                
                window->renderer->Println("> Heap# {0} - Size: {1} Bytes, Title: \"{2}\", AMFV: {3}", vars, col);

                totalSegCount += 1;
                totalSegSpace += current->length;
            }
            window->renderer->Println();
            window->renderer->Println("-----------------------------------");
            window->renderer->Println();
            window->renderer->Println("> Total Amount of Heapsegments: {}", to_string(totalSegCount), Colors.yellow);
            window->renderer->Println("> Amount of Free Heapsegments:  {}", to_string(freeSegCount), Colors.yellow);
            window->renderer->Println("> Amount of Used Heapsegments:  {}", to_string(usedSegCount), Colors.yellow);
            window->renderer->Println();
            window->renderer->Println("> Total Heap Space:             {} Bytes", to_string(totalSegSpace), Colors.yellow);
            window->renderer->Println("> Free Heap Space:              {} Bytes", to_string(freeSegSpace), Colors.yellow);
            window->renderer->Println("> Used Heap Space:              {} Bytes", to_string(usedSegSpace), Colors.yellow);
        }
    }
    else
    {
        LogError("Parameter \"{}\" does not exist.", name, window);
    }

    // window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.bgreen);
    // window->renderer->Println("");
    RemoveFromStack();
}

ParsedColData ParseColor(const char* col)
{
    AddToStack();
    ParsedColData data = ParsedColData();
    data.parseSuccess = false;
    data.col = 0;

    //window->renderer->Println("Col: \"{}\" (", col, Colors.orange);
    //window->renderer->Print(col[0]);
    //window->renderer->Println(")", Colors.orange);

    if (col[0] == '#')
    {
        const char* subStr = StrSubstr(col, 1);
        //window->renderer->Println("Col: \"{}\"", subStr, Colors.cyan);
        data.col = ConvertStringToHex(subStr);
        data.parseSuccess = true;
        RemoveFromStack();
        return data;
    }

    for (int i = 0; i < colCount; i++)
        if (StrEquals(col, colNames[i]))
        {
            data.col = colValues[i];
            //window->renderer->Println("Color 1: {}", ConvertHexToString(colValues[i]), Colors.white);
            data.parseSuccess = true;
            break;
        }
    
    //window->renderer->Println("Color 2: {}", ConvertHexToString(data.col), Colors.white);

    RemoveFromStack();
    return data;
}

/*
    //window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
    StringArrData* data = SplitLine(input);
    //window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);


    window->renderer->Println("Parts:");
    for (int i = 0; i < data->len; i++)
        window->renderer->Println(" - \"{}\"", data->data[i], Colors.bgreen);


    //free(splitLine);
    GlobalAllocator->FreePage(data);
    //window->renderer->Println("Free: {} Bytes.", to_string(GlobalAllocator->GetFreeRAM()), Colors.pink);
*/

StringArrData* SplitLine(const char* input)
{
    AddToStack();
    uint64_t index = 0;
    uint64_t parts[256];
    uint64_t partIndex = 0;

    for (int i = 0; i < 256; i++)
        parts[i] = 0;

    bool inString = false;

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
            partIndex++;
        else 
        {
            parts[partIndex]++;
            if (input[index] == '\\')
                if (input[index + 1] != 0)
                    index++;
        }
    }

    int partCount = partIndex + 1;

    int totalsize = sizeof(char**) + sizeof(StringArrData);
    for (int i = 0; i < partCount; i++)
    {
        totalsize += parts[i] + 1 + sizeof(char*);
    }

    //uint64_t datAddr = (uint64_t) GlobalAllocator->RequestPage();
    int64_t datAddr = (uint64_t) malloc(totalsize, "Space for split command parameters");
    StringArrData* data = (StringArrData*) datAddr;

    //char** splitLine = (char**) GlobalAllocator->RequestPage(); //(char**)calloc(partCount, sizeof(char*));

    //window->renderer->Println("ADDR OG:      {}",ConvertHexToString((uint64_t)data),Colors.cyan);

    //window->renderer->Println("ADDR datAddr: {}",ConvertHexToString(datAddr),Colors.bgreen);

    data->addrOfData = (void*)(datAddr + sizeof(StringArrData));

    data->data = (char**)data->addrOfData;


    //window->renderer->Println("ADDR of Data 1: {}",ConvertHexToString((uint64_t)data->data),Colors.cyan);
    //window->renderer->Println("ADDR of Data 2: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    char** splitLine = data->data;
    data->len = partCount;

    // window->renderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    // window->renderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine), Colors.yellow);

    uint64_t splitIndex = sizeof(char**) + sizeof(char*) * partCount;

    //window->renderer->Println("ADDR of Data 3: {}",ConvertHexToString((uint64_t)data->addrOfData),Colors.cyan);

    for (int i = 0; i < partCount; i++)
    {
        //window->renderer->Println("INDEX  {}", to_string((uint64_t)splitIndex), Colors.cyan); 
        //window->renderer->Println("Count: {}", to_string((uint64_t)parts[i]), Colors.cyan);
        splitLine[i] = (char*)((uint64_t)data->addrOfData + splitIndex);
        //window->renderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)&splitLine[i][0]), Colors.yellow);

        for (int i2 = 0; i2 < parts[i] + 1; i2++)
            splitLine[i][i2] = 0;
       
        //window->renderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)splitLine[i]), Colors.yellow);
        //window->renderer->Println("ADDR 3: {}", ConvertHexToString((uint64_t)data->addrOfData + splitIndex), Colors.yellow);
        splitIndex += parts[i] + 1;
    }

    // ---------------------------------------------------------------------------------------------------------------------------------

    inString = false;
    partIndex = 0;
    index = 0;

    for (int i = 0; i < 100; i++)
        parts[i] = 0;
    
    //window->renderer->Println("AAA");
    //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);

    //window->renderer->Println("Lines:", Colors.yellow);

    for (; input[index] != 0; index++)
    {
        if (input[index] == '\"')
            inString = !inString;
        else if (!inString && input[index] == ' ')
        {
            //window->renderer->Println();
            //window->renderer->Println("PartIndex:   {}", to_string(partIndex), Colors.cyan);
            //window->renderer->Println("PartIndex 2: {}", to_string(parts[partIndex]), Colors.cyan);
            //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
            //window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
            splitLine[partIndex][parts[partIndex]] = 0;
            //window->renderer->Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
            //window->renderer->Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);
            partIndex++;
        }
        else 
        {
            if (input[index] == '\\')
                if (input[index + 1] != 0)
                    index++;
            
            //splitLine[partIndex][parts[partIndex]] = input[index];
            //*(splitLine[partIndex] + parts[partIndex]) = input[index];
            //window->renderer->Print(input[index]);
            //window->renderer->Println("Index:           {}", to_string(index), Colors.bgreen);
            //window->renderer->Println("Part Index:      {}", to_string(partIndex), Colors.bgreen);
            //window->renderer->Println("Part Part Index: {}", to_string(parts[partIndex]), Colors.bgreen);

            splitLine[partIndex][parts[partIndex]] = input[index];

            parts[partIndex]++;
        }
    }

    ///window->renderer->Println(" - \"{}\"", (const char*)splitLine[partIndex], Colors.yellow);
    // window->renderer->Println();
    // window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)splitLine[partIndex]), Colors.orange);
    // window->renderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&splitLine[partIndex][parts[partIndex]]), Colors.orange);
    // window->renderer->Print(" - \"{}\"", splitLine[partIndex], Colors.yellow);
    // window->renderer->Println(", Count: {}", to_string((uint64_t)parts[partIndex] + 1), Colors.yellow);

    RemoveFromStack();
    return data;
}