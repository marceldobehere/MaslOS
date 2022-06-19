#include "pci.h"
#include "../paging/PageTableManager.h"
#include "../OSDATA/osdata.h"

namespace PCI
{
    void EnumeratePCI(ACPI::MCFGHeader* mcfg)
    {
        AddToStack("EnumeratePCI", "pci/pci.cpp");
        int entries = (mcfg->Header.Length - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);

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
        AddToStack("EnumerateBus", "pci/pci.cpp");
        uint64_t offset = bus << 20;

        uint64_t busAddress = baseAddress + offset;

        GlobalPageTableManager.MapMemory((void*)busAddress, (void*)busAddress);
        
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
        AddToStack("EnumerateDevice", "pci/pci.cpp");
        uint64_t offset = device << 15;

        uint64_t deviceAddress = busAddress + offset;

        GlobalPageTableManager.MapMemory((void*)deviceAddress, (void*)deviceAddress);
        
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
        AddToStack("EnumerateFunction", "pci/pci.cpp");
        uint64_t offset = function << 12;

        uint64_t functionAddress = deviceAddress + offset;

        GlobalPageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);
        
        PCIDeviceHeader* pciDeviceHeader  = (PCIDeviceHeader*)functionAddress;

        if (pciDeviceHeader ->Device_ID == 0x0000) {RemoveFromStack(); return;}
        if (pciDeviceHeader ->Device_ID == 0xFFFF) {RemoveFromStack(); return;}

        BasicRenderer* renderer = osData.debugTerminalWindow->renderer;

        renderer->Print(ConvertHexToString(pciDeviceHeader->Vendor_ID));
        renderer->Print(" ");
        renderer->Print(ConvertHexToString(pciDeviceHeader->Device_ID));
        renderer->Println();
        RemoveFromStack();
    }

}