#include "ahci.h"
#include "../../../OSDATA/osdata.h"
#include "../../../paging/PageTableManager.h"

namespace AHCI 
{
    AHCIDriver::AHCIDriver (PCI::PCIDeviceHeader* pciBaseAddress)
    {
        this->PCIBaseAddress = pciBaseAddress;
        osData.debugTerminalWindow->Log("AHCIDriver has been created!", Colors.orange);

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;

        GlobalPageTableManager.MapMemory(ABAR, ABAR);

        ProbePorts();

    }

    AHCIDriver::~AHCIDriver()
    {
        
    }

    #define HBA_PORT_DEV_PRESENT 0x3
    #define HBA_PORT_IPM_ACTIVE 0x1
    #define SATA_SIG_ATAAPI 0xEB140101
    #define SATA_SIG_ATA    0x00000101
    #define SATA_SIG_SEMB   0xC33C0101
    #define SATA_SIG_PM     0x96690101
    

    void AHCIDriver::ProbePorts()
    {
        uint32_t portsImplemented = ABAR->portsImplemented;

        for (int i = 0; i < 32; i++)
        {
            if (portsImplemented & (1 << i))
            {
                PortType portType = CheckPortType(&ABAR->ports[i]);

                if (portType == PortType::SATA)
                    osData.debugTerminalWindow->Log("SATA drive",  Colors.orange);
                else if (portType == PortType::SATAPI)
                    osData.debugTerminalWindow->Log("SATAPI drive",  Colors.orange);
                else
                    osData.debugTerminalWindow->Log("Not interested",  Colors.orange);
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