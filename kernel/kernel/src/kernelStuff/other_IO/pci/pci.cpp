#include "pci.h"
#include "../../../paging/PageTableManager.h"
#include "../../../OSDATA/osdata.h"
#include "../ahci/ahci.h"

namespace PCI
{
    void EnumeratePCI(ACPI::MCFGHeader* mcfg)
    {
        AddToStack();
        int entries = (mcfg->Header.Length - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
        RemoveFromStack();
        
        AddToStack();

        for (int t = 0; t < entries; t++)
        {
            ACPI::DeviceConfig* newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + sizeof(ACPI::DeviceConfig) * t);
            for (uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++)
                EnumerateBus(newDeviceConfig->BaseAddress, bus);
        }
        RemoveFromStack();
    }

    void EnumerateBus(uint64_t baseAddress, uint64_t bus)
    {
        AddToStack();
        uint64_t offset = bus << 20;

        uint64_t busAddress = baseAddress + offset;

        //GlobalPageTableManager.MapMemory((void*)busAddress, (void*)busAddress);
        
        PCIDeviceHeader* pciDeviceHeader  = (PCIDeviceHeader*)busAddress;

        if (pciDeviceHeader ->Device_ID == 0x0000) {RemoveFromStack(); return;}
        if (pciDeviceHeader ->Device_ID == 0xFFFF) {RemoveFromStack(); return;}

        for (uint64_t device = 0; device < 32; device++)
        {
            EnumerateDevice(busAddress, device);
        }
        RemoveFromStack();
    }

    void EnumerateDevice(uint64_t busAddress, uint64_t device)
    {
        AddToStack();
        uint64_t offset = device << 15;

        uint64_t deviceAddress = busAddress + offset;

        //GlobalPageTableManager.MapMemory((void*)deviceAddress, (void*)deviceAddress);
        
        PCIDeviceHeader* pciDeviceHeader  = (PCIDeviceHeader*)deviceAddress;

        if (pciDeviceHeader ->Device_ID == 0x0000) {RemoveFromStack(); return;}
        if (pciDeviceHeader ->Device_ID == 0xFFFF) {RemoveFromStack(); return;}

        for (uint64_t function = 0; function < 8; function++)
        {
            EnumerateFunction(deviceAddress, function);
        }
        RemoveFromStack();
    }

    void EnumerateFunction(uint64_t deviceAddress, uint64_t function)
    {
        AddToStack();
        uint64_t offset = function << 12;

        uint64_t functionAddress = deviceAddress + offset;

        if (functionAddress == 0x0000000000000000) {RemoveFromStack(); return;}

        //GlobalPageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);
        
        PCIDeviceHeader* pciDeviceHeader  = (PCIDeviceHeader*)functionAddress;

        if (pciDeviceHeader ->Device_ID == 0x0000) {RemoveFromStack(); return;}
        if (pciDeviceHeader ->Device_ID == 0xFFFF) {RemoveFromStack(); return;}

        BasicRenderer* renderer = osData.debugTerminalWindow->renderer;


        {
            const char* vendorName = GetVendorName(pciDeviceHeader->Vendor_ID);
            if (vendorName != unknownString)
                renderer->Print(vendorName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Vendor_ID));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* deviceName = GetDeviceName(pciDeviceHeader->Vendor_ID, pciDeviceHeader->Device_ID);
            if (deviceName != unknownString)
                renderer->Print(deviceName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Device_ID));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* className = GetClassName(pciDeviceHeader->Class);
            if (className != unknownString)
                renderer->Print(className);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Class));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* subclassName = GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->SubClass);
            if (subclassName != unknownString)
                renderer->Print(subclassName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->SubClass));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* progIFName = GetProgIFName(pciDeviceHeader->Class, pciDeviceHeader->SubClass, pciDeviceHeader->Prog_IF);
            if (progIFName != unknownString)
                renderer->Print(progIFName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Prog_IF));
                renderer->Print(">");
            }
            //renderer->Print(" / ");
        }
        renderer->Println();

        switch (pciDeviceHeader->Class)
        {
            case 0x01:
            {
                switch (pciDeviceHeader->SubClass)
                {
                    case 0x06:
                    {
                        switch (pciDeviceHeader->Prog_IF)
                        {
                            case 0x01:
                            {
                                new AHCI::AHCIDriver(pciDeviceHeader);
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }





        RemoveFromStack();
    }

}