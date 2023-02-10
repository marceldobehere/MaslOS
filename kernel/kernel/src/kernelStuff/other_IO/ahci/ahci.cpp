#include "ahci.h"
#include "../../../OSDATA/osdata.h"
#include "../../../paging/PageTableManager.h"
#include "../../../memory/heap.h"
#include "../../memory/memory.h"
#include "../../Disk_Stuff/Disk_Interfaces/sata/sataDiskInterface.h"
#include "../../Disk_Stuff/Partition_Interfaces/generic/genericPartitionInterface.h"
#include "../../Disk_Stuff/Partition_Interfaces/mraps/mrapsPartitionInterface.h"
#include "../../Disk_Stuff/Filesystem_Interfaces/generic/genericFileSystemInterface.h"
#include "../../Disk_Stuff/Filesystem_Interfaces/mrafs/mrafsFileSystemInterface.h"

namespace AHCI 
{
    #define HBA_PORT_DEV_PRESENT 0x3
    #define HBA_PORT_IPM_ACTIVE  0x1
    #define HBA_PxCMD_CR  0x8000
    #define HBA_PxCMD_FRE 0x0010
    #define HBA_PxCMD_ST  0x0001
    #define HBA_PxCMD_FR  0x4000
    #define HBA_PxIS_TFES (1 << 30)

    void Port::Configure()
    {
        StopCMD();

        void* newBase = GlobalAllocator->RequestPage();
        GlobalPageTableManager.MapMemory(newBase, newBase, false);
        hbaPort->commandListBase = (uint32_t)(uint64_t)newBase;
        hbaPort->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
        _memset((void*)(uint64_t)hbaPort->commandListBase, 0, 1024);

        void* fisBase = GlobalAllocator->RequestPage();
        GlobalPageTableManager.MapMemory(fisBase, fisBase, false);
        hbaPort->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
        hbaPort->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
        _memset(fisBase, 0, 256);
        
        HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)hbaPort->commandListBase + ((uint64_t)hbaPort->commandListBaseUpper << 32));

        for (int i = 0; i < 32; i++)
        {
            cmdHeader[i].prdtLength = 8;

            void* cmdTableAddress = GlobalAllocator->RequestPage();
            GlobalPageTableManager.MapMemory(cmdTableAddress, cmdTableAddress, false);
            uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
            cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t)address;
            cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
            _memset(cmdTableAddress, 0, 256);
        }

        StartCMD();
    }

    void Port::StopCMD()
    {
        hbaPort->cmdStatus &= ~HBA_PxCMD_ST;
        hbaPort->cmdStatus &= ~HBA_PxCMD_FRE;

        while (true)
        {
            if (hbaPort->cmdStatus & HBA_PxCMD_FR)
                continue;
            if (hbaPort->cmdStatus & HBA_PxCMD_CR)
                continue;
            break;
        }
    }

    void Port::StartCMD()
    {
        while (hbaPort->cmdStatus & HBA_PxCMD_CR);

        hbaPort->cmdStatus |= HBA_PxCMD_FRE;
        hbaPort->cmdStatus |= HBA_PxCMD_ST;
    }

    SATA_Ident Port::Identifydrive()
    {
        AddToStack();
        /***Make the Command Header***/
        HBACommandHeader* cmdhead=(HBACommandHeader*)(uint64_t)hbaPort->commandListBase;//kmalloc(sizeof(HBA_CMD_HEADER));
        //port->clb = (DWORD)cmdhead;
        //cmdhead->commandFISLenght = 5;
        //cmdhead->a=0;
        //_memset(cmdhead, 0, sizeof(HBACommandHeader));
        cmdhead->write = 0;
        cmdhead->prdtLength = 1;
        //cmdhead->prefetchable = 1; //p
        cmdhead->clearBusy = 1;
        RemoveFromStack();

        AddToStack();
        cmdhead->commandFISLenght = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // command FIS size
        cmdhead->prdtLength = 1;
        RemoveFromStack();

        AddToStack();
        /***Make the Command Table***/
        HBACommandTable* cmdtbl = (HBACommandTable*)((uint64_t)cmdhead->commandTableBaseAddress);//(HBACommandTable*)GlobalAllocator->RequestPage();// kmalloc(sizeof(HBA_CMD_TBL));
        //cmdhead->commandTableBaseAddress = (uint32_t)(uint64_t)cmdtbl;
        RemoveFromStack();

        AddToStack();
        //_memset((void*)cmdtbl, 0, sizeof(HBACommandTable));
        RemoveFromStack();

        AddToStack();
        cmdtbl->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)GlobalAllocator->RequestPage();
        //_memset((void*)(uint64_t)cmdtbl->prdtEntry[0].dataBaseAddress , 0, 0x1000);
        //GlobalPageTableManager.MapMemory((void*)(uint64_t)cmdtbl->prdtEntry[0].dataBaseAddress, (void*)(uint64_t)cmdtbl->prdtEntry[0].dataBaseAddress);
        RemoveFromStack();

        AddToStack();
        cmdtbl->prdtEntry[0].byteCount = 0x200 - 1;
        cmdtbl->prdtEntry[0].interruptOnCompletion = 1;   // interrupt when identify complete
        uint32_t data_base = cmdtbl->prdtEntry[0].dataBaseAddress;
        //_memset((void*)(uint64_t)data_base, 0, 4096);
        RemoveFromStack();


        AddToStack();
        /***Make the IDENTIFY DEVICE h2d FIS***/
        FIS_REG_H2D* cmdfis = (FIS_REG_H2D*)(uint64_t)cmdtbl->commandFIS;
        //printf("cmdfis %x ",cmdfis);
        _memset((void*)cmdfis,0,sizeof(FIS_REG_H2D));
        cmdfis->fisType = FIS_TYPE_REG_H2D;
        cmdfis->commandControl = 1;
        cmdfis->command = ATA_CMD_IDENTIFY;
        RemoveFromStack();

        AddToStack();
        /***Send the Command***/
        hbaPort->commandIssue = 1;

        /***Wait for a reply***/
        uint64_t s =  PIT::TimeSinceBootMS() + 3000;
        //GlobalRenderer->Clear(Colors.green);
        while(PIT::TimeSinceBootMS() < s)
        {
            if(hbaPort->commandIssue == 0)
                break;
        }
        RemoveFromStack();
        //if (PIT::TimeSinceBootMS() >= s)
        //    GlobalRenderer->Clear(Colors.red);

        AddToStack();
        uint32_t* baddr = (uint32_t*)(uint64_t)data_base;
        SATA_Ident test = *((SATA_Ident*)baddr);

        GlobalAllocator->FreePage((void*)(uint64_t)data_base);
        //GlobalAllocator->FreePage((void*)(uint64_t)cmdtbl);
        RemoveFromStack();

        return test;
    }

    int Port::FindCommandSlot()
    {
        uint32_t cmdSlots = 32;
        uint32_t slots = (hbaPort->sataControl | hbaPort->commandIssue);
        for (int i = 0; i < cmdSlots; i++)
        {
            if ((slots & 1) == 0)
                return i;
            slots >>= 1;
        }
        osData.debugTerminalWindow->Log("Coult not find free Command Slot!");
        return -1;
    }

    bool Port::Read(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        //osData.mainTerminalWindow->Log("This Port: 0x{}", ConvertHexToString((uint64_t)this), Colors.yellow);
        uint32_t sectorL = (uint32_t)sector;
        uint32_t sectorH = (uint32_t)(sector >> 32);
        uint32_t sectorCountCopy = sectorCount;
        
        hbaPort->interruptStatus = (uint32_t)-1;
        int slot = FindCommandSlot();
        if (slot == -1)
            return false;
        
        //osData.mainTerminalWindow->Log("This Slot: {}", to_string(slot), Colors.yellow);

        HBACommandHeader* cmdHeader = (HBACommandHeader*)(uint64_t)hbaPort->commandListBase;
        cmdHeader += slot;
        cmdHeader->commandFISLenght = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // command FIS size
        cmdHeader->write = 0;
        cmdHeader->prdtLength = ((sectorCount) / 16) + 1;

        HBACommandTable* commandTable = (HBACommandTable*)((uint64_t)cmdHeader->commandTableBaseAddress);
        _memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HBAPRDTEntry));

        int i = 0;
        for (i = 0; i < cmdHeader->prdtLength - 1; i++)
        {
            commandTable->prdtEntry[i].dataBaseAddress = (uint32_t)(uint64_t)buffer;
            commandTable->prdtEntry[i].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
            commandTable->prdtEntry[i].byteCount = 0x2000 - 1;
            commandTable->prdtEntry[i].interruptOnCompletion = 1;
            buffer = (uint8_t*)buffer + 0x2000;
            sectorCount -= 16;
        }

        commandTable->prdtEntry[i].dataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->prdtEntry[i].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->prdtEntry[i].byteCount = (sectorCount << 9) - 1; // 512 bytes per sector
        //osData.debugTerminalWindow->Log("Writing {} Bytes.", to_string((uint64_t)(commandTable->prdtEntry[i].byteCount + 1)), Colors.bgreen);
        commandTable->prdtEntry[i].interruptOnCompletion = 1;
        
        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);
        cmdFIS->fisType = FIS_TYPE_REG_H2D;
        cmdFIS->commandControl = 1;
        cmdFIS->command = ATA_CMD_READ_DMA_EX;

        // cmdFIS->lba0 = (uint8_t)sectorL;
        // cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
        // cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
        // cmdFIS->lba3 = (uint8_t)sectorH;
        // cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
        // cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

        cmdFIS->lba0 = (uint8_t)sectorL;
        cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
        cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
        cmdFIS->lba3 = (uint8_t)(sectorL >> 24);
        cmdFIS->lba4 = (uint8_t)sectorH;
        cmdFIS->lba5 = (uint8_t)(sectorH >> 8);

        cmdFIS->deviceRegister = 1<<6; // Set to LBA Mode

        cmdFIS->countLow = sectorCountCopy & 0xFF;
        cmdFIS->countHigh = (sectorCountCopy >> 8) & 0xFF;
        
        uint64_t spin = 0;
        while((hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
            spin++;
        if (spin == 1000000)
            return false;
        //osData.debugTerminalWindow->Log("Spin: {}", to_string(spin), Colors.bblue);

        hbaPort->commandIssue = 1<<slot;
        
        while (true)
        {
            if ((hbaPort->commandIssue & (1<<slot)) == 0)
                break;
            if (hbaPort->interruptStatus & HBA_PxIS_TFES) 
                return false;
        }

        if (hbaPort->interruptStatus & HBA_PxIS_TFES) 
                return false;

        return true;
    }
    bool Port::Write(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        uint32_t sectorL = (uint32_t)sector;
        uint32_t sectorH = (uint32_t)(sector >> 32);
        uint32_t sectorCountCopy = sectorCount;
        
        hbaPort->interruptStatus = (uint32_t)-1;
        int slot = FindCommandSlot();
        if (slot == -1)
            return false;

        HBACommandHeader* cmdHeader = (HBACommandHeader*)(uint64_t)hbaPort->commandListBase;
        cmdHeader += slot; // A
        cmdHeader->commandFISLenght = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // command FIS size
        cmdHeader->write = 1;
        cmdHeader->prdtLength = ((sectorCount) / 16) + 1;

        HBACommandTable* commandTable = (HBACommandTable*)((uint64_t)cmdHeader->commandTableBaseAddress);
        _memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HBAPRDTEntry));

        int i = 0;
        for (i = 0; i < cmdHeader->prdtLength - 1; i++)
        {
            commandTable->prdtEntry[i].dataBaseAddress = (uint32_t)(uint64_t)buffer;
            commandTable->prdtEntry[i].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
            commandTable->prdtEntry[i].byteCount = 0x2000 - 1;
            commandTable->prdtEntry[i].interruptOnCompletion = 1;
            buffer = (uint8_t*)buffer + 0x2000;
            sectorCount -= 16;
        }

        commandTable->prdtEntry[i].dataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->prdtEntry[i].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->prdtEntry[i].byteCount = (sectorCount << 9) - 1; // 512 bytes per sector
        //osData.debugTerminalWindow->Log("Writing {} Bytes.", to_string((uint64_t)(commandTable->prdtEntry[i].byteCount + 1)), Colors.bgreen);
        commandTable->prdtEntry[i].interruptOnCompletion = 1;
        
        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);
        cmdFIS->fisType = FIS_TYPE_REG_H2D;
        cmdFIS->commandControl = 1;
        cmdFIS->command = ATA_CMD_WRITE_DMA_EX;

        // cmdFIS->lba0 = (uint8_t)sectorL;
        // cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
        // cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
        // cmdFIS->lba3 = (uint8_t)sectorH;
        // cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
        // cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

        cmdFIS->lba0 = (uint8_t)sectorL;
        cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
        cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
        cmdFIS->lba3 = (uint8_t)(sectorL >> 24);
        cmdFIS->lba4 = (uint8_t)sectorH;
        cmdFIS->lba5 = (uint8_t)(sectorH >> 8);

        cmdFIS->deviceRegister = 1<<6; // Set to LBA Mode

        cmdFIS->countLow = sectorCountCopy & 0xFF;
        cmdFIS->countHigh = (sectorCountCopy >> 8) & 0xFF;
        
        uint64_t spin = 0;
        while((hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
            spin++;
        if (spin == 1000000)
            return false;
        //osData.debugTerminalWindow->Log("Spin: {}", to_string(spin), Colors.bblue);

        hbaPort->commandIssue = 1<<slot; // A
        
        while (true)
        {
            if ((hbaPort->commandIssue & (1<<slot)) == 0) // A
                break;
            if (hbaPort->interruptStatus & HBA_PxIS_TFES) 
                return false;
        }

        if (hbaPort->interruptStatus & HBA_PxIS_TFES) 
                return false;

        return true;
    }

    uint32_t Port::GetMaxSectorCount()
    {
        SATA_Ident test = Identifydrive();
        //uint32_t cap = (((uint32_t)test.cur_capacity1) << 16) + test.cur_capacity0;
        uint32_t cap = test.lba_capacity;
        return cap;
    }


    AHCIDriver::AHCIDriver (PCI::PCIDeviceHeader* pciBaseAddress)
    {
        this->PCIBaseAddress = pciBaseAddress;
        osData.debugTerminalWindow->Log("AHCIDriver has been created!", Colors.orange);

        ABAR = (HBAMemory*)(uint64_t)((PCI::PCIHeader0*)(uint64_t)pciBaseAddress)->BAR5;

        GlobalPageTableManager.MapMemory(ABAR, ABAR, false);
        ABAR->globalHostControl |= 0x80000000;

        ProbePorts();

        osData.debugTerminalWindow->Log("Checking {} Ports:", to_string(PortCount), Colors.bred);

        for (int i = 0; i < PortCount; i++)
        {
            Port* port = Ports[i];
            PortType portType = port->portType;

            if (portType == PortType::SATA)
                osData.debugTerminalWindow->Log("* SATA drive",  Colors.orange);
            else if (portType == PortType::SATAPI)
                osData.debugTerminalWindow->Log("* SATAPI drive",  Colors.orange);
            else
                osData.debugTerminalWindow->Log("* Not interested",  Colors.orange);


            port->Configure();

            DiskInterface::SataDiskInterface* testDiskInterface = new DiskInterface::SataDiskInterface(port);
            {
                PartitionInterface::PartitionInterfaceType diskPartType = PartitionInterface ::GetPartitionInterfaceTypeFromDisk(testDiskInterface);
                if (diskPartType == PartitionInterface::PartitionInterfaceType::mraps)
                {
                    osData.debugTerminalWindow->Log("* - Partition Type: \"MRAPS\"",  Colors.orange);
                    PartitionInterface::MRAPSPartitionInterface* diskPart = new PartitionInterface::MRAPSPartitionInterface(testDiskInterface);
                }
                else if (true)
                {
                    osData.debugTerminalWindow->Log("* - Partition Type: undefined",  Colors.orange);
                }
            }
            osData.diskInterfaces.add(testDiskInterface);

            {
                if (testDiskInterface->partitionInterface != NULL)
                {
                    PartitionInterface::GenericPartitionInterface* tDiskInterface = ((PartitionInterface::GenericPartitionInterface*)testDiskInterface->partitionInterface);
                    tDiskInterface->LoadPartitionTable();
                    
                    int pLen = tDiskInterface->partitionList.getCount();
                    //osData.debugTerminalWindow->Log("<bruh {}>", to_string(pLen),  Colors.orange);
                    for (int i = 0; i < pLen; i++)
                    {
                        PartitionInterface::PartitionInfo* p = tDiskInterface->partitionList[i];
                        if (p->type != PartitionInterface::PartitionType::Normal)
                            continue;
                        FilesystemInterface::FilesystemInterfaceType fsType = FilesystemInterface::GetFilesystemInterfaceTypeFromPartition(tDiskInterface, i);
                        if (fsType == FilesystemInterface::FilesystemInterfaceType::Mrafs)
                        {
                            osData.debugTerminalWindow->Log("  * - Filesystem Type: \"MRAFS\"",  Colors.orange);
                            FilesystemInterface::GenericFilesystemInterface* fsInterface = new FilesystemInterface::MrafsFilesystemInterface(tDiskInterface, p);
                        }
                        else if (true)
                        {
                            osData.debugTerminalWindow->Log("  * - Filesystem Type: undefined",  Colors.orange);
                        }

                        if (p->fsInterface != NULL)
                            ((FilesystemInterface::GenericFilesystemInterface*)p->fsInterface)->LoadFSTable();
                    }


                }

            }

            // if (i == 1)
            // {
            //     // Test Read 
            //     // Buffer only has 4096 bytes
            //     port->buffer = (uint8_t*)GlobalAllocator->RequestPage();
                
            //     // Read 1
            //     osData.debugTerminalWindow->Log("Preparing To Read From Disk {}...", to_string(i), Colors.yellow);
            //     _memset(port->buffer, 0, 0x1000);
            //     if (port->Read(1, 1, port->buffer))
            //     {
            //         osData.debugTerminalWindow->Log("Raw Data:");
            //         for (int t = 0; t < 256; t++)
            //         {
            //             osData.debugTerminalWindow->renderer->Print(port->buffer[t]);
            //         }
            //         osData.debugTerminalWindow->renderer->Println();
            //     }
            //     else
            //     {
            //         osData.debugTerminalWindow->Log("Reading Disk failed!");
            //     }

            //     // Write
            //     osData.debugTerminalWindow->Log("Preparing To Write To Disk {}...", to_string(i), Colors.yellow);
            //     _memset(port->buffer, 'E', 0x1000);
            //     if (port->TestWrite(0, 4, port->buffer))
            //     {
            //         osData.debugTerminalWindow->Log("Raw Data:");
            //         for (int t = 0; t < 128; t++)
            //         {
            //             osData.debugTerminalWindow->renderer->Print(port->buffer[t]);
            //         }
            //         osData.debugTerminalWindow->renderer->Println();
            //     }
            //     else
            //     {
            //         osData.debugTerminalWindow->Log("Writing to Disk failed!");
            //     }

            //     // Read 2
            //     osData.debugTerminalWindow->Log("Preparing To Read From Disk {}...", to_string(i), Colors.yellow);
            //     _memset(port->buffer, 0, 0x1000);
            //     if (port->Read(1, 1, port->buffer))
            //     {
            //         osData.debugTerminalWindow->Log("Raw Data:");
            //         for (int t = 0; t < 256; t++)
            //         {
            //             osData.debugTerminalWindow->renderer->Print(port->buffer[t]);
            //         }
            //         osData.debugTerminalWindow->renderer->Println();
            //     }
            //     else
            //     {
            //         osData.debugTerminalWindow->Log("Reading Disk failed!");
            //     }


            // }
        }
    }

    AHCIDriver::~AHCIDriver()
    {
        
    }

    #define SATA_SIG_ATAAPI 0xEB140101
    #define SATA_SIG_ATA    0x00000101
    #define SATA_SIG_SEMB   0xC33C0101
    #define SATA_SIG_PM     0x96690101
    

    void AHCIDriver::ProbePorts()
    {
        uint32_t portsImplemented = ABAR->portsImplemented;

        PortCount = 0;

        //osData.debugTerminalWindow->Log("Probing Ports:",  Colors.bred);
        for (int i = 0; i < 32; i++)
        {
            if (portsImplemented & (1 << i))
            {
                PortType portType = CheckPortType(&ABAR->ports[i]);

                // if (portType == PortType::SATA)
                //     osData.debugTerminalWindow->Log("* SATA drive",  Colors.orange);
                // else if (portType == PortType::SATAPI)
                //     osData.debugTerminalWindow->Log("* SATAPI drive",  Colors.orange);
                // else
                //     osData.debugTerminalWindow->Log("* Not interested",  Colors.orange);

                if (portType == PortType::SATA || portType == PortType::SATAPI)
                {
                    Ports[PortCount] = new Port();
                    Ports[PortCount]->portType = portType;
                    Ports[PortCount]->hbaPort = &ABAR->ports[i];
                    Ports[PortCount]->portNumber = PortCount;
                    PortCount++;
                }
            }

        }
    }

    PortType AHCIDriver::CheckPortType(HBAPort* port)
    {
        uint32_t sataStatus = port->sataStatus;

        uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
        uint8_t deviceDetection = sataStatus & 0b111;

        if (deviceDetection != HBA_PORT_DEV_PRESENT)
            return PortType::None;
        if (interfacePowerManagement != HBA_PORT_IPM_ACTIVE)
            return PortType::None;

        switch (port->signature)
        {
            case SATA_SIG_ATA:
                return PortType::SATA;
            case SATA_SIG_ATAAPI:
                return PortType::SATAPI;
            case SATA_SIG_PM:
                return PortType::PM;
            case SATA_SIG_SEMB:
                return PortType::SEMB;
            default:
                return PortType::None;
        }
    }
}